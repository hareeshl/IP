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

#define PORTNO "5540"
#define MAXDATASIZE 1025

char* command;
char* hostname;
char* rfctitle;
char* buf;

void *get_in_addr(struct sockaddr *sa){
	if(sa->sa_family == AF_INET){
		return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void addClientToPeer(char* hostname,int portno){
    peernode *temp = (struct peernode *)malloc(sizeof(peernode));
	temp->hostname=hostname;
    temp->uploadportno=portno;
	insertFrontPeerList(temp);
}

void addRFCDetail(int rfcid,char* rfctitle, char *hostname){
    
    rfcdetailnode *temp1 = (struct rfcdetailnode *)malloc(sizeof(rfcdetailnode));
	temp1->rfcno =rfcid;
	temp1->rfctitle=rfctitle;
	temp1->hostname=hostname;
	insertFrontrfcList(temp1);
}

char* generateResponseHeader(){
    char* temp;
    temp= "P2P-CI/1.0 200 OK\n";
    return temp;
}

int main(int argc,char *argv[]){
    
	int status,sockfd,newfd,numbytes;
	struct addrinfo hints,*res;
	struct sockaddr_in connector_addr;
	socklen_t addr_size;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
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
                        //printf("Server : got connection from %s \n",client->h_name);
                        
                    }
                }else{
                    //Handle data from client
                    int initFlag = 0;
                    
                    buf=(char *)malloc(MAXDATASIZE);
                    command=(char *)malloc(10);
                    rfctitle=(char *)malloc(256);
                    
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
                    char *ch;
                    char field[256];
                    ch = strstr(buf,"Host:");
                    hostname=(char *)malloc(256);
                    memset(hostname,'\0',strlen(hostname));
                    sscanf(ch,"%[^' '] %[^\n]",field,hostname);
                    
                    strncpy(command,buf,3);
                                
                    //Check if method ADD
                    if(strncmp(command,"ADD",3) == 0){
                   
                        int rfcid,uploadportno;

                        //Upload port no
                        ch = strstr(buf,"Port:");
                        sscanf(ch,"%s %d",field,&uploadportno);
                        
                        //Add host if unavailable
                        if(!isHostAvailable(hostname))
                            addClientToPeer(hostname,uploadportno);
                        
                        printf("ADD method call\n");
                        //Add rfc detail to the rfc detail list,rfcid
                        char *ch;
                        char field[256],field1[256];
                        ch = strstr(buf,"ADD");
                        printf("%s\n",ch);
                        sscanf(ch,"%s %s %d",field,field1,&rfcid);
                        
                        //rfctitle
                        memset(rfctitle,'\0',strlen(rfctitle));
                        ch = strstr(buf,"Title:");
                        sscanf(ch,"%[^' '] %[^\n]",field,rfctitle);
                        
                        addRFCDetail(rfcid,rfctitle,hostname);
                        
                        //Reply back to the client
                        char response[2048]="";
                        char *temp = "P2P-CI/1.0 200 OK\nRFC ";
                        strcat(response,temp);
                        char rfcid_string[32];
                        sprintf(rfcid_string,"%d",rfcid);
                        strcat(response,rfcid_string);
                        strcat(response," ");
                        strcat(response,rfctitle);
                        strcat(response," ");
                        strcat(response,hostname);
                        char port_string[32];
                        sprintf(port_string,"%d",uploadportno);
                        strcat(response,port_string);
                        send(i,response,strlen(response),0);
                        
                        printf("Add request response completed\n");
                        
                    }
                    
                    strncpy(command,buf,6);
                    
                    //Check if method GET
                    if(strncmp(command,"LOOKUP",3) == 0){
                        
                        int rfcid,uploadportno;
                        
                        printf("LOOKUP CALL\n");
                        char *responseheader = generateResponseHeader();
                        char response[4096]="";
                        strcat(response,responseheader);
                        
                        //Get rfcid
                        char *ch;
                        char field[256],field1[256];
                        ch = strstr(buf,"LOOKUP");
                        printf("%s\n",ch);
                        sscanf(ch,"%s %s %d",field,field1,&rfcid);
                        
                        rfcdetailnode* lookup;
                        lookup = getHostwithRFC(rfcid);
                        
                        while(lookup != NULL){
                            
                            char buf[10]="";
                            
                            sprintf(buf,"%d",lookup->rfcno);
                            strcat(response,buf);
                            strcat(response," ");
                            strcat(response,lookup->rfctitle);
                            strcat(response," ");
                            strcat(response,lookup->hostname);
                            strcat(response," ");
                            strcat(response,"\n");
                            
                            lookup = lookup->next;
                        }
                                                
                        printf("Lookup response : %s\n",response);
                        send(i,response,strlen(response),0);
                        
                    }
                    
                    strncpy(command,buf,6);
                    
                    //Check if method GET
                    if(strncmp(command,"LIST",3) == 0){
                        int rfcid,uploadportno;
                        
                        printf("LIST CALL\n");
                        char *responseheader = generateResponseHeader();
                        char response[4096]="";
                        strcat(response,responseheader);
                        
                        //Get rfcid
                        char *ch;
                        char field[256],field1[256];
                        ch = strstr(buf,"LIST");
                        printf("%s\n",ch);
                        sscanf(ch,"%s %s %d",field,field1,&rfcid);
                        
                        rfcdetailnode* list;
                        list = getList();
                        
                        while(list != NULL){
                            
                            char buf[10]="";
                            
                            sprintf(buf,"%d",list->rfcno);
                            strcat(response,buf);
                            strcat(response," ");
                            strcat(response,list->rfctitle);
                            strcat(response," ");
                            strcat(response,list->hostname);
                            strcat(response," ");
                            strcat(response,"\n");
                            
                            list = list->next;
                        }
                        
                        printf("List response : %s\n",response);
                        send(i,response,strlen(response),0);
                        
                    }
                    
                    printAll();
                    free(buf);
                    free(command);
                }
            }
        }
	}
	
	return 0;
}