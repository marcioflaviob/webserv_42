/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trimize <trimize@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 21:39:55 by svydrina          #+#    #+#             */
/*   Updated: 2024/10/02 19:49:32 by trimize          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

CGI::CGI(){}

CGI::CGI(Request * req, std::string path): _path(path)
{
	size_t boundaryPos = req->getHeader("Content-Type").find("boundary=");
	std::string boundary = "";
	_req = req;
	_env["REQUEST_METHOD"] = req->getType();
	_env["REQUEST_URI"] = req->getPath();
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
	_env["CONTENT_TYPE"] = req->getHeader("Content-Type");
	_env["CONTENT_LENGTH"] = req->getHeader("Content-Length");
	_env["AUTH_TYPE"] = "";
	_env["REMOTE_USER"] = "";
	_env["REMOTE_IDENT"] = "";
	if (boundaryPos != std::string::npos)
		boundary = req->getHeader("Content-Type").substr(boundaryPos + 9);
	else
	{
		//std::cerr << "Boundary not found in Content-Type header." << std::endl;
		boundary = "";
	}
	_env["BOUNDARY"] = boundary;
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

std::string	CGI::getPath()
{
	return (this->_path);
}

bool isExecutable(const std::string& filePath)
{
	struct stat fileStat;
	
	// Get file status
	if (stat(filePath.c_str(), &fileStat) != 0)
	{
		std::cerr << "stat failed" << std::endl;
		return false;
	}
	// Check if it's a regular file and if it has execute permissions
	return S_ISREG(fileStat.st_mode) && (fileStat.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH));
}

char** CGI::env_map_to_string(){
	char **env = new char*[_env.size() + 1];
	int i = 0;
	for (std::map<std::string, std::string>::iterator it = _env.begin(); it != _env.end(); it++)
	{
		 env[i] = new char[it->first.size() + it->second.size() + 2];
		 std::string tmp = it->first + "=" + it->second;
		 size_t j = 0;
		 while (j < tmp.size())
		 {
			env[i][j] = tmp[j];
			j++;
		 }
		 env[i][j] = '\0';
		i++;
		
	}
	env[i] = NULL;
	return (env);
}


Response * CGI::executeCGI()
{
	pid_t pid;
	int pipes[2];
	int pipes_2[2];
	std::string responseBody = "";
	int ret;
	char **env = env_map_to_string();
	Response * response = new Response();
	//std::cout << "This is in cgi :";
	//std::cout << std::endl << std::endl << "Request Length : " << _req->getRaw() << std::endl << std::endl;
	if(pipe(pipes) == -1)
	{
		std::cerr << "Pipe failed" << std::endl;
		return new Response(INTERNAL_SERVER_ERROR, _req->getType(), _req);
	}
	if(pipe(pipes_2) == -1)
	{
		std::cerr << "Pipe failed" << std::endl;
		return new Response(INTERNAL_SERVER_ERROR, _req->getType(), _req);
	}
	pid = fork();
	if (pid == -1)
	{
		std::cerr << "Fork failed" << std::endl;
		return new Response(INTERNAL_SERVER_ERROR, _req->getType(), _req);
	}
	if (pid == 0)
	{
		dup2(pipes[1], 1);
		close(pipes[1]);
		dup2(pipes_2[0], 0);
		close(pipes_2[0]);
		close(pipes_2[1]);
		close(pipes[0]);
		this->_path = "./" + this->_path;
		std::string tmp_path = this->_path;
		std::string tmp = this->_path;
		std::ifstream script_file;
		if (isExecutable(_path))
		{
			const char *argv[] = {tmp.c_str(), NULL};
			execve(argv[0], (char *const *)argv, env);	
		}
		else
		{
			script_file.open(_path.c_str());
			std::string line;
			std::getline(script_file, line);
			tmp = line.erase(0, 2);	
			const char *argv[] = {tmp.c_str(), tmp_path.c_str(), NULL};
			execve(argv[0], (char *const *)argv, env);
		}
	}
	else
	{
		char buffer[BufferSize];
		int status;
		write(pipes_2[1], _req->getRaw().c_str(), _req->getRaw().length());
		close(pipes_2[1]);
		close(pipes_2[0]);
		close(pipes[1]);
		waitpid(-1, &status, WNOHANG);
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
		{
			std::cerr << "CGI failed" << std::endl;
			return new Response(INTERNAL_SERVER_ERROR, _req->getType(), _req);
		}
		while ((ret = read(pipes[0], buffer, BufferSize)) > 0)
		{
			//memset(buffer, 0, BufferSize);
			//std::cout << "BUFFER IS: " << buffer << std::endl;
			responseBody.append(buffer, ret);
		}
		close(pipes[0]);
		
	}
	for (size_t i = 0; env[i]; i++)
	{
		delete [] env[i];
	}
	delete [] env;
	//std::cout << responseBody << std::endl;
	response->setResponse(responseBody);
	response->setStatus(OK);
	response->setRequestType(_req->getType());
	return response;
}

	
	

	
	
