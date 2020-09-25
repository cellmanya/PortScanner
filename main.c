// To include getaddrinfo, freeaddrinfo and getnameinfo functions _WIN32_WINNT was defined as 0x0501
#define _WIN32_WINNT	0x0501 // default is 0x500

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>


int main(){


	int status;
	int startport, endport;
	int err;
	WSADATA firstsock;
	SOCKET s;
	struct addrinfo hints;
	struct addrinfo *res;

	 //Initialise winsock
	 if (WSAStartup(MAKEWORD(2,0),&firstsock) != 0)  //CHECKS FOR WINSOCK VERSION 2.0
	 {
	  fprintf(stderr,"WSAStartup() failed"); //print formatted data specify stream and options
	  exit(EXIT_FAILURE);        //or exit(1);
	 }


	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_INET; // IPv4
	hints.ai_socktype = SOCK_STREAM; // TCP

	char buffer[2];
	char host[15];

	puts("Enter the hostname or IP address : ");
	gets(host);

	puts("Enter the start port : ");
	scanf("%d", &startport);

	puts("Enter the start port : ");
	scanf("%d", &endport);


	 //Start the portscan loop
	 puts("Starting the scan loop...\n");

	 for(int i = startport ; i<= endport ; i++){

		 	itoa(i, buffer, 10);

			if ((status = getaddrinfo(host, buffer, &hints, &res)) != 0) {

				fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));

			 exit(EXIT_FAILURE);

			}

	  s = socket(res->ai_family , res->ai_socktype , res->ai_protocol); //make net a valid socket
	  if(s < 0)  //if not a socket
	  {
	   perror("\nSocket creation failed");  // perror function prints an error message to stderr
	   exit(EXIT_FAILURE);
	  }


	  //connect to the server with that socket
	  err = connect(s , res->ai_addr , res->ai_addrlen);

	  if(err == SOCKET_ERROR) //connection not accepted
	  {
	   printf("%s %5d Winsock Error Code : %d\n" , res->ai_canonname , i , WSAGetLastError());
	   fflush(stdout);
	  }
	  else  //connection accepted
	  {
	   printf("%s %5d accepted\n" , res->ai_canonname , i);

	  }
	  closesocket(s);   //closes the net socket
	 }

	 freeaddrinfo(res);

	 fflush(stdout);

	 return(0);
}
