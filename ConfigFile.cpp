/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trimize <trimize@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 19:18:24 by trimize           #+#    #+#             */
/*   Updated: 2024/09/07 22:13:08 by trimize          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigFile.hpp"

ConfigFile::ConfigFile()
{
	this->path = "./config.conf";
}

ConfigFile::ConfigFile(std::string path)
{
	this->path = path;
}

ConfigFile::ConfigFile(const ConfigFile &cf)
{
	this->port = cf.port;
	this->host = cf.host;
	this->error = cf.error;
	this->bodysize = cf.bodysize;
	this->path = cf.path;
}

ConfigFile& ConfigFile::operator=(const ConfigFile &cf)
{
	if(this != &cf)
	{
		this->port = cf.port;
		this->host = cf.host;
		this->error = cf.error;
		this->bodysize = cf.bodysize;
		this->path = cf.path;
	}
	return (*this);
}

ConfigFile::~ConfigFile()
{
	
}

std::string	ConfigFile::getPath()
{
	return(this->path);
}

std::string	ConfigFile::getHost()
{
	return(this->host);
}

std::string	ConfigFile::getError()
{
	return(this->error);
}

int		ConfigFile::getPort()
{
	return(this->port);
}

int		ConfigFile::getBodySize()
{
	return(this->bodysize);
}

void		ConfigFile::setPath(std::string path)
{
	this->path = path;
}

bool		ConfigFile::isAllDigits(const std::string &str)
{
    for (std::size_t i = 0; i < str.size(); ++i)
    {
	if (!isdigit(str[i]))
		return false;
    }
    return true;
}

bool		ConfigFile::fileExists(const std::string& filePath)
{
    return (access(filePath.c_str(), F_OK) != -1);
}

void		ConfigFile::fillVariables()
{
	std::ifstream file(this->getPath().c_str());
	std::string tmp;
	std::size_t pos;
	std::string rm_part;

	if (file.is_open())
	{
		std::getline(file, tmp);
		if (tmp.empty())
			throw ConfigFile::InvalidPort();
		pos = tmp.find('=') + 1;
		rm_part = tmp.substr(0, pos);
		if (rm_part == "PORT=")
		{
			tmp.erase(0, pos);
			std::stringstream ss(tmp);
			ss >> this->port;
			if (this->port <= 0 || this->port > 65535 || !this->isAllDigits(tmp))
				throw ConfigFile::InvalidPort();
		}
		else
			throw ConfigFile::InvalidPort();

		std::getline(file, this->host);
		if (this->host.empty())
			throw ConfigFile::InvalidHost();
		pos = this->host.find('=') + 1;
		rm_part = this->host.substr(0, pos);
		if (rm_part == "HOST=")
		{
			this->host.erase(0, pos);
			if (this->host != "localhost" && this->host != "127.0.0.1")
				throw ConfigFile::InvalidHost();		
		}
		else
			throw ConfigFile::InvalidHost();
		
		std::getline(file, this->error);
		if (this->error.empty())
			throw ConfigFile::InvalidErrorFile();
		pos = this->error.find('=') + 1;
		rm_part = this->error.substr(0, pos);
		if (rm_part == "ERROR_PAGE=")
		{
			this->error.erase(0, pos);
			if (!this->fileExists(this->error))
				throw ConfigFile::InvalidErrorFile();		
		}
		else
			throw ConfigFile::InvalidErrorFile();

		std::getline(file, tmp);
		if (tmp.empty())
			throw ConfigFile::InvalidBodySize();
		pos = tmp.find('=') + 1;
		rm_part = tmp.substr(0, pos);
		if (rm_part == "BODY_SIZE=")
		{
			tmp.erase(0, pos);
			std::stringstream ss(tmp);
			ss >> this->bodysize;
			if (this->bodysize <= 0 || this->bodysize > 10024 || !this->isAllDigits(tmp))
				throw ConfigFile::InvalidBodySize();
		}
		else
			throw ConfigFile::InvalidBodySize();
	}
	else
		std::cout << "Unable to open config file" << std::endl;
}