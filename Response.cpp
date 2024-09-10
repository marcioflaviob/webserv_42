/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrandao <mbrandao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 22:20:57 by mbrandao          #+#    #+#             */
/*   Updated: 2024/09/10 23:21:33 by mbrandao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
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

Response::Response(HTTPStatus status, RequestType requestType) : _status(status), _requestType(requestType) {

}

Response::~Response() {

}

std::string	Response::getMessage(HTTPStatus status) {
	switch (status) {
		case OK:
			return "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";
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

RequestType	Response::getRequestType() {
	return _requestType;
}

HTTPStatus	Response::getStatus() {
	return _status;
}

std::string	Response::getResponse() {
	return _response;
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

void		Response::appendResponse(std::string str) {
	this->_response.append(str);
}

void Response::send_response(int client_fd, Route & route) {
	setResponse(getMessage(getStatus()));
	
	std::string response = route.getHtml(getStatus());

	std::cout << "[Server] Sending response to client " << client_fd << std::endl;
	std::cout << "Response: " << response << std::endl;

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