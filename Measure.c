#include <sys/socket.h> 
#include <netinet/in.h>
#include <netinet/tcp.h> 
#include <stdio.h> 
#include <string.h> 
#include <unistd.h> 
#include <signal.h>
#include <errno.h>
#include <time.h>
#define SERVER_PORT 5060

int main(int argc, char **argv) {
signal(SIGPIPE, SIG_IGN);
char buf[256];
socklen_t len;
int count = 0;
clock_t start_t, end_t, total_t;
int sock = socket(AF_INET, SOCK_STREAM, 0);//create socket
if (sock == -1) {
 	perror("socket");
    return -1;
} 
len = sizeof(buf); 
if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0) { //cubic algorithm
    perror("getsockopt");
    return -1;
} 

printf("Current: %s\n", buf);
int listeningSocket = -1;  
	 
    if((listeningSocket = socket(AF_INET , SOCK_STREAM , 0 )) == -1)
    {
        printf("Could not create listening socket : %d" ,errno);
    }
// Reuse the address if the server socket on was closed
	// and remains for 45 seconds in TIME-WAIT state till the final removal.
	//
    int enableReuse = 1;
    if (setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR, &enableReuse, sizeof(int)) < 0)
    {
         printf("setsockopt() failed with error code : %d" , errno);
    }

    // "sockaddr_in" is the "derived" from sockaddr structure
    // used for IPv4 communication. For IPv6, use sockaddr_in6
    //
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(SERVER_PORT);  //network order
      
    // Bind the socket to the port with any IP at this port
    if (bind(listeningSocket, (struct sockaddr *)&serverAddress , sizeof(serverAddress)) == -1)
    {
        printf("Bind failed with error code : %d" ,errno);
        return -1;
    }
      
    printf("Bind() success\n");
  
    // Make the socket listening; actually mother of all client sockets.
    if (listen(listeningSocket, 500) == -1) //500 is a Maximum size of queue connection requests
											//number of concurrent connections 
    {
	printf("listen() failed with error code : %d",errno);
        close(sock);
        return -1;
    }
      
    //Accept and incoming connection
    printf("Waiting for incoming TCP-connections...\n");
      
    struct sockaddr_in clientAddress;  
    socklen_t clientAddressLen = sizeof(clientAddress);

    while (1)//first 5 files
    {
    	memset(&clientAddress, 0, sizeof(clientAddress));
        clientAddressLen = sizeof(clientAddress);
        int clientSocket = accept(listeningSocket, (struct sockaddr *)&clientAddress, &clientAddressLen);//accept connection from sender
    	if (clientSocket == -1)
    	{
           printf("listen failed with error code : %d",errno);
           return -1;
    	}
      
    	printf("A new client connection accepted\n");
        start_t = clock();
        recv(sock, buf, len, 0);//recive file
        count++;
        end_t = clock();
        total_t+=(double)(end_t-start_t);//time to accept 5 files
        if(count == 5){
            break;
        }
    }
    double cubic_time = total_t/5;

    strcpy(buf, "reno"); //change cc algorithm to reno
    len = strlen(buf);
    if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, len) != 0) {
        perror("setsockopt"); 
        return -1;
    }
    len = sizeof(buf); 
    if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0) {
        perror("getsockopt"); 
        return -1; 
    } 
    printf("New: %s\n", buf); 
    count = 0;
    while (1)//last 5 files
    {
    	memset(&clientAddress, 0, sizeof(clientAddress));
        clientAddressLen = sizeof(clientAddress);
        int clientSocket = accept(listeningSocket, (struct sockaddr *)&clientAddress, &clientAddressLen);//accept connection from sender
    	if (clientSocket == -1)
    	{
            printf("listen failed with error code : %d",errno);
            close(sock);
            return -1;
    	}
      
    	printf("A new client connection accepted\n");
        start_t = clock();
        recv(sock, buf, len, 0);//recive file
        count++;
        end_t = clock();
        total_t+=(double)(end_t-start_t);
        if(count == 5){
            break;
        }
    }
    double reno_time = total_t/5;
    printf("cubic time: %f, reno time: %f\n", cubic_time, reno_time);//print average time for cubic and for reno
    close(sock); 
    return 0; 
}
