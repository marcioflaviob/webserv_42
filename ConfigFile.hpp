/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrandao <mbrandao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 19:14:42 by trimize           #+#    #+#             */
/*   Updated: 2024/09/26 22:22:58 by mbrandao         ###   ########.fr       */
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
#include "ServerConfig.hpp"

class ConfigFile
{
	private:
		
		std::string path;
		// ConfigFile();
		ConfigFile(std::string path);
		std::vector<ServerConfig> servers;
		// ConfigFile(const ConfigFile &cf);
		// ConfigFile& operator=(const ConfigFile &cf);

		static ConfigFile* instance;
		
	public:
		static void initialize(std::string path);
		static ConfigFile & getInstance();
	
		// Constructors/Destructor
		~ConfigFile();

		std::vector<ServerConfig> getServers() const;
		void parseConfig(const std::string &path);

		// int		getPort();
		// int		getBodySize();
		// void		setPath(std::string path);
		// std::string	getPath();
		// std::string	getError();
		// std::string	getHost();
		// std::string	getRoot();
		// std::vector<Route *>	getRoutes();

		RequestType formatType(std::string request);
		
		// void		addRoute(Route * route);

		// bool		isAllDigits(const std::string &str);
		// bool		fileExists(const std::string& filePath);
		// void		fillRoutes(std::string str);
		// bool		isPathValid(const std::string & path);
		// void		fillVariables();
		// Route	*	getRoute(std::string path);
};

#endif