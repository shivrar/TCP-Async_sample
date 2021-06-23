#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <future>
#include <chrono>
#include <vector>

int guard(int n, char * err) { if (n == -1) { perror(err); exit(1); } return n; }


struct ClientInfo{
sockaddr_in cli_addr;
int cli_fd;
};


int main() {

	int tcp_socket = guard(socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, 0), "could not create TCP listening socket (Non-blocking)");
	int yes = 1;
	if (setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
		printf("Unable to set socket to reusable. \n");
	}

	std::vector<ClientInfo> clients;

	struct sockaddr_in tcp_addr_;
	tcp_addr_.sin_family = AF_INET;
	tcp_addr_.sin_port = htons(8081); // this should be local port
	tcp_addr_.sin_addr.s_addr = INADDR_ANY;
	guard(bind(tcp_socket, (struct sockaddr *) &tcp_addr_, sizeof(tcp_addr_)), "could not bind");
	guard(listen(tcp_socket, 100), "could not listen");
  
	while(1){ 
		//We should scan for new connections, monitor said connections and close them if they are closed by the client
		//Test if we can send data as well.
		ClientInfo cli;
		socklen_t clilen = sizeof(cli.cli_addr);
		
		cli.cli_fd = accept(tcp_socket, reinterpret_cast<struct sockaddr *>(&cli.cli_addr), &clilen);
		if(cli.cli_fd >= 0){
			printf("Connection made with %s : %u. \n", inet_ntoa(cli.cli_addr.sin_addr), ntohs(cli.cli_addr.sin_port));
			clients.emplace_back(cli);
		}
			
		auto cli_it = clients.begin();	
		/*for(auto client: clients){
			if(send(client.cli_fd, "test\n", sizeof("test\n"), MSG_NOSIGNAL) == -1){
				//We can use this as an indication that the connection was closed
				close(client.cli_fd); // close connection
				clients.erase(client);
				printf("\033[10;1m Closing connection with %s : %u \033[0m\n", inet_ntoa(client.cli_addr.sin_addr), ntohs(client.cli_addr.sin_port));
			}
		}*/
		
		while(cli_it!=clients.end()){
			if(send(cli_it->cli_fd, "test\n", sizeof("test\n"), MSG_NOSIGNAL) == -1){
				close(cli_it->cli_fd);
				printf("\033[10;1m Closing connection with %s : %u \033[0m\n", inet_ntoa(cli_it->cli_addr.sin_addr), ntohs(cli_it->cli_addr.sin_port));
				clients.erase(cli_it);
			} else 
				cli_it++;
		}
		sleep(1);
	}
  return EXIT_SUCCESS;
}
