/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbrandao <mbrandao@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 20:42:51 by trimize           #+#    #+#             */
/*   Updated: 2024/09/13 11:46:24 by mbrandao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigFile.hpp"

int	main(int argc, char **argv)
{

	if (argc < 2)
		ConfigFile::initialize("./config.conf");
	else
		ConfigFile::initialize(argv[1]);

	ConfigFile & cf = ConfigFile::getInstance();
		
	try
	{
		cf.fillVariables();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	std::cout << "The port is " << cf.getPort() << std::endl;
	std::cout << "The host is " << cf.getHost() << std::endl;
	std::cout << "The error page is " << cf.getError() << std::endl;
	std::cout << "The body size is " << cf.getBodySize() << std::endl;
	std::vector<Route> routes = cf.getRoutes();
	std::vector<Route>::iterator it;
	for (it = routes.begin(); it != routes.end(); it++)
	{
		std::cout << "Route path: " << it->getPath() << std::endl;
		std::cout << "Route root: " << it->getRoot() << std::endl;
		std::cout << "Route index: " << it->getIndex() << std::endl;
		std::vector<RequestType> methods = it->getAllowedMethods();
		std::vector<RequestType>::iterator it2;
		for (it2 = methods.begin(); it2 != methods.end(); it2++)
		{
			std::cout << "Method: " << *it2 << std::endl;
		}
	}
	return (0);
}