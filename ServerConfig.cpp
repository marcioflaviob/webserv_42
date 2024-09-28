/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrandao <mbrandao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/24 19:24:21 by mbrandao          #+#    #+#             */
/*   Updated: 2024/09/28 14:12:42 by mbrandao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <sys/stat.h>

#include "ServerConfig.hpp"

static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
}

RequestType formatType2(std::string request) {
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

std::string	ServerConfig::getHost()
{
	return(this->host);
}

std::string	ServerConfig::getError()
{
	return(this->error);
}

int		ServerConfig::getPort()
{
	return(this->port);
}

int		ServerConfig::getBodySize()
{
	return(this->bodysize);
}

std::vector<Route *>	ServerConfig::getRoutes()
{
	return(this->routes);
}

bool ServerConfig::isPathValid(const std::string & path)
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

Route * ServerConfig::getRoute(std::string path) {
    std::vector<Route *>::iterator it;

    for (it = this->routes.begin(); it != this->routes.end(); it++) {
        if ((*it)->getPath() == path)
            return *it;
    }
    throw ServerConfig::RouteNotFound();
}

bool		ServerConfig::isAllDigits(const std::string &str)
{
    for (std::size_t i = 0; i < str.size(); ++i)
    {
	if (!isdigit(str[i]))
		return false;
    }
    return true;
}

bool		ServerConfig::fileExists(const std::string& filePath)
{
    return (access(filePath.c_str(), F_OK) != -1);
}

void	ServerConfig::fillRoutes(std::string str) {
    std::istringstream stream(str);
    std::string line;
    Route * route;
    bool inRoute = false;

    while (std::getline(stream, line))
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
                    route->addAllowedMethod(formatType2(method));
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
                if (route->getIndex() != "*AUTO*") {
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
                        throw ServerConfig::InvalidIndex();
                    }
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

void ServerConfig::fillVariables(std::string str)
{
    std::istringstream stream(str);
    std::string line;
    std::size_t pos;
    std::string key, value;

    while (std::getline(stream, line))
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
                throw ServerConfig::InvalidPort();
        }
        else if (key == "HOST=")
        {
            this->host = value;
            if (this->host != "localhost" && this->host != "127.0.0.1")
                throw ServerConfig::InvalidHost();
        }
        else if (key == "ERROR_PAGE=")
        {
            this->error = value;
            if (!this->fileExists(this->error))
                throw ServerConfig::InvalidErrorFile();
        }
        else if (key == "BODY_SIZE=")
        {
            std::stringstream ss(value);
            ss >> this->bodysize;
            if (this->bodysize <= 0 || this->bodysize > 10024 || !this->isAllDigits(value))
                throw ServerConfig::InvalidBodySize();
        }
        else if (key == "ROOT=")
        {
            this->root = value;
            struct stat info;
            if (stat(this->root.c_str(), &info) != 0 || !S_ISDIR(info.st_mode))
                throw ServerConfig::InvalidRoot();
        }
        else if (key == "ROUTE=")
        {
            fillRoutes(str);
            break; // Assuming ROUTE is the last configuration in the file
        }
        else
        {
            throw std::runtime_error("Unknown configuration key: " + key);
        }
    }
}
