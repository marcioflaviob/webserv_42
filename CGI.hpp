/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: svydrina <svydrina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 21:40:33 by svydrina          #+#    #+#             */
/*   Updated: 2024/09/16 19:37:11 by svydrina         ###   ########.fr       */
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
	Request _req;
	std::string getType(int type);
	std::string getQuery();
	std::string getScriptName();
	std::string _script_path;
	void upload_execve();
public:
	CGI(/* args */);
	CGI(Request req, std::string path);
	CGI(const CGI &copy);
	CGI &operator=(const CGI &copy);
	~CGI();
	std::map<std::string, std::string> getEnv() const;
	
	char** env_map_to_string(); 
	void setEnvVar(std::string key, std::string value);
	Response executeCGI();
};


