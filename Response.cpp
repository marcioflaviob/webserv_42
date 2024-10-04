/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trimize <trimize@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 22:20:57 by mbrandao          #+#    #+#             */
/*   Updated: 2024/10/04 15:53:57 by trimize          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "ConfigFile.hpp"

#include <errno.h>
#include <netdb.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <iterator>
#include <iostream>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

Response::Response(HTTPStatus status, RequestType requestType, Request * request) : _status(status), _requestType(requestType), _request(request){
	_route = NULL;
}

Response::Response(HTTPStatus status, RequestType requestType, Route * route, std::string path, Request * request) : _status(status), _requestType(requestType), _route(route), _adjusted_path(path), _request(request) {

}

Response::Response() {
	_route = NULL;
}

Response::~Response() {
	delete _request;
}

std::string	sendSetCookies(int id)
{
	std::stringstream	out;
	std::string		sessionid_str;
	out << id;
	sessionid_str = out.str();	
	std::string string = "Set-Cookie: session_id=" + sessionid_str + "; Path=/; Secure; SameSite=Strict\r\nSet-Cookie: text=PLACEHOLDER; Path=/; Secure; SameSite=Strict\r\n";
	return (string);
}

std::string	Response::getMessage(HTTPStatus status, int client_fd) {
	std::string str;
	if (this->_request->getHeader("Cookie").empty())
		str = sendSetCookies(client_fd);
	switch (status) {
		case OK:
			return "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n" + str + "Content-Length: ";
		case CREATED:
			return "HTTP/1.1 201 Created\r\nContent-Type: text/html\r\nContent-Length: ";
		case ACCEPTED:
			return "HTTP/1.1 202 Accepted\r\nContent-Type: text/html\r\nContent-Length: ";
		case NO_CONTENT:
			return "HTTP/1.1 204 No Content\r\nContent-Type: text/html\r\nContent-Length: ";
		case BAD_REQUEST:
			return "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: ";
		case FORBIDDEN:
			return "HTTP/1.1 403 Forbidden\r\nContent-Type: text/html\r\nContent-Length: ";
		case NOT_FOUND:
			return "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: ";
		case INTERNAL_SERVER_ERROR:
			return "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\nContent-Length: ";
		case SERVICE_UNAVAILABLE:
			return "HTTP/1.1 503 Service Unavailable\r\nContent-Type: text/html\r\nContent-Length: ";
		case GATEWAY_TIMEOUT:
			return "HTTP/1.1 504 Gateway Timeout\r\nCintent-Type: text/html\r\nContent-Length: ";
		case PAYLOAD_TOO_LARGE:
			return "HTTP/1.1 413 Content too large\r\nCintent-Type: text/html\r\nContent-Length: ";
		default:
			return "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\nContent-Length: ";
	}
}

void Response::send_cgi_response(int client_fd) {
	std::string response = _response;
	setResponse(getMessage(getStatus(), client_fd));
	
	// std::string response = _response;

	//std::cout << "\n\n\n\n\n" << std::endl;
	//std::cout << "Response: " << response << std::endl;
	//std::cout << "\n\n\n\n\n" << std::endl;

	//std::cout << "[Server] Sending response to client " << client_fd << std::endl;
	//std::cout << "Response: " << response << std::endl;

	std::stringstream ss;
	ss << response.size();
	appendResponse(ss.str());
	appendResponse("\r\n\r\n");
	appendResponse(response);
	
	std::string finalResponse = getResponse();
	size_t messageSize = finalResponse.size();
	const char* message = finalResponse.c_str();

	int send_status = send(client_fd, message, messageSize, 0);
	if (send_status == -1 || send_status == 0)
	{
		close(client_fd);
		ConfigFile config = ConfigFile::getInstance();
		config.remove_from_poll_fds(config.getPoll_fds(), config.getClients(), client_fd);
		std::cout << "[Server] Closed connection on client socket " << client_fd << std::endl;
    	}
}

RequestType	Response::getRequestType() {
	return _requestType;
}

HTTPStatus	Response::getStatus() {
	return _status;
}

std::string	Response::getResponse() {
	return _response;
}

Route *		Response::getRoute() {
	return _route;
}

std::string	Response::getAdjustedPath() {
	return _adjusted_path;
}

void		Response::setAdjustedPath(std::string path) {
	_adjusted_path = path;
}

void Response::setRoute(Route * route) {
	this->_route = route;
}

void		Response::setRequestType(RequestType requestType) {
	this->_requestType = requestType;
}

void		Response::setStatus(HTTPStatus status) {
	this->_status = status;
}

void		Response::setResponse(std::string response) {
	this->_response = response;
}

Request &		Response::getRequest() {
	return *_request;
}

void		Response::setRequest(Request * request) {
	this->_request = request;
}

void		Response::appendResponse(std::string str) {
	this->_response.append(str);
}

std::string		indexHtml(std::string path) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        std::cerr << "Error getting current working directory" << std::endl;
        return "<html><body><h1>Error getting current working directory</h1></body></html>";
    }

    std::string baseDir(cwd);

	if (baseDir[baseDir.size() - 1] != '/') {
		baseDir += "/";
	}

    // Adjust the path to use the base directory if the path is "/"
    if (path == "/") {
        path = baseDir;
    } else {
        path = baseDir + path;
    }

    std::string html = "<html><head><title>Index of " + path + "</title></head><body>";
    html += "<h1>Index of " + path + "</h1><ul>";

    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(path.c_str())) != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            std::string name = entry->d_name;
            if (entry->d_type == DT_DIR) {
                name += "/";
            }
            html += "<li><a href=\"" + name + "\">" + name + "</a></li>";
        }
        closedir(dir);
    } else {
        // Could not open directory
        html += "<li>Could not open directory</li>";
    }

    html += "</ul></body></html>";
    return html;
}

void Response::send_response(Client & client) {
	int client_fd = client.getFd();
	
	std::string str = getMessage(getStatus(), client_fd);

	setResponse(str);
	
	if (getStatus() == OK || getStatus() == ACCEPTED || getStatus() == CREATED) {
		struct stat info;
		if (stat(getAdjustedPath().c_str(), &info) != 0 || !S_ISDIR(info.st_mode)) {
			//std::cout << "Path is not a directory" << std::endl;
			if (!S_ISREG(info.st_mode)) {
				setAdjustedPath(getRoute()->getIndex());
				//std::cout << "Path is not a file" << std::endl;
			}
			else {
				//std::cout << "Path is a file" << std::endl;
				if (getAdjustedPath()[0] == '/') {
					setAdjustedPath(getAdjustedPath().substr(1));
				}
			}
		// } else {
			// std::cout << "Path is a directory" << std::endl;
			if (getRoute()->getIndex() == "*AUTO*" && (!S_ISDIR(info.st_mode) && !S_ISREG(info.st_mode))) {
				std::string tmp = getAdjustedPath();

				if (tmp == "*AUTO*") {
					tmp = "";
				} else if (tmp[tmp.size() - 1] != '/') {
					tmp += "/";
				}
				if (tmp[0] == '/') {
					tmp = tmp.substr(1);
				}

				
				setAdjustedPath(getRoute()->getRoot() + tmp);

				//std::cout << "Adjusted path crazy scenario: " << getAdjustedPath() << std::endl;
			} else if (S_ISDIR(info.st_mode) /*|| S_ISREG(info.st_mode)*/) {
				setAdjustedPath(getRoute()->getRoot() + getRoute()->getIndex());
			}
		}

		if (getAdjustedPath()[0] == '/') {
			setAdjustedPath(getAdjustedPath().substr(1));
		}
		if (getAdjustedPath().empty()) {
			setAdjustedPath("index.html");
		}
	}

	std::string response;

	struct stat info;
	stat(getAdjustedPath().c_str(), &info);

	if (!S_ISREG(info.st_mode)) {
		//std::cout << "Path is not a file" << std::endl;
	}

	if (this->_route != NULL && this->_route->getIndex() == "*AUTO*" && (!S_ISREG(info.st_mode) || getAdjustedPath() == "index.html") && (getStatus() == OK || getStatus() == ACCEPTED || getStatus() == CREATED)) {
		if (getAdjustedPath() == "index.html")
			setAdjustedPath(this->_route->getRoot());
		response = indexHtml(this->_route->getRoot());
	} else {
		response = _route->getHtml(getStatus(), getAdjustedPath(), client.getServer());
	}

	if (response == "") {
		setStatus(INTERNAL_SERVER_ERROR);
		std::string tmpstr = getMessage(getStatus(), client_fd);
		setResponse(tmpstr);
		response = _route->getHtml(getStatus(), getAdjustedPath(), client.getServer());
	}

	//std::cout << "[Server] Sending response to client " << client_fd << std::endl;
	//std::cout << "Response: " << response << std::endl;

	std::stringstream ss;
	ss << response.size();
	appendResponse(ss.str());
	appendResponse("\r\n\r\n");
	appendResponse(response);
	
	std::string finalResponse = getResponse();
	size_t messageSize = finalResponse.size();
	const char* message = finalResponse.c_str();

	int send_status = send(client_fd, message, messageSize, 0);
	// send_status value checked in the next if

	ConfigFile config = ConfigFile::getInstance();

	if ((client.getRequest().getHeader("Connection").find("keep-alive") == std::string::npos && !client.getError()) || send_status == -1 || send_status == 0)
	{
		close(client_fd);
		config.remove_from_poll_fds(config.getPoll_fds(), config.getClients(), client_fd);
		std::cout << "[Server] Closed connection on client socket " << client_fd << std::endl;
    	}
	client.setResponse(NULL);
	if (!client.getError())
		client.setStatus(DONE);
}
