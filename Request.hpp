/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrandao <mbrandao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 20:55:35 by mbrandao          #+#    #+#             */
/*   Updated: 2024/09/16 20:56:25 by mbrandao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Enums.hpp"
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <iterator>
#include <algorithm>

#define RED "\033[31m"
#define CYAN "\033[36m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"

class Request {
	private:
		RequestType _type;
		std::string _path;
		std::string _body;
		std::map<std::string, std::string> _headers;
		bool isCgi;
	
	public:
		Request();
		~Request();

		std::string getBody();
		std::string getPath();
		RequestType getType();
		bool getIsCgi();
		std::map<std::string, std::string> getHeaders();
		std::string getHeader(std::string key);
		
		void setBody(std::string body);
		void setPath(std::string path);
		void setType(RequestType type);
		void setIsCgi(bool isCgi);
		void addHeader(std::string key, std::string value);

		void fillVariables(std::string request);

		class	KeyNotFound : public std::exception {
		public:
			virtual const char* what() const throw() {
					return ("Key not found");
				};
		};

		class	BadRequest : public std::exception {
		public:
			virtual const char* what() const throw() {
					return ("Request invalid");
				};
		};
};

#endif