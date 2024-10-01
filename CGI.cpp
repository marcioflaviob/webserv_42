/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: svydrina <svydrina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 21:39:55 by svydrina          #+#    #+#             */
/*   Updated: 2024/10/01 21:14:23 by svydrina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

CGI::CGI(){}

CGI::CGI(Request req, std::string path): _path(path)
{
	// std::cout << "Hello from CGI constructor" << std::endl;
	// std::cout << "What's the reques type?" << getType(req.getType()) << std::endl;

	_req = req;
	_script_path =  getScriptName();
	std::map<std::string, std::string> req_headers = req.getHeaders();
	for (std::map<std::string, std::string>::iterator it = req_headers.begin();
		it != req_headers.end(); it++)
		{
			std::string name = it->first;
			std::transform(name.begin(), name.end(), name.begin(), ::toupper);
			std::replace(name.begin(), name.end(), '-', '_');
			std::string key = "HTTP_" + name;
			if (key != "HTTP_CONTENT_TYPE" && key != "HTTP_CONTENT_LENGTH")
				_env[key] = (it->second).substr(1);
		}
	_env["REQUEST_METHOD"] = getType(req.getType());
	_env["REQUEST_URI"] = "/" + req.getPath();
	_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	_env["SERVER_SOFTWARE"] = "webserv";
	_env["SERVER_NAME"] = "localhost";
	_env["SERVER_PORT"] = "4142";
	//_env["REMOTE_ADDR"] = "";
	_env["REMOTE_PORT"] = "";
	//_env["SCRIPT_NAME"] = "";
//	_env["SCRIPT_NAME"] = _path;
	_env["SCRIPT_NAME"] = "/" + _script_path;
//	_env["SCRIPT_FILENAME"] = _path;
	// _env["PATH_INFO"] = "";
	// _env["PATH_TRANSLATED"] = "";
	_env["PATH"] = getenv("PATH");
	//_env["PATH_INFO"] = _path;
	//_env["PATH_TRANSLATED"] = _path;
	//_env["QUERY_STRING"] = "";
	_env["QUERY_STRING"] = getQuery();
	_env["AUTH_TYPE"] = "";
	_env["REMOTE_USER"] = "";
	_env["REMOTE_IDENT"] = "";
	_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	_env["REDIRECT_STATUS"] = "200";
	if (_env["REQUEST_METHOD"] == "POST")
	{
		_env["CONTENT_TYPE"] = req.getHeader("Content-Type").substr(1);
		_env["CONTENT_LENGTH"] = req.getHeader("Content-Length").substr(1);
	}

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

std::string CGI::getQuery()
{
	size_t pos = _req.getPath().find("?");

	if (pos == std::string::npos)
		return std::string("");
	return _req.getPath().substr(pos + 1);
	
}

std::string CGI::getScriptName()
{
	size_t pos = _req.getPath().find("?");

	if (pos == std::string::npos)
		return _req.getPath();
	return _req.getPath().substr(0, pos);
}

std::string CGI::getType(int type)
{
	switch (type)
	{
		case 0: 
			return "GET";
		case 1:
			return "POST";
		case 2:
			return "DELETE";
		default:
			return "UNDEFINED";
	}
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

void CGI::setEnvVar(std::string key, std::string value)
{
	_env[key] = value;
}

static void printMap(std::map<std::string, std::string> headers)
{
	for(std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
	{
		// std::cout << CYAN "key: " RESET<< it->first;
		// std::cout << CYAN " value: " RESET << it->second << std::endl;
		std::cout << it->first << "=" << it->second << std::endl;
	}
}

Response CGI::executeCGI()
{
	pid_t pid;
	int pipes[2];
	std::string responseBody;
	int ret;
	char **env = env_map_to_string();
	Response response;
	char *const argv[] = {const_cast<char *>(_script_path.c_str()), NULL};

	// std::cout << "Are we executing the cgi?" << std::endl;
	// std::cout << "What's the content length?" << _req.getHeader("Content-Length") << std::endl;
	// std::cout << "Is there a query string? " << _env["QUERY_STRING"] << std::endl;
	// std::cout << "What's the script name? " << _env["SCRIPT_NAME"] << std::endl;
	
	// std::cout << "What's the gateway interface? " << _env["GATEWAY_INTERFACE"] << std::endl;
	// std::cout << "What's the request uri? " << _env["REQUEST_URI"] << std::endl;
	printMap(_env);

	std::cout << CYAN"script path is: " RESET << _script_path << std::endl;
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
		std::cout << "Ok are we in the child process?" << std::endl;
		dup2(pipes[1], STDOUT_FILENO);
		dup2(pipes[0], STDIN_FILENO);
		close(pipes[1]);
		close(pipes[0]);
		//std::cout << "Did we make it all the way to execve?" << std::endl;
		//execve(_path.c_str(), argv, env);
		
		execve(_script_path.c_str(), argv, env);
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
		std::cout << "CGI kinda succeded" << std::endl;
		while ((ret = read(pipes[0], buffer, BufferSize - 1)) > 0)
		{
			buffer[BufferSize - 1] = '\0';
			responseBody.append(buffer, ret);
			//kinda like memset
			for(size_t i = 0; i < BufferSize; i++)
				buffer[i] = '\0';
		}
		
	}
	close (pipes[0]);
	close(pipes[1]);
	//std::cout << "Response body: " << responseBody << std::endl;
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

	
	

	
	
