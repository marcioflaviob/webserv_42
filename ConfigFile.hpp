/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrandao <mbrandao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 19:14:42 by trimize           #+#    #+#             */
/*   Updated: 2024/09/10 23:00:36 by mbrandao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGFILE_HPP
#define CONFIGFILE_HPP

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <unistd.h>

#include "Route.hpp"

class ConfigFile
{
	private:
		int		port;
		std::string	host;
		std::string	error;
		int		bodysize;
		std::string	path;
		std::vector<Route>	routes;
		
		ConfigFile(const ConfigFile &cf);
		ConfigFile& operator=(const ConfigFile &cf);
		
	public:
		// Constructors/Destructor
		ConfigFile();
		ConfigFile(std::string path);
		~ConfigFile();

		int		getPort();
		int		getBodySize();
		std::string	getPath();
		std::string	getError();
		std::string	getHost();
		std::vector<Route>	getRoutes();
		
		void		addRoute(Route route);

		bool		isAllDigits(const std::string &str);
		bool		fileExists(const std::string& filePath);
		void		fillVariables();
		bool		isPathValid(const std::string & path);
		Route	&	getRoute(std::string & path);

		class	InvalidPort : public std::exception {
		public:
			virtual const char* what() const throw() {
					return ("Port informed in the config file is invalid.");
				};
		};
		
		class	InvalidHost : public std::exception {
		public:
			virtual const char* what() const throw() {
					return ("Host informed in the config file is invalid.");
				};
		};

		class	InvalidErrorFile : public std::exception {
		public:
			virtual const char* what() const throw() {
					return ("Error page informed in the config file is invalid.");
				};
		};

		class	InvalidBodySize : public std::exception {
		public:
			virtual const char* what() const throw() {
					return ("Body Size informed in the config file is invalid.");
				};
		};

		class	RouteNotFound : public std::exception {
		public:
			virtual const char* what() const throw() {
					return ("Route not found.");
				};
		};
};

#endif