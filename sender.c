#include <sys/socket.h> 
#include <netinet/in.h>
#include <netinet/tcp.h> 
#include <stdio.h> 
#include <string.h> 
#include <unistd.h>
#include <errno.h> 
#include <stdlib.h>
#include <sys/types.h>
#include <arpa/inet.h>
#define SERVER_PORT 5060
#define SERVER_IP_ADDRESS "127.0.0.1"

int main(int argc, char **argv) {
    char buf[256];
    socklen_t len;
    int i=0;
    int sock, rval;
    struct sockaddr_in serverAddress;
    len = sizeof(buf);  

    printf("Current: %s\n", buf); 
    
    FILE *large_file = fopen("1mb.txt","r");//open file
    if(large_file ==NULL){
        printf("Unable to open file\n");
        return 1;
    }
    int messageLen =strlen((char*)large_file);
    for(i=0;i<5;i++){//first 5 files
        sock = socket(AF_INET, SOCK_STREAM, 0);//create socket
        if (sock == -1) {
 	        perror("socket");
            return -1;
        } 
        if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0) { 
        perror("getsockopt");
        return -1;
        } 
        memset(&serverAddress, 0, sizeof(serverAddress));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(SERVER_PORT);
        rval = inet_pton(AF_INET, (const char*)SERVER_IP_ADDRESS, &serverAddress.sin_addr);//convert to binary 
        if (rval <= 0)
        {
	        printf("inet_pton() failed");
	        return -1;
        }
        if (connect(sock, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1)//connect to measure
        {
	        printf("connect() failed with error code : %d",errno);
        }

        printf("connected to server\n");
        int bytesSent = send(sock, large_file, messageLen, 0);//sent file
        if (-1 == bytesSent)
        {
	        printf("send() failed with error code : %d" ,errno);
        }
        else if (0 == bytesSent)
        {
	        printf("peer has closed the TCP connection prior to send().\n");
        }
        else if (messageLen > bytesSent)
        {
	        printf("sent only %d bytes from the required %d.\n", messageLen, bytesSent);
        }
        else 
        {
	        printf("message was successfully sent .\n");
        }
        close(sock);
    }
    
    
    for(i=0;i<5;i++){//last 5 files
        sock = socket(AF_INET, SOCK_STREAM, 0);//create socket
        if (sock == -1) {
 	        perror("socket");
            return -1;
        } 
        strcpy(buf, "reno"); //change to reno algorithm
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
        memset(&serverAddress, 0, sizeof(serverAddress));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(SERVER_PORT);
        rval = inet_pton(AF_INET, (const char*)SERVER_IP_ADDRESS, &serverAddress.sin_addr);//convert to binary
        if (rval <= 0)
        {
	        printf("inet_pton() failed");
	        return -1;
        }
        if (connect(sock, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1)
        {
	        printf("connect() failed with error code : %d",errno);
        }

        printf("connected to server\n");
        int bytesSent = send(sock, large_file, messageLen, 0);//send file
        if (-1 == bytesSent)
        {
	        printf("send() failed with error code : %d" ,errno);
        }
        else if (0 == bytesSent)
        {
	        printf("peer has closed the TCP connection prior to send().\n");
        }
        else if (messageLen > bytesSent)
        {
	        printf("sent only %d bytes from the required %d.\n", messageLen, bytesSent);
        }
        else 
        {
	        printf("message was successfully sent .\n");
        }
        close(sock);
    }

    close(sock); 
    return 0; 
}
