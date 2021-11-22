#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#define STAD struct sockaddr

//This functoin raad and display to and pass wo server
void funcRead(int sock_desc){	

	int MAX = 1000;
	char buff[MAX];
	int n;
	bzero(buff, sizeof(buff));
	
	printf("Pass any text to server : ");
	n = 0;
	
	time_t now; // `time_t` is an arithmetic time type
    time(&now); // Obtain current time
	
	if((buff[n++] = getchar()) != '\n'){
		
		write(sock_desc, buff, sizeof(buff));
		bzero(buff, sizeof(buff));
		read(sock_desc, buff, sizeof(buff));
		printf("\nFrom server : %s\n", buff);
		printf("\n");
		printf("Today is %s", ctime(&now)); // Convert to local time format and print to stdout
		printf("\n");
	}
}

//main code for connected between server and client code
int main(){
	
	int sock_desc;
	struct sockaddr_in servaddr, client;

	// socket create and verification
	sock_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_desc == -1) {
		printf("Failed to create socket...\n");
		exit(0);
	}
	else{
		printf("Socket successfully created…\n");
		bzero(&servaddr, sizeof(servaddr));
	}
	
	// assign IP Adress for server and PORT that using in it
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("192.168.56.107");
	servaddr.sin_port = htons(1717);

	// connect the client socket to server socket
	if (connect(sock_desc, (STAD*)&servaddr, sizeof(servaddr)) != 0){
		printf("Failed to connect with server...\n");
		exit(0);
	}
	else{
		printf("Successfully connected with server...\n");
	}
	
	// This funvtion to read
	funcRead(sock_desc);

	// close the socket
	close(sock_desc);
}
