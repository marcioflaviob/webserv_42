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
#include <iterator>
#include <iostream>
#include <sys/stat.h>

#include "Route.hpp"
#include "Response.hpp"
#include "Enums.hpp"
#include "Request.hpp"
#include "ConfigFile.hpp"
#include "CGI.hpp"

int create_server_socket(void);
void accept_new_connection(int server_socket, std::vector<pollfd> & poll_fds);
std::string read_data_from_socket(int client_fd);
Response request_dealer(Request & request);
void add_to_poll_fds(std::vector<pollfd> & poll_fds, int fd);
void remove_from_poll_fds(std::vector<pollfd> & poll_fds, int fd);
void handle_client_request(int client_fd, std::vector<pollfd> & poll_fds);
// void del_from_poll_fds(struct pollfd **poll_fds, int i, int *poll_count);

int main(void)
{
    int server_socket;
    int status;
    ConfigFile::initialize("./config.conf");


    // To monitor client sockets:
    // struct pollfd *poll_fds; // Array of socket file descriptors
	std::vector<pollfd> poll_fds;

    // Create server listening socket
    server_socket = create_server_socket();
    if (server_socket == -1) {
        return (1);
    }

    // Listen to port via socket
    status = listen(server_socket, 3000);
    if (status != 0) {
		std::cerr << "[Server] Listen error: " << std::endl;
        return (3);
    }

    // Add the listening server socket to array
	add_to_poll_fds(poll_fds, server_socket);

	std::cout << "[Server] Set up poll fd array" << std::endl;

    while (1) { // Main loop
        // Poll sockets to see if they are ready (2 second timeout)
        status = poll(poll_fds.data(), poll_fds.size(), 2000);
        if (status == -1) {
			std::cerr << "[Server] Poll error: " << std::endl;
            exit(1);
        }
        else if (status == 0) {
            // None of the sockets are ready
			std::cout << "[Server] Waiting..." << std::endl;
            continue;
        }

        // Loop on our array of sockets
        for (size_t i = 0; i < poll_fds.size(); i++) {
            if ((poll_fds[i].revents & POLLIN) != 0) {
                if (poll_fds[i].fd == server_socket) {
                    accept_new_connection(server_socket, poll_fds);
                } else {
                    handle_client_request(poll_fds[i].fd, poll_fds);
                }
            }

			// std::cout << "Socket " << poll_fds[i].fd << " is ready for I/O operation" << std::endl;
            // accept_new_connection(server_socket, poll_fds);
            
        }
    }
    return (0);
}

int create_server_socket(void) {
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

    status = getaddrinfo(config.getHost().c_str(), NULL, &hints, &res);
    if (status != 0) {
        std::cerr << "[Server] getaddrinfo error: " << gai_strerror(status) << std::endl;
        return (-1);
    }

    // Copy the resolved IP address to sa.sin_addr
    sa.sin_addr = ((struct sockaddr_in *)res->ai_addr)->sin_addr;
    freeaddrinfo(res); // Free the address info structure

    sa.sin_port = htons(config.getPort());

    // Create listening socket
    socket_fd = socket(sa.sin_family, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        std::cerr << "[Server] Socket error" << std::endl;
        return (-1);
    }
	std::cout << "[Server] Created server socket fd: " << socket_fd << std::endl;

    // Bind socket to address and port
    status = bind(socket_fd, (struct sockaddr *)&sa, sizeof sa);
    if (status != 0) {
        std::cerr << "[Server] Bind error" << std::endl;
        return (-1);
    }
	std::cout << "Bound socket to localhost port " << config.getPort() << std::endl;

    return (socket_fd);
}

void handle_client_request(int client_fd, std::vector<pollfd> & poll_fds) {
    std::string buffer = read_data_from_socket(client_fd);

    if (buffer.empty()) {
        close(client_fd);
        remove_from_poll_fds(poll_fds, client_fd);
        std::cout << "[Server] Closed connection on client socket " << client_fd << std::endl;
        return;
    }

    std::cout << "Request receveid: " << buffer << std::endl;

    ConfigFile config = ConfigFile::getInstance();
    Request request;
    Response response;

    try
    {
        request.fillVariables(buffer);
        std::cout << "Is CGI: " << request.getIsCgi() << std::endl;
        if (request.getIsCgi()) {
            CGI cgi(request, request.getPath());
            response = cgi.executeCGI();
            response.send_cgi_response(client_fd);
        } else {
            response = request_dealer(request);
            // std::cout << "Path corrupted: " << response.getRoute()->getPath() << std::endl;
            response.send_response(client_fd);
        }
    }
    catch(const std::exception& e)
    {
        response = Response(BAD_REQUEST, UNDEFINED);
        response.send_response(client_fd);
    }

    if (request.getHeader("Connection").find("keep-alive") == std::string::npos) {
        close(client_fd);
        remove_from_poll_fds(poll_fds, client_fd);
        std::cout << "[Server] Closed connection on client socket " << client_fd << std::endl;
    }
}

void accept_new_connection(int server_socket, std::vector<pollfd> & poll_fds)
{
    int client_fd;

    client_fd = accept(server_socket, NULL, NULL);
    if (client_fd == -1) {
		std::cerr << "[Server] Accept error: " << std::endl;
        return ;
    }
    // add_to_poll_fds(poll_fds, client_fd, poll_count, poll_size);
	// poll_fds.push_back({client_fd, POLLIN});
	add_to_poll_fds(poll_fds, client_fd);

	std::cout << "[Server] Accepted new connection on client socket " << client_fd << std::endl;

	handle_client_request(client_fd, poll_fds);
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

std::string read_data_from_socket(int client_fd)
{
    char buffer[BUFSIZ];
    int bytes_read;

    bytes_read = recv(client_fd, buffer, BUFSIZ, 0);
    if (bytes_read <= 0) {
        if (bytes_read == 0) {
            std::cout << "[Server] Client socket closed connection." << std::endl;
            return "";
        }
        else {
            std::cerr << "[Server] Recv error: " << std::endl;
            return "";
        }
    }
    return std::string(buffer);
}

Response request_dealer(Request & request) {
    ConfigFile config = ConfigFile::getInstance();

    if (config.isPathValid(request.getPath())) {
        if (!config.getRoute(request.getPath())->isMethodAllowed(request.getType())) {
            return Response(FORBIDDEN, UNDEFINED, config.getRoute(request.getPath()), request.getPath());
        }
    }

    Route * route;
    
    try
    {
        route = config.getRoute(request.getPath());
    }
    catch(const std::exception& e)
    {
        std::string dir = get_directory_path(request.getPath());
        if (config.isPathValid(dir)) {
            route = config.getRoute(dir);
            if (!config.getRoute(dir)->isMethodAllowed(request.getType())) {
                return Response(FORBIDDEN, UNDEFINED, route, request.getPath());
            }
            if (request.getPath().find(route->getPath()) == 0) { // Found the path of the route in the request path
                std::cout << "Got here" << std::endl;
                std::string path;
                size_t pos = request.getPath().find('/');
                if (pos != std::string::npos) {
                    path = request.getPath().substr(pos + 1);
                }
                std::cout << "Path: " << path << std::endl;
                std::cout << "Previous path: " << request.getPath() << std::endl;
                request.setPath(path);
            }
            request.setPath(route->getRoot() + request.getPath());
            if (request.getPath()[0] == '/' && request.getPath().size() > 1) {
                request.setPath(request.getPath().substr(1));
            }
            struct stat info;
            if (stat(request.getPath().c_str(), &info) != 0 || !S_ISREG(info.st_mode)) {
                return Response(NOT_FOUND, UNDEFINED, NULL, request.getPath());
            }
        }
        else {
            struct stat info;
            if (stat(request.getPath().c_str(), &info) != 0) {
                return Response(INTERNAL_SERVER_ERROR, UNDEFINED);
            }
            if (!S_ISDIR(info.st_mode) && !S_ISREG(info.st_mode)) {
                return Response(NOT_FOUND, UNDEFINED, NULL, request.getPath());
            }
        }
    }

    std::cout << "All good" << std::endl;

    switch (request.getType()) {
        case GET:
            return Response(OK, request.getType(), route, request.getPath());
        case POST:
            return Response(CREATED, request.getType(), route, request.getPath());
        case DELETE:
            return Response(ACCEPTED, request.getType(), route, request.getPath());
        default:
            return Response(BAD_REQUEST, UNDEFINED, route, request.getPath());
    }
}

// Add a new file descriptor to the pollfd array
void add_to_poll_fds(std::vector<pollfd> & poll_fds, int fd) {
	struct pollfd new_element;
	new_element.fd = fd;
	new_element.events = POLLIN;
	poll_fds.push_back(new_element);
}

void remove_from_poll_fds(std::vector<pollfd> & poll_fds, int fd) {
    for (std::vector<pollfd>::iterator it = poll_fds.begin(); it != poll_fds.end(); ++it) {
        if (it->fd == fd) {
            poll_fds.erase(it);
            break;
        }
    }
}

// Remove an fd from the poll_fds array
// void del_from_poll_fds(struct pollfd **poll_fds, int i, int *poll_count) {
//     // Copy the fd from the end of the array to this index
//     (*poll_fds)[i] = (*poll_fds)[*poll_count - 1];
//     (*poll_count)--;
// }