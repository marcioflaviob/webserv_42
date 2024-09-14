/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: svydrina <svydrina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 22:18:15 by mbrandao          #+#    #+#             */
/*   Updated: 2024/09/14 21:51:41 by svydrina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>


#include "Route.hpp"
#include "Enums.hpp"
#include "CGI.hpp"

class Route;
class CGI;

class Response {
	private:
		HTTPStatus _status;
		RequestType _requestType;
		std::string _response;
		Route * _route;
		//bool isCgi;
		
	public:
		Response(HTTPStatus status, RequestType requestType);
		Response(HTTPStatus status, RequestType requestType, Route & route);
		~Response();
		
		std::string	getMessage(HTTPStatus status);

		RequestType	getRequestType();
		HTTPStatus	getStatus();
		Route &		getRoute();
		std::string	getResponse();
		bool		getIsCgi();
		
		void		setRoute(Route & route);
		void		setRequestType(RequestType requestType);
		void		setStatus(HTTPStatus status);
		void		setResponse(std::string response);
		void		appendResponse(std::string response);

		void send_response(int client_fd);
};

#endif