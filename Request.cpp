/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrandao <mbrandao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 20:56:47 by mbrandao          #+#    #+#             */
/*   Updated: 2024/09/11 22:02:43 by mbrandao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

RequestType formatType(std::string request) {
    if (request == "GET") {
        return GET;
    }
    else if (request == "POST") {
        return POST;
    }
    else if (request == "DELETE") {
        return DELETE;
    }
    return UNDEFINED;
}

void Request::addHeader(std::string key, std::string value) {
	_headers[key] = value;
}

void Request::fillVariables(std::string request) {
	std::istringstream request_stream(request);
	std::string method, path;
	request_stream >> method >> path;

	if (method.empty() || path.empty()) {
		throw BadRequest();
	}

	_type = formatType(method);
	_path = path;

	std::string line;
	bool is_first = true;
	while (std::getline(request_stream, line)) {
		if (is_first) {
			is_first = false;
			continue;
		}
		
		if (line.empty() || line == "\r") {
			break;
		}
		
		std::size_t pos = line.find(":");

		if (pos == std::string::npos) {
			throw BadRequest();
		}
		
		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos + 1);
		addHeader(key, value);
	}
}

Request::Request() {

}

Request::~Request() {

}

std::string Request::getBody() {
	return _body;
}

std::string Request::getPath() {
	return _path;
}

RequestType Request::getType() {
	return _type;
}

std::map<std::string, std::string> Request::getHeaders() {
	return _headers;
}

std::string Request::getHeader(std::string key) {
	std::map<std::string, std::string>::iterator it = _headers.find(key);
	if (it == _headers.end()) {
		return "";
	}
	return it->second;
}

void Request::setBody(std::string body) {
	_body = body;
}

void Request::setPath(std::string path) {
	_path = path;
}

void Request::setType(RequestType type) {
	_type = type;
}

