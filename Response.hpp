/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trimize <trimize@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 22:18:15 by mbrandao          #+#    #+#             */
/*   Updated: 2024/09/20 16:09:45 by trimize          ###   ########.fr       */
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
#include "Request.hpp"

class Route;

class Response {
	private:
		HTTPStatus _status;
		RequestType _requestType;
		std::string _response;
		Route * _route;
		std::string _adjusted_path;
		Request * _request;
		
	public:
		Response(HTTPStatus status, RequestType requestType, Request & request);
		Response(HTTPStatus status, RequestType requestType, Route * route, std::string path, Request & request);
		Response();
		~Response();
		
		std::string	getMessage(HTTPStatus status, int client_fd);

		RequestType	getRequestType();
		HTTPStatus	getStatus();
		Route *		getRoute();
		std::string	getAdjustedPath();
		std::string	getResponse();
		Request &	getRequest();
		
		void		setRoute(Route * route);
		void		setRequest(Request & request);
		void		setRequestType(RequestType requestType);
		void		setStatus(HTTPStatus status);
		void		setResponse(std::string response);
		void		setAdjustedPath(std::string path);
		void		appendResponse(std::string response);

		void send_response(int client_fd);
		void send_cgi_response(int client_fd);
};

#endif