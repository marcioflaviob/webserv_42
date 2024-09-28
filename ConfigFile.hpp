/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trimize <trimize@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 19:14:42 by trimize           #+#    #+#             */
/*   Updated: 2024/09/27 21:51:11 by trimize          ###   ########.fr       */
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
#include <vector>
#include <map>

#include "Route.hpp"
#include "ServerConfig.hpp"

class ConfigFile
{
	private:
		
		std::string path;

		std::vector<pollfd> poll_fds;
		std::vector<Client> clients;
		std::vector<int> server_sockets;
		std::map<int, ServerConfig> server_socket_map;

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

		RequestType formatType(std::string request);
		
		std::vector<pollfd> & getPoll_fds();
		std::vector<Client> & getClients();
		std::vector<int> & getServer_sockets();
		std::map<int, ServerConfig> & getServer_socket_map();

		void	setPoll_fds(std::vector<pollfd> poll_fds);
		void	setClients(std::vector<Client> clients);
		void	setServer_sockets(std::vector<int> server_sockets);
		void	setServer_socket_map(std::map<int, ServerConfig> server_socket_map);

		Client & add_to_poll_fds(std::vector<pollfd> & poll_fds, std::vector<Client> & clients, int fd, ServerConfig & server);
		void remove_from_poll_fds(std::vector<pollfd> & poll_fds, std::vector<Client> & clients, int fd);
};

#endif