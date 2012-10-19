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

#define MAXDATASIZE 100
#define GET 0
#define ADD 1
#define LOOKUP 2
#define LISTALL 3
#define UPLOADPORT 8000

struct utsname sysname;
char hostname[512];

char* reqHeader(int sockfd,int option,int rfcid,char* title){
	char* temp=NULL;
    char fulltext[2048]="";
    
	if(option == ADD){
		temp = "ADD RFC ";
        
        char rfcid_string[32];
        sprintf(rfcid_string,"%d",rfcid);
        
        char port_string[32];
        sprintf(port_string,"%d",UPLOADPORT);
        
        strcat(fulltext,temp);
        strcat(fulltext,rfcid_string);
        strcat(fulltext," P2P-CI/1.0\nHost: ");
        strcat(fulltext,hostname);
        strcat(fulltext,"\nPort: ");
        strcat(fulltext,port_string);
        strcat(fulltext,"\nTitle: ");
        strcat(fulltext,title);
        strcat(fulltext,"\0");
        
        printf("Sending %s\n",fulltext);
        
	}else if(option == LOOKUP){
        
		temp = "LOOKUP RFC 123 P2P-CI/1.0\nHost: thishost.csc.ncsu.edu\
		\nPort: 5678\nTitle: A Proferred Official ICP\0";
        
	}else if(option == LISTALL){
        
		temp = "LIST ALL P2P-CI/1.0\nHost: thishost.csc.ncsu.edu\
		\nPort: 5678\0";
        
	}else if(option == GET){
        
        temp = "GET RFC 1234 P2P-CI/1.0\
        \nHost: somehost.csc.ncsu.edu\
        \nOS: ";
        
        strcat(fulltext,temp);
        strcat(fulltext,sysname.sysname);
        strcat(fulltext," ");
        strcat(fulltext,sysname.release);
        
    }
	
    sleep(1);
    
    if(send(sockfd,fulltext,strlen(fulltext),0) == -1)
        perror("send");
    
	return fulltext;
}

//This method is used to initialize the client by sending rfc details to the server
int initClient(int sockfd){
    
    printf("Initializing client\n");
    
    FILE *fp;
    int id;
    char title[256];
    
    fp = fopen("index.txt","r");
    if(fp == NULL){
        perror("Rfc index not found");
        exit(-1);
    }
    printf("I have the below Rfc's\n");
    while(fscanf(fp,"%d %[^\t\n]",&id,title) == 2){
        //Send add requests to the server
        reqHeader(sockfd,ADD,id,title);
    }
    
    fclose(fp);
    return 0;
}

void *get_in_addr(struct sockaddr *sa){
	if(sa->sa_family == AF_INET){
		return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc,char *argv[]){

	char* PORTNO = "3508";
	char* SERVERIP = "127.0.0.1";

    //Populate the OS name
    if(uname(&sysname)){
        perror("Error obtaining os name");
        exit(-1);
    }
    
	hostname[511]='\0';
	gethostname(hostname,511);

	char *buf;
	int status,sockfd,numbytes;
	struct addrinfo hints,*res,*me;
	socklen_t addr_size;
	char s[INET6_ADDRSTRLEN];
	
	//Server details
	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	
	getaddrinfo(SERVERIP,PORTNO,&hints,&res);
	
	//Create a socket
	if((sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol)) == -1){
		perror("Server:socket creating error");
	}
	
	if(connect(sockfd,res->ai_addr,res->ai_addrlen) == -1){
		close(sockfd);
		perror("Client : Connect");
		exit(1);
	}
	
	inet_ntop(res->ai_family,get_in_addr((struct sockaddr *)res->ai_addr),s,sizeof(s));
	gethostname(hostname,1023);
	printf("Hostname : %s\n",hostname);
	printf("Client connecting to : %s\n",s);

    initClient(sockfd);
    
	/*buf=reqHeader(sockfd,ADD,NULL,NULL);
	
	printf("Client sent %s:%d\n",buf,strlen(buf));
	
	buf=reqHeader(sockfd,LOOKUP,NULL,NULL);
	
    printf("Client sent %s:%d\n",buf,strlen(buf));
    
	buf=reqHeader(sockfd,LISTALL,NULL,NULL);
	
	printf("Client sent %s:%d\n",buf,strlen(buf));
	
	buf = reqHeader(sockfd,GET,NULL,NULL);
    
    printf("Client sent %s:%d\n",buf,strlen(buf));*/
    
	//close(sockfd);

	return 0;
}