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

int guard(int n, char * err) { if (n == -1) { perror(err); exit(1); } return n; }

int main() {
	std::future<std::string> data_ready_;
	bool trig_started = false;
	std::string received_data_;
	
	int tcp_socket = guard(socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, 0), "could not create TCP listening socket (Non-blocking)");
	int yes = 1;
	if (setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
		printf("Unable to set socket to reusable. \n");
	}
	struct sockaddr_in tcp_addr_, cli_addr_;
	tcp_addr_.sin_family = AF_INET;
	tcp_addr_.sin_port = htons(8080); // this should be local port
	tcp_addr_.sin_addr.s_addr = INADDR_ANY;
	guard(bind(tcp_socket, (struct sockaddr *) &tcp_addr_, sizeof(tcp_addr_)), "could not bind");
	guard(listen(tcp_socket, 100), "could not listen");

	socklen_t clilen = sizeof(cli_addr_);
	// Wait until at least one connection is made.
  
	int client_fd_ = accept(tcp_socket, reinterpret_cast<struct sockaddr *>(&cli_addr_), &clilen);
	while(client_fd_ < 0){
		printf("No pending connections; sleeping for 5 seconds.\n");
		sleep(5);
		client_fd_ = accept(tcp_socket, reinterpret_cast<struct sockaddr *>(&cli_addr_), &clilen);
	}
	printf("Connection made with %s : %u. \n", inet_ntoa(cli_addr_.sin_addr), ntohs(cli_addr_.sin_port));
	// start the loop to receive data, asynchronously

	while(1){ 
		// think bascally what is happening on the retreive data/ spin cycle
		printf("Beginning of main thread. \n");
		if(data_ready_.valid() && data_ready_.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
		{
			printf("Threaded success. \n");
			std::string result = data_ready_.get();
			printf("\033[32;1m %s \033[0m\n", result.c_str());
			if(send(client_fd_, (">"+result).c_str(), result.size()+1, MSG_NOSIGNAL) == -1)
				printf("Unable to send data. \n");
			trig_started = false;
		}else if (!trig_started){
			printf("Triggering a check for data. \n");
			//Trigger a check for data received
			data_ready_ = std::async(std::launch::async, [&]()->std::string{
			//check and do time out stuff here etc
			trig_started = true;
			std::string res;
			char buffer[2048];
			//sleep(10);
			uint16_t rec_count = 0;
			while (1){
				printf("Before recv \n");
		 		rec_count = recv(client_fd_, buffer, sizeof(buffer), 0);
				printf("After recv \n");
		 		if(rec_count > 1){
	  				res=std::string(buffer, buffer+rec_count);
		 			break;
		 		} else if(rec_count == 0){
	  				printf("Connection closed: returning nothing\n");
	  				break;
				}
		 	}
			return res;
			});
		}
		sleep(1);
	}
  return EXIT_SUCCESS;
}
