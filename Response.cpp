/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trimize <trimize@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 22:20:57 by mbrandao          #+#    #+#             */
/*   Updated: 2024/09/28 19:48:32 by trimize          ###   ########.fr       */
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
	if (send_status == -1) {
		std::cerr << "[Server] Send error to client " << client_fd << std::endl;
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
	
	std::string response;

	if (this->_route != NULL && this->_route->getIndex() == "*AUTO*" && getAdjustedPath() == this->_route->getRoot()) {
		response = indexHtml(this->_route->getRoot());
	} else {
		response = _route->getHtml(getStatus(), getAdjustedPath(), client.getServer());
	}

	std::cout << "[Server] Sending response to client " << client_fd << std::endl;
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
	if (send_status == -1) {
		std::cerr << "[Server] Send error to client " << client_fd << std::endl;
	}

	ConfigFile config = ConfigFile::getInstance();

	if (client.getRequest().getHeader("Connection").find("keep-alive") == std::string::npos) {
		close(client_fd);
		config.remove_from_poll_fds(config.getPoll_fds(), config.getClients(), client_fd);
		std::cout << "[Server] Closed connection on client socket " << client_fd << std::endl;
    	}
	client.setResponse(NULL);
	client.setStatus(DONE);
}
