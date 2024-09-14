/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrandao <mbrandao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/10 20:54:29 by mbrandao          #+#    #+#             */
/*   Updated: 2024/09/13 20:56:11 by mbrandao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTE_HPP
#define ROUTE_HPP

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <fstream>

#include "Response.hpp"
#include "Enums.hpp"


class Route {
	private:
		std::string		_path;
		std::string		_root;
		std::string		_index;
		std::vector<RequestType>	_allowedMethods;
		
	public:
		Route(std::string path, std::string root, std::string index, std::vector<RequestType> allowedMethods);
		Route();
		~Route();
		
		std::vector<RequestType>	getAllowedMethods();
		std::string	getPath();
		std::string	getRoot();
		std::string	getIndex();
		std::string getHtml(HTTPStatus status);

		void		addAllowedMethod(RequestType method);
		void		setPath(std::string path);
		void		setRoot(std::string root);
		void		setIndex(std::string index);

		bool		isMethodAllowed(RequestType method);

};

#endif