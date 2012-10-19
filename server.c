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
#include<string.h>
#include "list.h"

#define PORTNO "3508"
#define MAXDATASIZE 1024

void sigchld_handler(int s){
	printf("Signal handler\n");
	while(waitpid(-1,NULL,WNOHANG) > 0);
}

void *get_in_addr(struct sockaddr *sa){
	if(sa->sa_family == AF_INET){
		return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int addClientToPeer(char *hostname){
    peernode *temp = (peernode *)malloc(sizeof(peernode));
	temp->hostname = hostname;
	insertFrontPeerList(temp);
}

int main(int argc,char *argv[]){

	int status,sockfd,newfd,numbytes;
	struct addrinfo hints,*res;
	struct sockaddr_in connector_addr;
	socklen_t addr_size;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	struct sigaction sa;
	
	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	getaddrinfo(NULL,PORTNO,&hints,&res);
	
	//Create a socket
	if((sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol)) == -1){
		perror("Server:socket creating error");
	}
	
	if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1){
		perror("setsockopt");
		exit(1);
	}
	
	if(bind(sockfd,res->ai_addr,res->ai_addrlen) == -1){
		close(sockfd);
		perror("Server : bind");
	}
	if(listen(sockfd,10) == -1){
		perror("Listen");
		exit(1);
	}
	
	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if(sigaction(SIGCHLD, &sa, NULL) == -1){
		perror("sigaction");
		exit(1);
	}
	
	printf("Server started\n");
	
	while(1){
		addr_size = sizeof(connector_addr);
		newfd = accept(sockfd,(struct sockaddr *)&connector_addr,&addr_size);
		
		if(newfd == -1){
			perror("Accept");
			continue;
		}
        
        struct hostent *client = gethostbyaddr((char *)&connector_addr.sin_addr,sizeof(connector_addr.sin_addr),connector_addr.sin_family);
		
		printf("Server : got connection from %s \n",client->h_name);
		
		if(!fork()){
			close(sockfd);
            
            int initFlag = 0;
            
            char buf[MAXDATASIZE]="";
            char command[256]="";
            
            while (1) {
                
                //Add RFC list
                numbytes = recv(newfd,buf,MAXDATASIZE-1,0);
	
                if(numbytes == -1){
                    perror("Receive");
                    exit(1);
                }else if(numbytes == 0){
                    printf("Connection closed: Exiting\n");
                    break;
                }
                
                buf[numbytes] = '\0';
                
                //If initial message add the client to the peer list
                if(initFlag == 0){
                    char *ch;
                    char field[256],field1[256];
                    ch = strstr(buf,"Host:");
                    sscanf(ch,"%[^' '] %[^\n]",field,field1);
                    //printf("Substring =>%s,%s\n",field,field1);
                    addClientToPeer(field1);
                    initFlag++;
                }
                
                //Check if method ADD
                strncpy(command,buf,3);
                printf("%s\n",command);
                if(strncmp(command,buf,3) == 0){
                    printf("ADD method call\n");
                }
                
                //printf("%s",buf);
                printAll();
            
            }
            close(newfd);
            exit(0);
		}
		close(newfd);	
	}
	
	return 0;
}