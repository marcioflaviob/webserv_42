/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrandao <mbrandao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 20:55:35 by mbrandao          #+#    #+#             */
/*   Updated: 2024/09/13 11:17:56 by mbrandao         ###   ########.fr       */
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

class Request {
	private:
		RequestType _type;
		std::string _path;
		std::string _body;
		std::map<std::string, std::string> _headers;
	
	public:
		Request();
		~Request();

		std::string getBody();
		std::string getPath();
		RequestType getType();
		std::map<std::string, std::string> getHeaders();
		std::string getHeader(std::string key);
		
		void setBody(std::string body);
		void setPath(std::string path);
		void setType(RequestType type);
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