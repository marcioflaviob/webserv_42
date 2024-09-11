/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: svydrina <svydrina@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 21:40:33 by svydrina          #+#    #+#             */
/*   Updated: 2024/09/11 21:55:03 by svydrina         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once
#include <iostream>
#include <string>
#include <map>
#include "Request.hpp"
#include "Enums.hpp"

class Request;

class CGI
{
private:
	std::map<std::string, std::string> _env;
public:
	CGI(/* args */);
	CGI(Request req);
	CGI(const CGI &copy);
	CGI &operator=(const CGI &copy);
	~CGI();
	std::map<std::string, std::string> getEnv() const;
};


