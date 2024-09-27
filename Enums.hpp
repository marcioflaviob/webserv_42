/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Enums.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trimize <trimize@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 23:14:43 by mbrandao          #+#    #+#             */
/*   Updated: 2024/09/27 15:58:47 by trimize          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ENUMS_HPP
#define ENUMS_HPP

enum RequestType {
	GET,
	POST,
	DELETE,
	UNDEFINED
};

enum HTTPStatus {
	OK = 200,
	CREATED = 201,
	ACCEPTED = 202,
	NO_CONTENT = 204,
	BAD_REQUEST = 400,
	UNAUTHORIZED = 401,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	PAYLOAD_TOO_LARGE = 413,
	INTERNAL_SERVER_ERROR = 500,
	NOT_IMPLEMENTED = 501,
	SERVICE_UNAVAILABLE = 503
};

enum ClientStatus {
	READ,
	WRITE,
	DONE
};

#endif