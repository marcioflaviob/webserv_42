/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrandao <mbrandao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 19:18:24 by trimize           #+#    #+#             */
/*   Updated: 2024/09/16 21:30:13 by mbrandao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigFile.hpp"
#include "Request.hpp"
#include <sys/stat.h>

ConfigFile * ConfigFile::instance = NULL;

static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
}

RequestType ConfigFile::formatType(std::string request) {
    if (request == "GET") {
        return GET;
    }
    else if (request == "POST") {
        return POST;
    }
    else if (request == "DELETE") {
        return DELETE;
    }
    return UNDEFINED;
}

// ConfigFile::ConfigFile()
// {
// 	this->path = "./config.conf";
// }

ConfigFile::ConfigFile(std::string path)
{
	this->path = path;
    fillVariables();
}

void ConfigFile::initialize(std::string path) {
    if (instance == NULL) {
        instance = new ConfigFile(path);
    }
}

ConfigFile& ConfigFile::getInstance() {
    return *instance;
}


// ConfigFile::ConfigFile(const ConfigFile &cf)
// {
// 	this->port = cf.port;
// 	this->host = cf.host;
// 	this->error = cf.error;
// 	this->bodysize = cf.bodysize;
// 	this->path = cf.path;
// }

// ConfigFile& ConfigFile::operator=(const ConfigFile &cf)
// {
// 	if(this != &cf)
// 	{
// 		this->port = cf.port;
// 		this->host = cf.host;
// 		this->error = cf.error;
// 		this->bodysize = cf.bodysize;
// 		this->path = cf.path;
// 	}
// 	return (*this);
// }

ConfigFile::~ConfigFile()
{
	
}

void ConfigFile::setPath(std::string path)
{
    this->path = path;
}

void	ConfigFile::fillRoutes(std::string str) {
    std::ifstream file(str.c_str());
    std::string line;
    Route * route;
    bool inRoute = false;

    if (file.is_open())
    {
        while (std::getline(file, line))
        {
            ltrim(line);
            if (line.find("ROUTE=") == 0)
            {
                if (inRoute)
                {
                    this->routes.push_back(route);
                }
                inRoute = true;
                route = new Route();
                route->setPath(line.substr(6, line.find(" {") - 6));
                if (route->getPath()[0] == '/' && route->getPath().size() > 1)
                    route->setPath(route->getPath().substr(1));
                if (route->getPath().empty())
                    route->setPath("/");
                // std::cout << "PATH=" << route->getPath() << std::endl;
            }
            else if (inRoute)
            {
                if (line.find("ALLOW=") == 0)
                {
                    std::string methods = line.substr(6);
                    std::stringstream ss(methods);
                    std::string method;
                    while (std::getline(ss, method, ',')) {
                        route->addAllowedMethod(formatType(method));
                    }
                }
                else if (line.find("ROOT=") == 0)
                {
                    route->setRoot(line.substr(5));
                    if (route->getRoot()[route->getRoot().size() - 1] != '/')
                        route->setRoot(route->getRoot() + "/");
                    if (route->getRoot()[0] == '/' && route->getRoot().size() > 1)
                        route->setRoot(route->getRoot().substr(1));
                }
                else if (line.find("INDEX=") == 0)
                {
                    route->setIndex(line.substr(6));
                    std::string index = route->getRoot() + route->getIndex();
                    if (index[0] == '/' && index.size() > 1)
                        route->setIndex(index.substr(1));
                    else
                        route->setIndex(index);
                    if (route->getIndex().empty())
                        route->setIndex("index.html");
                    struct stat info;
                    if (stat(route->getIndex().c_str(), &info) != 0 || !S_ISREG(info.st_mode)) {
                        std::cout << "Index is " << route->getIndex() << std::endl;
                        throw ConfigFile::InvalidIndex();
                    }
                }
                else if (line.find("}") == 0)
                {
                    this->routes.push_back(route);
                    inRoute = false;
                }
            }
        }
        if (inRoute)
        {
            this->routes.push_back(route);
        }
    }
    else
    {
        std::cout << "Unable to open config file" << std::endl;
    }
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

std::vector<Route *>	ConfigFile::getRoutes()
{
	return(this->routes);
}

bool ConfigFile::isPathValid(const std::string & path)
{
    std::vector<Route *>::iterator it;

    for (it = this->routes.begin(); it != this->routes.end(); it++)
    {
        std::cout << "Comparing " << (*it)->getPath() << " with " << path << std::endl;
        if ((*it)->getPath() == path)
            return true;
    }
    return false;
}

Route * ConfigFile::getRoute(std::string path) {
    std::vector<Route *>::iterator it;

    for (it = this->routes.begin(); it != this->routes.end(); it++) {
        if ((*it)->getPath() == path)
            return *it;
    }
    throw ConfigFile::RouteNotFound();
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

void ConfigFile::fillVariables()
{
    std::ifstream file(this->getPath().c_str());
    std::string line;
    std::size_t pos;
    std::string key, value;

    if (file.is_open())
    {
        while (std::getline(file, line))
        {
            if (line.empty())
                continue;

            pos = line.find('=');
            if (pos == std::string::npos)
                throw std::runtime_error("Invalid configuration line: " + line);

            key = line.substr(0, pos + 1);
            value = line.substr(pos + 1);

            if (key == "PORT=")
            {
                std::stringstream ss(value);
                ss >> this->port;
                if (this->port <= 0 || this->port > 65535 || !this->isAllDigits(value))
                    throw ConfigFile::InvalidPort();
            }
            else if (key == "HOST=")
            {
                this->host = value;
                if (this->host != "localhost" && this->host != "127.0.0.1")
                    throw ConfigFile::InvalidHost();
            }
            else if (key == "ERROR_PAGE=")
            {
                this->error = value;
                if (!this->fileExists(this->error))
                    throw ConfigFile::InvalidErrorFile();
            }
            else if (key == "BODY_SIZE=")
            {
                std::stringstream ss(value);
                ss >> this->bodysize;
                if (this->bodysize <= 0 || this->bodysize > 10024 || !this->isAllDigits(value))
                    throw ConfigFile::InvalidBodySize();
            }
            else if (key == "ROOT=")
            {
                this->root = value;
                struct stat info;
                if (stat(this->root.c_str(), &info) != 0 || !S_ISDIR(info.st_mode))
                    throw ConfigFile::InvalidRoot();
            }
            else if (key == "ROUTE=")
            {
                fillRoutes(this->getPath());
                break; // Assuming ROUTE is the last configuration in the file
            }
            else
            {
                throw std::runtime_error("Unknown configuration key: " + key);
            }
        }
    }
    else
    {
        std::cout << "Unable to open config file" << std::endl;
    }
}

// void		ConfigFile::fillVariables()
// {
// 	std::ifstream file(this->getPath().c_str());
// 	std::string tmp;
// 	std::size_t pos;
// 	std::string rm_part;

// 	if (file.is_open())
// 	{
// 		std::getline(file, tmp);
// 		if (tmp.empty())
// 			throw ConfigFile::InvalidPort();
// 		pos = tmp.find('=') + 1;
// 		rm_part = tmp.substr(0, pos);
// 		if (rm_part == "PORT=")
// 		{
// 			tmp.erase(0, pos);
// 			std::stringstream ss(tmp);
// 			ss >> this->port;
// 			if (this->port <= 0 || this->port > 65535 || !this->isAllDigits(tmp))
// 				throw ConfigFile::InvalidPort();
// 		}
// 		else
// 			throw ConfigFile::InvalidPort();

// 		std::getline(file, this->host);
// 		if (this->host.empty())
// 			throw ConfigFile::InvalidHost();
// 		pos = this->host.find('=') + 1;
// 		rm_part = this->host.substr(0, pos);
// 		if (rm_part == "HOST=")
// 		{
// 			this->host.erase(0, pos);
// 			if (this->host != "localhost" && this->host != "127.0.0.1")
// 				throw ConfigFile::InvalidHost();		
// 		}
// 		else
// 			throw ConfigFile::InvalidHost();
		
// 		std::getline(file, this->error);
// 		if (this->error.empty())
// 			throw ConfigFile::InvalidErrorFile();
// 		pos = this->error.find('=') + 1;
// 		rm_part = this->error.substr(0, pos);
// 		if (rm_part == "ERROR_PAGE=")
// 		{
// 			this->error.erase(0, pos);
// 			if (!this->fileExists(this->error))
// 				throw ConfigFile::InvalidErrorFile();		
// 		}
// 		else
// 			throw ConfigFile::InvalidErrorFile();

// 		std::getline(file, tmp);
// 		if (tmp.empty())
// 			throw ConfigFile::InvalidBodySize();
// 		pos = tmp.find('=') + 1;
// 		rm_part = tmp.substr(0, pos);
// 		if (rm_part == "BODY_SIZE=")
// 		{
// 			tmp.erase(0, pos);
// 			std::stringstream ss(tmp);
// 			ss >> this->bodysize;
// 			if (this->bodysize <= 0 || this->bodysize > 10024 || !this->isAllDigits(tmp))
// 				throw ConfigFile::InvalidBodySize();
// 		}
// 		else
// 			throw ConfigFile::InvalidBodySize();
// 	}
// 	else
// 		std::cout << "Unable to open config file" << std::endl;
// }