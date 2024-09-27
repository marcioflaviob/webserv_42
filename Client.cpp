/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trimize <trimize@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/20 17:49:37 by mbrandao          #+#    #+#             */
/*   Updated: 2024/09/27 17:14:51 by trimize          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Response.hpp"
#include "Request.hpp"

Client::Client() : _response(NULL) {

}

Client::Client(pollfd pollfd, ClientStatus status) : _pollfd(pollfd), _status(status), _response(NULL) {

}

Client::Client(pollfd pollfd, ClientStatus status, Response & response) : _pollfd(pollfd), _status(status), _response(&response) {

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
