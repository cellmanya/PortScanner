// To include getaddrinfo, freeaddrinfo and getnameinfo functions _WIN32_WINNT was defined as 0x0501
//#define _WIN32_WINNT	0x0501 // default is 0x500

//#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <windows.h>


typedef struct{

	SOCKET sckt;
	struct sockaddr_in sockin;
	int i;
	int err;

}thread_s;


DWORD WINAPI ThreadFunc(void *connect_t){

	  thread_s * sa = (thread_s*)connect_t;
	  sa->err = connect(sa->sckt , (struct sockaddr*)&sa->sockin , sizeof(sa->sockin));


	  if(sa->err == SOCKET_ERROR) //connection not accepted
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


int main(){


//	int status;
	int startport_i, endport_i;
	int threadcount;
//	int err;
	WSADATA firstsock;
	HANDLE *arrThread;
	DWORD ThreadID;
//	SOCKET sckt;
//	struct addrinfo ai;
//	struct addrinfo *result;
//	struct sockaddr_in sa;

	thread_s connect_t;

	 //Initialise winsock
	 if (WSAStartup(MAKEWORD(2,0),&firstsock) != 0)  //CHECKS FOR WINSOCK VERSION 2.0
	 {
	  fprintf(stderr,"WSAStartup() failed"); //print formatted data specify stream and options
	  exit(EXIT_FAILURE);        //or exit(1);
	 }


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

	arrThread = (void**)malloc( threadcount * sizeof(void*));

	connect_t.sockin.sin_addr.S_un.S_addr = inet_addr(ip); // just for IPv4

	  connect_t.sckt = socket(PF_INET , SOCK_STREAM , 0); //make net a valid socket
	  if(connect_t.sckt < 0)  //if not a socket
	  {
	   perror("\nSocket creation failed");  // perror function prints an error message to stderr
	   exit(EXIT_FAILURE);
	  }

//	  startport_i = atoi(startport);
//	  endport_i = atoi(endport);

	 //Start the portscan loop
	 puts("Starting the scan loop...\n");

	 for(int i = startport_i ; i<= endport_i ; i++){


		/*	if ((status = getaddrinfo(ip, , &ai, &result)) != 0) {

				fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));

			 exit(EXIT_FAILURE);

			} */

	  connect_t.sockin.sin_port = htons(i);
	  connect_t.i = i;

	  arrThread[i - startport_i] = CreateThread(NULL, 0, ThreadFunc, (void*)&connect_t, 0, &ThreadID);
      if (arrThread[i - startport_i] == NULL)
      {
          printf("Create Thread %d get failed. Error no: %d", (i - startport_i), GetLastError);
      }
      else
    	  printf("CreateThread() is OK, thread ID %u\n", ThreadID);
	  //connect to the server with that socket
//	  err = connect(sckt , result->ai_addr , result->ai_addrlen);


	 }

	 WaitForMultipleObjects( threadcount, arrThread, FALSE, 3000 );

	 closesocket(connect_t.sckt);   //closes the net socket

	 //freeaddrinfo();
	 for(int j = 0; j < threadcount; j++){

		 if(CloseHandle(arrThread[j]) != 0){

			 puts("Handle was closed successfully!");
		 }
		 else{
			 printf("Failed to close handle, error %u\n", GetLastError());
		 }
	 }

	 fflush(stdout);

	 return(0);
}
