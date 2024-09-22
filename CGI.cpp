/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trimize <trimize@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 21:39:55 by svydrina          #+#    #+#             */
/*   Updated: 2024/09/20 16:06:07 by trimize          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

CGI::CGI(){}

CGI::CGI(Request req, std::string path): _path(path)
{
	_req = req;
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



Response CGI::executeCGI()
{
	pid_t pid;
	int pipes[2];
	std::string responseBody;
	int ret;
	char **env = env_map_to_string();
	Response response;

	if(pipe(pipes) == -1)
	{
		std::cerr << "Pipe failed" << std::endl;
		return Response(INTERNAL_SERVER_ERROR, _req.getType(), _req);
	}
	pid = fork();
	if (pid == -1)
	{
		std::cerr << "Fork failed" << std::endl;
		return Response(INTERNAL_SERVER_ERROR, _req.getType(), _req);
	}
	if (pid == 0)
	{
		char *const argv[] = {NULL};
		dup2(pipes[1], 1);
		dup2(pipes[0], 0);
		close(pipes[1]);
		close(pipes[0]);
		execve(_path.c_str(), argv, env);
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
			
			return Response(INTERNAL_SERVER_ERROR, _req.getType(), _req);
		}
		while ((ret = read(pipes[0], buffer, BufferSize - 1)) > 0)
		{
			buffer[BufferSize - 1] = '\0';			std::cout << "BUFFER IS: " << buffer << std::endl;
			responseBody.append(buffer, ret);
			//kinda like memset
			for(size_t i = 0; i < BufferSize; i++)
				buffer[i] = '\0';
		}
		
	}
	close (pipes[0]);
	close(pipes[1]);
	for (size_t i = 0; env[i]; i++)
	{
		delete [] env[i];
	}
	delete [] env;
	response.setResponse(responseBody);
	response.setStatus(OK);
	response.setRequestType(_req.getType());
	return response;
}

	
	

	
	
