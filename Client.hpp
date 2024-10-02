/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trimize <trimize@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/20 17:45:35 by mbrandao          #+#    #+#             */
/*   Updated: 2024/10/01 19:14:09 by trimize          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <sys/poll.h>

#include "Enums.hpp"
#include "ServerConfig.hpp"

class Response;
class Request;
class ServerConfig;

class Client {
	private:
		struct pollfd _pollfd;
		ClientStatus _status;
		Response * _response;
		ServerConfig * _server;
		int	_totalBytes;
		std::string _rawRequest;
		int contentLength;
		bool error;
	
	public:

		Client();
		Client(pollfd pollfd, ClientStatus status);
		Client(pollfd pollfd, ClientStatus status, Response & response);
		~Client();
		
		int			getFd();
		pollfd		getPollfd();
		ClientStatus	getStatus();
		Request &		getRequest();
		Response &		getResponse();
		ServerConfig &		getServer();
		std::string 		getRawRequest();
		int			getTotalBytes();
		int			getContentLength();
		bool			getError();

		void 		setResponse(Response * response);
		void 		setPollfd(pollfd pollfd);
		void		setStatus(ClientStatus status);
		void		setServer(ServerConfig & server);
		void		setRawRequest(std::string raw);
		void		setTotalBytes(int bytes);
		void		setContentLength(int contentLength);
		void		setError(bool error);


};

#endif