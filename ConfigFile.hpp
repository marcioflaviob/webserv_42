/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrandao <mbrandao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 19:14:42 by trimize           #+#    #+#             */
/*   Updated: 2024/09/13 14:15:00 by mbrandao         ###   ########.fr       */
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
		std::string root;
		std::vector<Route>	routes;
		
		// ConfigFile();
		ConfigFile(std::string path);
		// ConfigFile(const ConfigFile &cf);
		// ConfigFile& operator=(const ConfigFile &cf);

		static ConfigFile* instance;
		
	public:
		static void initialize(std::string path);
		static ConfigFile & getInstance();
	
		// Constructors/Destructor
		~ConfigFile();

		int		getPort();
		int		getBodySize();
		void		setPath(std::string path);
		std::string	getPath();
		std::string	getError();
		std::string	getHost();
		std::string	getRoot();
		std::vector<Route>	getRoutes();

		RequestType formatType(std::string request);
		
		void		addRoute(Route route);

		bool		isAllDigits(const std::string &str);
		bool		fileExists(const std::string& filePath);
		void		fillVariables();
		void		fillRoutes(std::string str);
		bool		isPathValid(const std::string & path);
		Route	&	getRoute(std::string path);

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