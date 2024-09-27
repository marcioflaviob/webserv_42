/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrandao <mbrandao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 21:40:33 by svydrina          #+#    #+#             */
/*   Updated: 2024/09/27 14:18:22 by mbrandao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once
#include <iostream>
#include <string>
#include <string.h>
#include <map>
#include <unistd.h>
#include <sys/wait.h>
#include "Request.hpp"
#include "Enums.hpp"
#include "Response.hpp"

#define BufferSize 1024

class Request;
class Response;

class CGI
{
private:
	std::map<std::string, std::string> _env;
	std::string _path;
	Request * _req;

public:
	CGI(/* args */);
	CGI(Request * req, std::string path);
	CGI(const CGI &copy);
	CGI &operator=(const CGI &copy);
	~CGI();
	std::map<std::string, std::string> getEnv() const;
	char** env_map_to_string(); 
	Response * executeCGI();
};


