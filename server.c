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

#define PORTNO "5530"
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

int addRFCDetail(int rfcid,char* rfctitle, char *hostname){
    
    rfcdetailnode *temp1 = (rfcdetailnode *)malloc(sizeof(rfcdetailnode));
	temp1->rfcno =rfcid;
	temp1->rfctitle = rfctitle;
	temp1->hostname = hostname;
	insertFrontrfcList(temp1);
}

int main(int argc,char *argv[]){

	int status,sockfd,newfd,numbytes;
	struct addrinfo hints,*res;
	struct sockaddr_in connector_addr;
	socklen_t addr_size;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	struct sigaction sa;
    int i;
    
    fd_set master;
    fd_set read_fds;
    int fdmax;
    
    FD_ZERO(&master);
    FD_ZERO(&read_fds);
	
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
	
	printf("Server started\n");
    
    FD_SET(sockfd,&master);
    fdmax = sockfd;
	
	while(1){
        
        read_fds = master;
        if(select(fdmax+1,&read_fds,NULL,NULL,NULL) == -1){
            perror("select");
            exit(4);
        }
        
        for(i=0;i<=fdmax;i++){
            if(FD_ISSET(i,&read_fds)){
                if(i == sockfd){
                    //New connection
                    printf("New connection\n");
                    addr_size = sizeof(connector_addr);
                    newfd = accept(sockfd,(struct sockaddr *)&connector_addr,&addr_size);
                    
                    if(newfd == -1){
                        perror("Accept");
                        continue;
                    }else{
                        FD_SET(newfd,&master);
                        if(newfd > fdmax){
                            fdmax = newfd;
                        }
                        
                        struct hostent *client = gethostbyaddr((char *)&connector_addr.sin_addr,sizeof(connector_addr.sin_addr),connector_addr.sin_family);
                        
                        printf("Server : got connection from %s \n",client->h_name);

                    }
                }else{
                    //Handle data from client
                    int initFlag = 0;
                    
                    char buf[MAXDATASIZE]="";
                    char command[256]="";
                    char hostname[256]="";
                    char *rfctitle = (char *)malloc(sizeof(256));
                    
                    //Add RFC list
                    numbytes = recv(i,buf,MAXDATASIZE-1,0);
                    
                    if(numbytes == -1){
                        perror("Receive");
                        close(i);
                        FD_CLR(i,&master);
                        continue;
                    }else if(numbytes == 0){
                        printf("Connection closed: Exiting\n");
                        close(i);
                        FD_CLR(i,&master);
                        continue;
                    }
                    
                    buf[numbytes] = '\0';
                    
                    //If initial message add the client to the peer list
                    if(initFlag == 0){
                        char *ch;
                        char field[256];
                        ch = strstr(buf,"Host:");
                        sscanf(ch,"%[^' '] %[^\n]",field,hostname);
                        //printf("Substring =>%s,%s\n",field,field1);
                        addClientToPeer(hostname);
                        initFlag++;
                    }
                    
                    //Check if method ADD
                    strncpy(command,buf,3);
                    printf("%s\n",command);
                    if(strncmp(command,buf,3) == 0){
                        printf("ADD method call\n");
                        //Add rfc detail to the rfc detail list
                        //RFCID
                        char *ch;
                        char field[256],field1[256];
                        int rfcid;
                        ch = strstr(buf,"ADD");
                        printf("%s\n",ch);
                        sscanf(ch,"%s %s %d",field,field1,&rfcid);
                        
                        //RFCTITLE
                        ch = strstr(buf,"Title:");
                        sscanf(ch,"%[^' '] %[^\n]",field,rfctitle);
                        
                        addRFCDetail(rfcid,rfctitle,hostname);
                    }
                    
                    //printf("%s",buf);
                    printAll();

                }
            }
        }
	}
	
	return 0;
}