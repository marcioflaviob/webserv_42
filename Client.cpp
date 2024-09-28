/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trimize <trimize@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/20 17:49:37 by mbrandao          #+#    #+#             */
/*   Updated: 2024/09/28 15:22:52 by trimize          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Response.hpp"
#include "Request.hpp"

Client::Client() : _response(NULL), _totalBytes(0) {
	_rawRequest = "";
}

Client::Client(pollfd pollfd, ClientStatus status) : _pollfd(pollfd), _status(status), _response(NULL), _totalBytes(0) {
	_rawRequest = "";
}

Client::Client(pollfd pollfd, ClientStatus status, Response & response) : _pollfd(pollfd), _status(status), _response(&response), _totalBytes(0) {
	_rawRequest = "";
}

Client::~Client() {

}

int			Client::getFd() {
	return _pollfd.fd;
}

pollfd		Client::getPollfd() {
	return _pollfd;
}

ClientStatus	Client::getStatus() {
	return _status;
}

Request &		Client::getRequest() {
	return _response->getRequest();
}

Response &		Client::getResponse() {
	return *_response;
}

ServerConfig &		Client::getServer() {
	return *_server;
}

void		Client::setServer(ServerConfig & server) {
	_server = &server;
}

void 		Client::setResponse(Response * response) {
	_response = response;
}

void 		Client::setPollfd(pollfd pollfd) {
	_pollfd = pollfd;
}

void		Client::setStatus(ClientStatus status) {
	_status = status;
}

int		Client::getTotalBytes() {
	return this->_totalBytes;
}

void		Client::setTotalBytes(int bytes) {
	this->_totalBytes = bytes;
}


std::string	Client::getRawRequest() {
	return _rawRequest;
}

void		Client::setRawRequest(std::string raw) {
	_rawRequest = raw;
}
