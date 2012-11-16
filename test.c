#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<sys/wait.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/utsname.h>
#define MAXDATASIZE 1025

int main(){
    
    char *buf;
    int status,sockfd,numbytes;
    struct addrinfo hints,getfilestruct,*res,*getfileserver,getstruct,*me,*p;
    socklen_t addr_size;
    char s[INET6_ADDRSTRLEN];
    struct sigaction sa;
    int getfilesocketfd;
    struct sockaddr_in *h;
	int rv;
    
    memset(&getfilestruct,0,sizeof(getfilestruct));
    getfilestruct.ai_family = AF_UNSPEC;
    getfilestruct.ai_socktype = SOCK_STREAM;
    
    getaddrinfo("nom0054215.nomadic.ncsu.edu","65432",&getfilestruct,&getfileserver);
    
    // loop through all the results and connect to the first we can
    
    if((getfilesocketfd = socket(p->ai_family,p->ai_socktype,p->ai_protocol)) == -1){
        perror("Server:upload socket creation error");
    }
    
    if(connect(getfilesocketfd,p->ai_addr,p->ai_addrlen) == -1){
        close(getfilesocketfd);
        perror("Client : Connect");
        exit(1);
    }
    
    char *str,*uploadbuf="P2P-CI/1.0 200 OK\n345 RFC345 nom0061096.nomadic.ncsu.edu";
    
    char field[256],field1[256],rfcid[256];
    
    str = strstr(uploadbuf,"\n");
    printf("%s\n",str);
    sscanf(str,"%s %s %s",field,field1,rfcid);
    
    printf("%s,%s,%s",field,field1,rfcid);
}