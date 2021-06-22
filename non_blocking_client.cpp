#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int guard(int n, char * err) { if (n == -1) { perror(err); exit(1); } return n; }

int main() {
	int tcp_socket_fd = guard(socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, 0), "could not create TCP listening socket (Non-blocking)");
	//int listen_socket_fd = guard(socket(AF_INET, SOCK_STREAM, 0), "could not create TCP listening socket (blocking)");
	//int flags = guard(fcntl(listen_socket_fd, F_GETFL), "could not get flags on TCP listening socket");
	//guard(fcntl(listen_socket_fd, F_SETFL, flags | O_NONBLOCK), "could not set TCP listening socket to be non-blocking");
	struct sockaddr_in local_addr_, server_addr_;
	local_addr_.sin_family = AF_INET;
	local_addr_.sin_port = htons(2020);
	local_addr_.sin_addr.s_addr = htonl(INADDR_ANY);
	guard(bind(tcp_socket_fd, (struct sockaddr *) &local_addr_, sizeof(local_addr_)), "could not bind");

	server_addr_.sin_family = AF_INET;
	server_addr_.sin_port = htons(8080);
	server_addr_.sin_addr.s_addr = INADDR_ANY;

	int server_fd_ = connect(tcp_socket_fd, reinterpret_cast<struct sockaddr *>(&server_addr_), sizeof(server_addr_));
	while(server_fd_ < 0){
		printf("Unable to connect; sleeping for 1 second.\n");
		sleep(1);
		server_fd_ = connect(tcp_socket_fd, reinterpret_cast<struct sockaddr *>(&server_addr_), sizeof(server_addr_));
	}
	printf("Connection made with %s : %u. \n", inet_ntoa(server_addr_.sin_addr), ntohs(server_addr_.sin_port));
	// Lets send some data on a loop
	
	while(1){
		printf("Writing 'hello' then sleeping for 10 seconds.\n");
		char msg[] = "hello\n";
		if(send(server_fd_, msg, sizeof(msg), MSG_NOSIGNAL) < 0){
			printf("Destination was closed, exiting loop. \n");
			break;
		}
		sleep(10);
	}

	return EXIT_SUCCESS;
}
