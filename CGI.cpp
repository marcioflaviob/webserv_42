/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: svydrina <svydrina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 21:39:55 by svydrina          #+#    #+#             */
/*   Updated: 2024/09/11 21:58:09 by svydrina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

CGI::CGI(){}

CGI::CGI(Request req)
{
	_env["REQUEST_METHOD"] = req.getType();
	_env["REQUEST_URI"] = req.getPath();
	_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	_env["SERVER_SOFTWARE"] = "webserv";
	_env["SERVER_NAME"] = "localhost";
	_env["SERVER_PORT"] = "4142";
	_env["REMOTE_ADDR"] = "";
	_env["REMOTE_PORT"] = "";
	_env["SCRIPT_NAME"] = "";
	_env["PATH_INFO"] = "";
	_env["PATH_TRANSLATED"] = "";
	_env["QUERY_STRING"] = "";
	_env["CONTENT_TYPE"] = req.getHeader("Content-Type");
	_env["CONTENT_LENGTH"] = req.getHeader("Content-Length");
	_env["AUTH_TYPE"] = "";
	_env["REMOTE_USER"] = "";
	_env["REMOTE_IDENT"] = "";
}

CGI::CGI(const CGI &copy)
{
	_env = copy._env;
}

CGI &CGI::operator=(const CGI &copy)
{
	_env = copy._env;
	return (*this);
}

CGI::~CGI(){}

std::map<std::string, std::string> CGI::getEnv() const{
	return (_env);
}