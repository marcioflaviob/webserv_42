/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: trimize <trimize@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/07 20:42:51 by trimize           #+#    #+#             */
/*   Updated: 2024/09/07 22:10:30 by trimize          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigFile.hpp"

int	main(int argc, char **argv)
{
	ConfigFile cf;

	if (argc < 2)
		cf.setPath("./config.conf");
	else
		cf.setPath(argv[1]);

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
}