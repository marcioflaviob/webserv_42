/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: svydrina <svydrina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 21:39:55 by svydrina          #+#    #+#             */
/*   Updated: 2024/09/13 20:06:02 by svydrina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

CGI::CGI(){}

CGI::CGI(Request req, std::string path): _path(path)
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

char** CGI::env_map_to_string(){
	char **env = new char*[_env.size() + 1];
	int i = 0;
	for (std::map<std::string, std::string>::iterator it = _env.begin(); it != _env.end(); it++)
	{
		env[i] = new char[it->first.size() + it->second.size() + 2];
		std::string tmp = it->first + "=" + it->second;
		strcpy(env[i], tmp.c_str());
		i++;
		
	}
	env[i] = NULL;
	return (env);
}

void CGI::executeCGI()
{
	pid_t pid;
	int pipes[2];
	std::string responseBody;
	int ret;
	char **env = env_map_to_string();

	if(pipe(pipes) == -1)
	{
		std::cerr << "Pipe failed" << std::endl;
		return;
	}
	pid = fork();
	if (pid == -1)
	{
		std::cerr << "Fork failed" << std::endl;
	}
	if (pid == 0)
	{
		close(pipes[1]);
		dup2(pipes[0], 0);
		close(pipes[0]);
		execve(_path.c_str(), NULL, env);
	}
	else{
		char buffer[BufferSize];
		int status;

		waitpid(-1, &status, 0);
		close(pipes[1]);
		dup2(pipes[0], 0);
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
		{
			std::cerr << "CGI failed" << std::endl;
			return;
		}
		while ((ret = read(pipes[0], buffer, BufferSize)) > 0)
		{
			responseBody.append(buffer, ret);
			//memset(buffer, 0, BufferSize);
		}
		
	}
	close (pipes[0]);
	close(pipes[1]);
	for (size_t i = 0; env[i]; i++)
	{
		delete [] env[i];
	}
	delete [] env;
	std::cout << responseBody << std::endl;
}

	
	

	
	
