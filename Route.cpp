/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trimize <trimize@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 21:04:39 by mbrandao          #+#    #+#             */
/*   Updated: 2024/10/02 09:54:27 by trimize          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Route.hpp"
#include "ConfigFile.hpp"
#include <sys/stat.h>

Route::Route() {

}

Route::Route(std::string path, std::string root, std::string index, std::vector<RequestType> allowedMethods) : _path(path), _root(root), _index(index), _allowedMethods(allowedMethods) {
	
}

Route::~Route() {

}

std::vector<RequestType> Route::getAllowedMethods() {
	return _allowedMethods;
}

std::string	Route::getPath() {
	return _path;
}

std::string	Route::getRoot() {
	return _root;
}

std::string	Route::getIndex() {
	return _index;
}

std::string Route::getHtml(HTTPStatus status, std::string good_path, ServerConfig & server) {
	if (status == OK || status == CREATED || status == ACCEPTED) {
		std::string filePath; 
		
		struct stat info;
		if (stat(good_path.c_str(), &info) != 0 || !S_ISDIR(info.st_mode)) {
			//std::cout << "Path is not a directory" << std::endl;
			if (!S_ISREG(info.st_mode)) {
				//std::cout << "Root: " << _root << std::endl;
				filePath = _index;
				//std::cout << "File path: " << filePath << std::endl;
				//std::cout << "Path is not a file" << std::endl;
			}
			else {
				filePath = good_path;
				//std::cout << "Path is a file" << std::endl;
			}
		} else {
			//std::cout << "Path is a directory" << std::endl;
			filePath = _root + _index;
		}

		if (filePath[0] == '/') {
			filePath = filePath.substr(1);
		}
		if (filePath.empty()) {
			filePath = "index.html";
		}

		//std::cout << "File path: " << filePath << std::endl;
		
		std::ifstream file(filePath.c_str());

		if (!file.is_open()) {
			//std::cout << "SOMETHING WENT WRONG" << std::endl;
			return ""; // TODO THROW EXCEPTION MAYBE
		}

		std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		
		file.close();
		return content;
	} else {
		std::string filePath = server.getError();
		
		std::ifstream file(filePath.c_str());

		if (!file.is_open()) {
			return ""; // TODO THROW EXCEPTION MAYBE
		}

		std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		
		file.close();
		return content;
	}
}

void Route::addAllowedMethod(RequestType method) {
	std::vector<RequestType>::iterator it = std::find(_allowedMethods.begin(), _allowedMethods.end(), method);
	if (it == _allowedMethods.end()) {
		_allowedMethods.push_back(method);
	}
}

void Route::setPath(std::string path) {
	this->_path = path;
}

void Route::setRoot(std::string root) {
	this->_root = root;
}

void Route::setIndex(std::string index) {
	this->_index = index;
}

bool Route::isMethodAllowed(RequestType method) {
	std::vector<RequestType>::iterator it = std::find(_allowedMethods.begin(), _allowedMethods.end(), method);
	if (it == _allowedMethods.end()) {
		return false;
	}
	return true;
}
