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

#define PORT 4142  // our server's port


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
	INTERNAL_SERVER_ERROR = 500,
	NOT_IMPLEMENTED = 501,
	SERVICE_UNAVAILABLE = 503
};

int create_server_socket(void);
void accept_new_connection(int server_socket, std::vector<pollfd> & poll_fds);
RequestType read_data_from_socket(int client_fd);
void add_to_poll_fds(std::vector<pollfd> & poll_fds, int fd);
// void del_from_poll_fds(struct pollfd **poll_fds, int i, int *poll_count);

int main(void)
{
    int server_socket;
    int status;

    // To monitor client sockets:
    // struct pollfd *poll_fds; // Array of socket file descriptors
	std::vector<pollfd> poll_fds;

    // Create server listening socket
    server_socket = create_server_socket();
    if (server_socket == -1) {
        return (1);
    }

    // Listen to port via socket
	std::cout << "[Server] Listening on port " << PORT << std::endl;
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

std::string getMessage(HTTPStatus status) {
	switch (status) {
		case OK:
			return "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";
		case CREATED:
			return "HTTP/1.1 201 Created\r\nContent-Type: text/html\r\nContent-Length: ";
		case ACCEPTED:
			return "HTTP/1.1 202 Accepted\r\nContent-Type: text/html\r\nContent-Length: ";
		case NO_CONTENT:
			return "HTTP/1.1 204 No Content\r\nContent-Type: text/html\r\nContent-Length: ";
		case BAD_REQUEST:
			return "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: ";
		case UNAUTHORIZED:
			return "HTTP/1.1 401 Unauthorized\r\nContent-Type: text/html\r\nContent-Length: ";
		case FORBIDDEN:
			return "HTTP/1.1 403 Forbidden\r\nContent-Type: text/html\r\nContent-Length: ";
		case NOT_FOUND:
			return "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: ";
		case INTERNAL_SERVER_ERROR:
			return "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\nContent-Length: ";
		case NOT_IMPLEMENTED:
			return "HTTP/1.1 501 Not Implemented\r\nContent-Type: text/html\r\nContent-Length: ";
		case SERVICE_UNAVAILABLE:
			return "HTTP/1.1 503 Service Unavailable\r\nContent-Type: text/html\r\nContent-Length: ";
		default:
			return "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\nContent-Length: ";
	}
}

std::string getHtml(HTTPStatus status, RequestType requestType) {
    (void)status;
    std::string response;
    if (requestType == GET)
	    response = "<html><h1>Hello, I got a GET Request!</h1></html>";
    else if (requestType == POST)
        response = "<html><h1>Hello, I got a POST Request!</h1></html>"; 
    else if (requestType == DELETE)
        response = "<html><h1>Hello, I got a DELETE Request!</h1></html>";
    else {
        response = "<html><h1>400 BAD REQUEST</h1></html>";
    }
    return response;
}

void send_response(int client_fd, HTTPStatus status, RequestType requestType) {

	std::string serverMessage = getMessage(status);
    std::string response = getHtml(status, requestType);

	std::stringstream ss;
	ss << response.size();
	serverMessage.append(ss.str());
	serverMessage.append("\r\n\r\n");
	serverMessage.append(response);

	size_t messageSize = serverMessage.size();
	const char* message = serverMessage.c_str();

	(void)requestType;

    int send_status = send(client_fd, message, messageSize, 0);
    if (send_status == -1) {
		std::cerr << "[Server] Send error to client " << client_fd << std::endl;
    }
}

int create_server_socket(void) {
    // Prepare the address and port for the server socket
    struct sockaddr_in sa;
    int socket_fd;
    int status;

    sa.sin_family = AF_INET; // IPv4
    sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // 127.0.0.1, localhost
    sa.sin_port = htons(PORT);

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
	std::cout << "Bound socket to localhost port " << PORT << std::endl;

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
	add_to_poll_fds(poll_fds, client_fd);

	std::cout << "[Server] Accepted new connection on client socket " << client_fd << std::endl;

	RequestType requestType = read_data_from_socket(client_fd);

    if (requestType == UNDEFINED) {
        send_response(client_fd, BAD_REQUEST, requestType);
        close(client_fd);
        return;
    }

	send_response(client_fd, OK, requestType);

	close(client_fd);
	// poll_fds.erase(std::remove_if(poll_fds.begin(), poll_fds.end(), [client_fd](const pollfd & pfd) {
	// 	return pfd.fd == client_fd;
	// }), poll_fds.end());

    std::cout << "[Server] Closed connection on client socket " << client_fd << std::endl;
}

RequestType getType(std::string request) {
    if (request.find("GET") != std::string::npos) {
        return GET;
    }
    else if (request.find("POST") != std::string::npos) {
        return POST;
    }
    else if (request.find("DELETE") != std::string::npos) {
        return DELETE;
    }
    return UNDEFINED;
}

RequestType read_data_from_socket(int client_fd)
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

	return getType(buffer);
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