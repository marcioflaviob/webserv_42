/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrandao <mbrandao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/24 19:17:22 by mbrandao          #+#    #+#             */
/*   Updated: 2024/09/26 22:36:32 by mbrandao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "Route.hpp"

class Route;

class ServerConfig {

	private:
	
	    int port;
		std::string host;
		std::string error;
		int bodysize;
		std::string root;
		std::vector<Route *> routes;


	public:

		void fillVariables(std::string str);
		
		int		getPort();
		int		getBodySize();
		void		setPath(std::string path);
		std::string	getPath();
		std::string	getError();
		std::string	getHost();
		std::string	getRoot();
		std::vector<Route *>	getRoutes();

		bool		isAllDigits(const std::string &str);
		bool		fileExists(const std::string& filePath);
		void		fillRoutes(std::string str);
		bool		isPathValid(const std::string & path);
		Route	*	getRoute(std::string path);

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

		class	InvalidRoot : public std::exception {
		public:
			virtual const char* what() const throw() {
					return ("Root is not a directory.");
				};
		};

		class	InvalidIndex : public std::exception {
		public:
			virtual const char* what() const throw() {
					return ("Index is not a file.");
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