
// Standard Libraries
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <pthread.h>


#if defined WIN32

#include <windows.h>
#define SOKET SOCKET

#else

#include <netdb.h>
#include <unistd.h>
#include <termios.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

#define SOKET int

#endif

char ip[16];



void* ThreadFunc(void* port_t){

	  	int port = *(int *)port_t;
	  	SOKET sckt;
	  	struct sockaddr_in sockin;
	  	int err;

	  	sckt = socket(PF_INET , SOCK_STREAM , 0); //make net a valid socket

		if(sckt < 0)  //if not a socket
		{
		   perror("\nSocket creation failed");  // perror function prints an error message to stderr
		   closesocket(sckt);
		   exit(EXIT_FAILURE);
		}

		sockin.sin_family = AF_INET;
		sockin.sin_port = htons(port);

		#if defined(WIN32)
		sockin.sin_addr.S_un.S_addr = inet_addr(ip); // just for IPv4
		#else
		sockin.sin_addr.s_addr = inet_addr(ip); // just for IPv4
		#endif

		memset(&sockin.sin_zero, 0, sizeof sockin.sin_zero);


		err = connect(sckt , (struct sockaddr*)&sockin , sizeof(sockin));


		if(err == SOCKET_ERROR) //connection not accepted
		{
			printf("%s %5u Winsock Error Code : %d\n" , "NULL" , port , WSAGetLastError());
			fflush(stdout);
	    }
		else  //connection accepted
		{
			printf("%s %5u accepted\n" , "NULL" , port);

		}

		#ifdef _WIN32
		closesocket(sckt);
		#else
		close(sckt);
		#endif

		pthread_exit(NULL);

}


int main(){


	//General Purpose Variables
	int startport_i, endport_i;
	int threadcount;
	int status;
	pthread_t *tid;
	int *port;

#ifdef _WIN32
		WSADATA firstsock;
		//Initialise winsock
		if (WSAStartup(MAKEWORD(2,0),&firstsock) != 0)  //CHECKS FOR WINSOCK VERSION 2.0
		{
			fprintf(stderr,"WSAStartup() failed"); //print formatted data specify stream and options
			exit(EXIT_FAILURE);        //or exit(1);
		}
#endif


	puts("Enter the hostname or IP address : ");
	fgets(ip, sizeof(ip), stdin);

	puts("Enter the start port : ");
	scanf("%d", &startport_i);

	puts("Enter the start port : ");
	scanf("%d", &endport_i);


	threadcount = endport_i - startport_i + 1;

	port = (int*)malloc( threadcount * sizeof(int));
	for(int i = 0; i < threadcount; i++) port[i] = startport_i + i;


	tid = (pthread_t*)malloc( threadcount * sizeof(pthread_t));


	 //Start the portscan loop
	puts("Starting the scan loop...\n");

	for(int j = 0 ; j < threadcount ; j++){

		if ((status = pthread_create(&tid[j], NULL, ThreadFunc, &port[j]))) {
	      fprintf(stderr, "error: pthread_create, status: %d\n", status);
	      return EXIT_FAILURE;
		}

	 }

	for(int j = 0; j < threadcount; j++){

		  pthread_join(tid[j], NULL);

	  }

	#ifdef _WIN32
		WSACleanup();
	#endif

	 fflush(stdout);

	 return(EXIT_SUCCESS);
}
