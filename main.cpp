#include <errno.h>
#include <netdb.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <map>
#include <iterator>
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>

#include "Route.hpp"
#include "Response.hpp"
#include "Enums.hpp"
#include "Request.hpp"
#include "ConfigFile.hpp"
#include "CGI.hpp"
#include "Client.hpp"
#include "ServerConfig.hpp"

int create_server_socket(ServerConfig & server);
void accept_new_connection(int server_socket, std::vector<pollfd> & poll_fds, std::vector<Client> & clients, ServerConfig & server);
int read_data_from_socket(Client & client);
Response * request_dealer(Request * request, Client & client);
void handle_client_request(Client & client, std::vector<pollfd> & poll_fds, std::vector<Client> & clients);
// void del_from_poll_fds(struct pollfd **poll_fds, int i, int *poll_count);

int main(void)
{
    int status;
    ConfigFile::initialize("./config.conf");
    ConfigFile config = ConfigFile::getInstance();


    // To monitor client sockets:
    // struct pollfd *poll_fds; // Array of socket file descriptors
	std::vector<pollfd> poll_fds;
    std::vector<Client> clients;
    std::vector<int> server_sockets;
    std::map<int, ServerConfig> server_socket_map;

    config.setPoll_fds(poll_fds);
    config.setClients(clients);
    config.setServer_sockets(server_sockets);
    config.setServer_socket_map(server_socket_map);

    for(size_t i = 0; i < config.getServers().size(); i++) {
        int server_socket = create_server_socket(config.getServers()[i]);
        if (server_socket == -1) {
            return (1);
        }
        status = listen(server_socket, 3000);
        if (status != 0) {
            std::cerr << "[Server] Listen error: " << std::endl;
            return (3);
        }
        config.add_to_poll_fds(poll_fds, clients, server_socket, config.getServers()[i]);
        server_sockets.push_back(server_socket);
        server_socket_map[server_socket] = config.getServers()[i];
    }

    // // Create server listening socket
    // server_socket = create_server_socket();
    // if (server_socket == -1) {
    //     return (1);
    // }

    // // Listen to port via socket
    // status = listen(server_socket, 3000);
    // if (status != 0) {
	// 	std::cerr << "[Server] Listen error: " << std::endl;
    //     return (3);
    // }

    // // Add the listening server socket to array
	// add_to_poll_fds(poll_fds, clients, server_socket);

	//std::cout << "[Server] Set up poll fd array" << std::endl;

    while (1) { // Main loop
        // Poll sockets to see if they are ready (2 second timeout)
        status = poll(poll_fds.data(), poll_fds.size(), 2000);

        if (status == -1) {
			std::cerr << "[Server] Poll error: " << std::endl;
            exit(1);
        }
        else if (status == 0) {
            // None of the sockets are ready
			//std::cout << "[Server] Waiting..." << std::endl;
            continue;
        }

        for (size_t i = 0; i < clients.size(); i++) {
            if ((poll_fds[i].revents & POLLIN) != 0) {
                if (std::find(server_sockets.begin(), server_sockets.end(), clients[i].getFd()) != server_sockets.end()) {
                    accept_new_connection(clients[i].getFd(), poll_fds, clients, server_socket_map[clients[i].getFd()]);
                } else {
                    handle_client_request(clients[i], poll_fds, clients);
                    //clients[i].setStatus(WRITE);
                    //poll_fds[i].events = POLLOUT;
                }
            } else if ((poll_fds[i].revents & POLLOUT) != 0) {
                if (clients[i].getStatus() == WRITE) {
                    if (clients[i].getRequest().getIsCgi()) {
                        clients[i].getResponse().send_cgi_response(clients[i].getFd());
                    } else {
                        clients[i].getResponse().send_response(clients[i]);
                    }
		    if (clients[i].getError())
                   	clients[i].setStatus(DONE);
                    poll_fds[i].events = POLLIN;
                }
            } else if (poll_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
				std::cerr << "[Server] Error on socket " << poll_fds[i].fd << std::endl;
				config.remove_from_poll_fds(poll_fds, clients, poll_fds[i].fd);
			}
        }
    }
    return (0);
}

int create_server_socket(ServerConfig & server) {
    // Prepare the address and port for the server socket
    struct sockaddr_in sa;
    int socket_fd;
    int status;
    ConfigFile config = ConfigFile::getInstance();

    sa.sin_family = AF_INET; // IPv4

    // Resolve the host name to an IP address using getaddrinfo
    struct addrinfo hints, *res;

    hints.ai_flags = 0;
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_protocol = 0;
    hints.ai_addrlen = 0;
    hints.ai_addr = NULL;
    hints.ai_canonname = NULL;
    hints.ai_next = NULL;

    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP

    status = getaddrinfo(server.getHost().c_str(), NULL, &hints, &res);
    if (status != 0) {
        std::cerr << "[Server] getaddrinfo error: " << gai_strerror(status) << std::endl;
        return (-1);
    }

    // Copy the resolved IP address to sa.sin_addr
    sa.sin_addr = ((struct sockaddr_in *)res->ai_addr)->sin_addr;
    freeaddrinfo(res); // Free the address info structure

    sa.sin_port = htons(server.getPort());

    // Create listening socket
    socket_fd = socket(sa.sin_family, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        std::cerr << "[Server] Socket error" << std::endl;
        return (-1);
    }
	std::cout << "[Server] Created server socket fd: " << socket_fd << std::endl;


	//TO REMOVE LATER !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	int opt = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}



    // Bind socket to address and port
    status = bind(socket_fd, (struct sockaddr *)&sa, sizeof sa);
    if (status != 0) {
        std::cerr << "[Server] Bind error" << std::endl;
        return (-1);
    }
	//std::cout << "Bound socket to localhost port " << server.getPort() << std::endl;

    return (socket_fd);
}

void handle_client_request(Client & client, std::vector<pollfd> & poll_fds, std::vector<Client> & clients)
{
	int client_fd = client.getFd();
	Request * request = new Request();
	int bytes = read_data_from_socket(client);


	if (client.getRawRequest() == "413 Payload Too Large")
	{
		Response * responseerror = new Response(PAYLOAD_TOO_LARGE, UNDEFINED, request);
		client.setResponse(responseerror);
		client.setStatus(WRITE);
		poll_fds.back().events = POLLOUT;
		client.setRawRequest("");
		client.setTotalBytes(0);
		return;
	}

	ConfigFile config = ConfigFile::getInstance();

	if (client.getRawRequest().empty()) {
		close(client_fd);
		config.remove_from_poll_fds(poll_fds, clients, client_fd);
		//std::cout << "[Server] Closed connection on client socket" << client_fd << std::endl;
		return;
	}

	//std::cout << "Request receveid: " << client.getRawRequest() << std::endl;

	if ((bytes < 4096 && client.getContentLength() == 0 && !client.getError()) || (client.getContentLength() < client.getTotalBytes() && !client.getError()))
	{
		Response * response = new Response();
		try
		{
			//std::cout << "This is right after reading the entire request :";
			//std::cout << std::endl << std::endl << "Request Length : " << client.getRawRequest().length() << std::endl << "Total bytes read : " << client.getTotalBytes() << std::endl << std::endl;
			request->fillVariables(client.getRawRequest());
			//std::cout << "Is CGI: " << request->getIsCgi() << std::endl;
			if (request->getIsCgi())
			{
				CGI cgi(request, request->getPath());
				response = cgi.executeCGI();
				response->setRequest(request);
				// response.send_cgi_response(client_fd);
				client.setResponse(response);
			}
			else
			{
				response = request_dealer(request, client);
				// std::cout << "Path corrupted: " << response.getRoute()->getPath() << std::endl;
				// response.send_response(client_fd);
				client.setResponse(response);
				
			}
		}
		catch(const std::exception& e)
		{
			response = new Response(BAD_REQUEST, UNDEFINED, request);
			client.setResponse(response);
			// response.send_response(client_fd);
		}
		client.setStatus(WRITE);
		poll_fds.back().events = POLLOUT;
		client.setRawRequest("");
		client.setTotalBytes(0);
	}

}

void accept_new_connection(int server_socket, std::vector<pollfd> & poll_fds, std::vector<Client> & clients, ServerConfig & server)
{
    int client_fd;

    client_fd = accept(server_socket, NULL, NULL);
    if (client_fd == -1) {
		std::cerr << "[Server] Accept error: " << std::endl;
        return ;
    }
    // add_to_poll_fds(poll_fds, client_fd, poll_count, poll_size);
	// poll_fds.push_back({client_fd, POLLIN});

	ConfigFile config = ConfigFile::getInstance();

	Client & client = config.add_to_poll_fds(poll_fds, clients, client_fd, server);

	//std::cout << "[Server] Accepted new connection on client socket " << client_fd << std::endl;

	handle_client_request(client, poll_fds, clients);
    //if (client.getStatus() == WRITE)
    //	poll_fds.back().events = POLLOUT;
}

std::string get_directory_path(std::string path) {
    std::string directory = path;
    size_t found = directory.find_last_of("/\\");
    if (found == std::string::npos) {
        return "/";
    }
    directory = directory.substr(0, found);
    if (directory == "") {
        directory = "/";
    }
    return directory;
}

void setNonBlocking(int socket_fd)
{
	int flags = fcntl(socket_fd, F_GETFL, 0);
	if (flags == -1) {
		std::cerr << "Error getting socket flags" << std::endl;
		return;
	}

	if (fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
		std::cerr << "Error setting socket to non-blocking mode" << std::endl;
	}
}

int	read_data_from_socket(Client & client)
{
	char buffer[4096];
	int bytes_read;


	//setNonBlocking(client.getFd());
	bytes_read = recv(client.getFd(), buffer, 4096, 0);
	client.setTotalBytes(client.getTotalBytes() + bytes_read);
	client.setRawRequest(client.getRawRequest().append(buffer, bytes_read));
	if (client.getRawRequest().substr(0, 4) == "POST")
	{
		std::string content_length_key = "Content-Length: ";
		size_t pos = client.getRawRequest().find(content_length_key);
		if (pos != std::string::npos)
		{
			pos += content_length_key.length();
			size_t end_pos = client.getRawRequest().find("\r\n", pos);
        		std::string content_length_value = client.getRawRequest().substr(pos, end_pos - pos);
       			int content_length = std::atoi(content_length_value.c_str());
			client.setContentLength(content_length);
			if (client.getContentLength() > client.getServer().getBodySize() * 1000000)
			{
				client.setRawRequest("413 Payload Too Large");
				client.setError(true);
			}
		}
	}
	if (client.getError() && client.getTotalBytes() >= client.getContentLength())
		client.setError(false);
	//std::cout << std::endl << std::endl << "This is Right after reading :" << std::endl << std::endl;
	//std::cout << "Buffer :" << std::endl << buffer << std::endl << std::endl;
	//std::cout << "Request :" << std::endl << client.getRawRequest() << std::endl << std::endl;
	//if (bytes_read < 0)
	//{
	//	std::cerr << "[Server] Recv error." << std::endl;
	//	return "";
	//}
	//if (bytes_read == 0 && client.getTotalBytes())
	//	std::cout << "[Server] Client socket closed connection." << std::endl;
	return bytes_read;
}

Response * request_dealer(Request * request, Client & client) {
    ServerConfig config = client.getServer();

    if (config.isPathValid(request->getPath())) {
        if (!config.getRoute(request->getPath())->isMethodAllowed(request->getType())) {
            return new Response(FORBIDDEN, UNDEFINED, config.getRoute(request->getPath()), request->getPath(), request);
        }
    }

    Route * route;
    
    try
    {
        route = config.getRoute(request->getPath());
    }
    catch(const std::exception& e)
    {
        std::string dir = get_directory_path(request->getPath());
        if (config.isPathValid(dir)) {
            route = config.getRoute(dir);
            if (!config.getRoute(dir)->isMethodAllowed(request->getType())) {
                return new Response(FORBIDDEN, UNDEFINED, route, request->getPath(), request);
            }
            if (request->getPath().find(route->getPath()) == 0) { // Found the path of the route in the request path
                //std::cout << "Got here" << std::endl;
                std::string path;
                size_t pos = request->getPath().find('/');
                if (pos != std::string::npos) {
                    path = request->getPath().substr(pos + 1);
                }
                //std::cout << "Path: " << path << std::endl;
                //std::cout << "Previous path: " << request->getPath() << std::endl;
                request->setPath(path);
            }
            request->setPath(route->getRoot() + request->getPath());
            if (request->getPath()[0] == '/' && request->getPath().size() > 1) {
                request->setPath(request->getPath().substr(1));
            }
            struct stat info;
            if (stat(request->getPath().c_str(), &info) != 0 || !S_ISREG(info.st_mode)) {
                return new Response(NOT_FOUND, UNDEFINED, NULL, request->getPath(), request);
            }
        }
        else {
            struct stat info;
            if (stat(request->getPath().c_str(), &info) != 0) {
                return new Response(INTERNAL_SERVER_ERROR, UNDEFINED, request);
            }
            if ((request->getPath() == "/") || (!S_ISDIR(info.st_mode) && !S_ISREG(info.st_mode))) {
                return new Response(NOT_FOUND, UNDEFINED, NULL, request->getPath(), request);
            }
        }
    }

//    std::cout << "All good" << std::endl;

    switch (request->getType()) {
        case GET:
            return new Response(OK, request->getType(), route, request->getPath(), request);
        case POST:
            return new Response(CREATED, request->getType(), route, request->getPath(), request);
        case DELETE:
            return new Response(ACCEPTED, request->getType(), route, request->getPath(), request);
        default:
            return new Response(BAD_REQUEST, UNDEFINED, route, request->getPath(), request);
    }
}

// Add a new file descriptor to the pollfd array

// Remove an fd from the poll_fds array
// void del_from_poll_fds(struct pollfd **poll_fds, int i, int *poll_count) {
//     // Copy the fd from the end of the array to this index
//     (*poll_fds)[i] = (*poll_fds)[*poll_count - 1];
//     (*poll_count)--;
// }