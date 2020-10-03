// To include getaddrinfo, freeaddrinfo and getnameinfo functions _WIN32_WINNT was defined as 0x0501
//#define _WIN32_WINNT	0x0501 // default is 0x500

#if defined WIN32
//#include <winsock2.h>
//#include <ws2tcpip.h>
#include <windows.h>

#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#endif

// Standard Libraries
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <pthread.h>

#if defined WIN32
typedef struct{

	SOCKET sckt;
	struct sockaddr_in sockin;
	int i;


}thread_s;

#ifndef _PTHREAD_H

DWORD WINAPI ThreadFunc(void *connect_t){

	  thread_s * sa = (thread_s*)connect_t;
	  int err = connect(sa->sckt , (struct sockaddr*)&sa->sockin , sizeof(sa->sockin));


	  if(err == SOCKET_ERROR) //connection not accepted
	  {
	   printf("%s %5u Winsock Error Code : %d\n" , "NULL" , sa->i , WSAGetLastError());
	   fflush(stdout);
	  }
	  else  //connection accepted
	  {
	   printf("%s %5u accepted\n" , "NULL" , sa->i);

	  }


	return 0;
}
#endif

#else
typedef struct{

	int sckt;
	struct sockaddr_in sockin;
	int i;


}thread_s;



#endif




void* ThreadFunc(void* connect_t){

	  thread_s * sa = (thread_s*)connect_t;
	  int err = connect(sa->sckt , (struct sockaddr*)&sa->sockin , sizeof(sa->sockin));


	  if(err == SOCKET_ERROR) //connection not accepted
	  {
	   printf("%s %5u Winsock Error Code : %d\n" , "NULL" , sa->i , WSAGetLastError());
	   fflush(stdout);
	  }
	  else  //connection accepted
	  {
	   printf("%s %5u accepted\n" , "NULL" , sa->i);

	  }

	  pthread_exit(NULL);

}


int main(){


	//General Purpose Variables
	int startport_i, endport_i;
	int threadcount;
	int status;
	thread_s connect_t;
	pthread_t *tid;

#ifndef _PTHREAD_H
	HANDLE *arrThread;
	DWORD ThreadID;
#endif

//	struct addrinfo ai;
//	struct addrinfo *result;
//	struct sockaddr_in sa;

#if 0
		WSADATA firstsock;
		//Initialise winsock
		if (WSAStartup(MAKEWORD(2,0),&firstsock) != 0)  //CHECKS FOR WINSOCK VERSION 2.0
		{
			fprintf(stderr,"WSAStartup() failed"); //print formatted data specify stream and options
			exit(EXIT_FAILURE);        //or exit(1);
		}
#endif

	memset(&connect_t.sockin, 0, sizeof connect_t.sockin); // make sure the struct is empty
//	ai.ai_family = AF_INET; // IPv4
//	ai.ai_socktype = SOCK_STREAM; // TCP
	connect_t.sockin.sin_family = AF_INET;

	char ip[16];

	puts("Enter the hostname or IP address : ");
	fgets(ip, sizeof(ip), stdin);

	puts("Enter the start port : ");
	scanf("%d", &startport_i);

	puts("Enter the start port : ");
	scanf("%d", &endport_i);


	threadcount = endport_i - startport_i + 1;


#ifndef _PTHREAD_H
	arrThread = (void**)malloc( threadcount * sizeof(void*));
#endif

	tid = (pthread_t*)malloc( threadcount * sizeof(pthread_t));

	connect_t.sockin.sin_addr.S_un.S_addr = inet_addr(ip); // just for IPv4

	connect_t.sckt = socket(PF_INET , SOCK_STREAM , 0); //make net a valid socket

	if(connect_t.sckt < 0)  //if not a socket
	{
	   perror("\nSocket creation failed");  // perror function prints an error message to stderr
	   closesocket(connect_t.sckt);
	   exit(EXIT_FAILURE);
	}

	 //Start the portscan loop
	puts("Starting the scan loop...\n");

	for(int i = startport_i ; i<= endport_i ; i++){


		/*	if ((status = getaddrinfo(ip, , &ai, &result)) != 0) {

				fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));

			 exit(EXIT_FAILURE);

			} */

	  connect_t.sockin.sin_port = htons(i);
	  connect_t.i = i;


	   if ((status = pthread_create(&tid[i - startport_i], NULL, ThreadFunc, &connect_t))) {
	      fprintf(stderr, "error: pthread_create, status: %d\n", status);
	      return EXIT_FAILURE;
	   }

#ifndef _PTHREAD_H

	   arrThread[i - startport_i] = CreateThread(NULL, 0, ThreadFunc, &connect_t, 0, &ThreadID);

      if (arrThread[i - startport_i] == NULL)
      {
          printf("Create Thread %d get failed. Error no: %d", (i - startport_i), GetLastError);
      }
      else
    	  printf("CreateThread() is OK, thread ID %u\n", ThreadID);

#endif

	 }

#ifndef _PTHREAD_H

	 	 WaitForMultipleObjects( threadcount, arrThread, FALSE, 3000 );


	 	 for(int j = 0; j < threadcount; j++){

	 		 if(CloseHandle(arrThread[j]) != 0){

	 			 puts("Handle was closed successfully!");
	 		 }
	 		 else{
	 			 printf("Failed to close handle, error %u\n", GetLastError());
	 		 }
	 	 }

#endif


	  for(int j = 0; j < threadcount; j++){

		  pthread_join(tid[j], NULL);

	  }

	 closesocket(connect_t.sckt);   //closes the net socket

	 fflush(stdout);

	 return(EXIT_SUCCESS);
}
