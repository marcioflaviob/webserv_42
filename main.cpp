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

int create_server_socket(void);
void accept_new_connection(int server_socket, std::vector<pollfd> & poll_fds);
Response read_data_from_socket(int client_fd);
void add_to_poll_fds(std::vector<pollfd> & poll_fds, int fd);
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
            if ((poll_fds[i].revents & POLLIN) != 1) {
                // The socket is not ready for reading
                // stop here and continue the loop
                continue ;
            }

			std::cout << "Socket " << poll_fds[i].fd << " is ready for I/O operation" << std::endl;
            accept_new_connection(server_socket, poll_fds);
            
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
	// add_to_poll_fds(poll_fds, client_fd);
    (void)poll_fds;

	std::cout << "[Server] Accepted new connection on client socket " << client_fd << std::endl;

	Response response = read_data_from_socket(client_fd);

    response.send_response(client_fd); // Send correct route

	close(client_fd);
	// poll_fds.erase(std::remove_if(poll_fds.begin(), poll_fds.end(), [client_fd](const pollfd & pfd) {
	// 	return pfd.fd == client_fd;
	// }), poll_fds.end());

    std::cout << "[Server] Closed connection on client socket " << client_fd << std::endl;
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

Response read_data_from_socket(int client_fd)
{
    char buffer[BUFSIZ];
    int bytes_read;

    bytes_read = recv(client_fd, buffer, BUFSIZ, 0);
    if (bytes_read <= 0) {
        if (bytes_read == 0) {
            std::cout << "[Server] Client socket closed connection." << std::endl;
        }
        else {
            std::cerr << "[Server] Recv error: " << std::endl;
        }
    }

    Request request;
    ConfigFile config = ConfigFile::getInstance();

    try {
        request.fillVariables(std::string(buffer));
    } catch(const std::exception& e) {
        return Response(BAD_REQUEST, UNDEFINED);
    }
    

    if (config.isPathValid(request.getPath())) {
        if (!config.getRoute(request.getPath()).isMethodAllowed(request.getType())) {
            return Response(FORBIDDEN, UNDEFINED, config.getRoute(request.getPath()));
        }
        // return Response(NOT_FOUND, UNDEFINED);
    }

    Route route;
    
    try
    {
        route = config.getRoute(request.getPath());
    }
    catch(const std::exception& e)
    {
        std::string dir = get_directory_path(request.getPath());
        if (config.isPathValid(dir)) {
            route = config.getRoute(dir);
            if (!config.getRoute(dir).isMethodAllowed(request.getType())) {
                return Response(FORBIDDEN, UNDEFINED, route);
            }
            if (request.getPath().find(route.getPath()) == 0) { // Found the path of the route in the request path
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
            route.setPath(route.getRoot() + request.getPath());
            std::cout << "Root: " << route.getRoot() << std::endl;
            if (route.getPath()[0] == '/' && route.getPath().size() > 1) {
                route.setPath(route.getPath().substr(1));
            }
            std::cout << "Route path: " << route.getPath() << std::endl;
            struct stat info;
            if (stat(route.getPath().c_str(), &info) != 0 || !S_ISREG(info.st_mode)) {
                return Response(NOT_FOUND, UNDEFINED);
            }
        }
        else {
            struct stat info;
            if (stat(request.getPath().c_str(), &info) != 0) {
                return Response(NOT_FOUND, UNDEFINED);
            }
            if (!S_ISDIR(info.st_mode) && !S_ISREG(info.st_mode)) {
                return Response(NOT_FOUND, UNDEFINED);
            }
            route.setPath(request.getPath());
        }
    }
    
    


    switch (request.getType()) {
        case GET:
            return Response(OK, request.getType(), route);
        case POST:
            return Response(CREATED, request.getType(), route);
        case DELETE:
            return Response(ACCEPTED, request.getType(), route);
        default:
            return Response(BAD_REQUEST, UNDEFINED, route);
    }
}

// Add a new file descriptor to the pollfd array
void add_to_poll_fds(std::vector<pollfd> & poll_fds, int fd) {
	struct pollfd new_element;
	new_element.fd = fd;
	new_element.events = POLLIN;
	poll_fds.push_back(new_element);
}

// Remove an fd from the poll_fds array
// void del_from_poll_fds(struct pollfd **poll_fds, int i, int *poll_count) {
//     // Copy the fd from the end of the array to this index
//     (*poll_fds)[i] = (*poll_fds)[*poll_count - 1];
//     (*poll_count)--;
// }