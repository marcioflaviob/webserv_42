/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrandao <mbrandao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 22:18:15 by mbrandao          #+#    #+#             */
/*   Updated: 2024/09/10 23:14:59 by mbrandao         ###   ########.fr       */
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

class Route;

class Response {
	private:
		HTTPStatus _status;
		RequestType _requestType;
		std::string _response;
		
	public:
		Response(HTTPStatus status, RequestType requestType);
		~Response();
		
		std::string	getMessage(HTTPStatus status);

		RequestType	getRequestType();
		HTTPStatus	getStatus();
		std::string	getResponse();
		
		void		setRequestType(RequestType requestType);
		void		setStatus(HTTPStatus status);
		void		setResponse(std::string response);
		void		appendResponse(std::string response);

		void send_response(int client_fd, Route & route);
};

#endif