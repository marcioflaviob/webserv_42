/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: svydrina <svydrina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 21:04:39 by mbrandao          #+#    #+#             */
/*   Updated: 2024/09/14 20:58:11 by svydrina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Route.hpp"
#include "ConfigFile.hpp"

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

std::string Route::getHtml(HTTPStatus status) {
	if (status == OK || status == CREATED || status == ACCEPTED) {
		std::string filePath = _root + _index;

		std::cout << "File path: " << filePath << std::endl;
		
		std::ifstream file(filePath.c_str());

		if (!file.is_open()) {
			std::cout << "SOMETHING WENT WRONG" << std::endl;
			return ""; // TODO THROW EXCEPTION MAYBE
		}

		std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		
		file.close();
		return content;
	} else {
		ConfigFile config = ConfigFile::getInstance();
		std::string filePath = config.getError();
		
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


