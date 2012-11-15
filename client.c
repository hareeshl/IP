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
#define GET 0
#define ADD 1
#define LOOKUP 2
#define LISTALL 3
#define UPLOADPORT 8058

struct utsname sysname;
char hostname[512];

void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

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
        
        //printf("Sending %s\n",fulltext);
        
	}else if(option == LOOKUP){
        
		temp = "LOOKUP RFC ";
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
        strcat(fulltext,"\0");
       
        //printf("Sending %s\n",fulltext);
 
	}else if(option == LISTALL){
        
        temp = "LIST ALL ";
        
        char port_string[32];
        sprintf(port_string,"%d",UPLOADPORT);
        
        strcat(fulltext,temp);
        strcat(fulltext," P2P-CI/1.0\nHost: ");
        strcat(fulltext,hostname);
        strcat(fulltext,"\nPort: ");
        strcat(fulltext,port_string);
        strcat(fulltext,"\0");
        
        //printf("Sending %s\n",fulltext);

        
	}else if(option == GET){
        
        temp = "GET RFC ";
        
        char rfcid_string[32];
        sprintf(rfcid_string,"%d",rfcid);
        
        strcat(fulltext,temp);
        strcat(fulltext,rfcid_string);
                
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
    int id,numbytes;
    char title[256];
    char* buf;
    
    fp = fopen("index.txt","r");
    if(fp == NULL){
        perror("Rfc index not found");
        exit(-1);
    }
    
    while(fscanf(fp,"%d %[^\t\n]",&id,title) == 2){
        //Send add requests to the server
        reqHeader(sockfd,ADD,id,title);
        buf=(char *)malloc(MAXDATASIZE);
        numbytes = recv(sockfd,buf,MAXDATASIZE-1,0);
        buf[numbytes] = '\0';
        
        printf("Response from server : %s\n\n",buf);
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

int sendGet(int sockfd,int rfcid){
    int numbytes=-1;
    char *buf=(char *)malloc(MAXDATASIZE);
    
    printf("Get request for : %d\n",rfcid);
    reqHeader(sockfd,GET,rfcid,NULL);
    
    //Create a copy of the rfc
    FILE *file;
    char *ext="(cpy).txt";
    char filename[1024];
    sprintf(filename,"%d%s",rfcid,ext);
    
    file = fopen(filename,"w");
    
    int bytesread = 0;
    
    while(numbytes != 0){
        numbytes = recv(sockfd,buf,MAXDATASIZE-1,0);
        bytesread += numbytes;
    }
    
    buf[bytesread] = '\0';
    
    fprintf(file,"%s",buf);
    
    fclose(file);
}

char* sendLookup(int sockfd, int rfcid){

    char* buf;
    int numbytes;
    
    reqHeader(sockfd,LOOKUP,rfcid,NULL);
    buf=(char *)malloc(MAXDATASIZE);
    numbytes = recv(sockfd,buf,MAXDATASIZE-1,0);
    buf[numbytes] = '\0';
    
    printf("Lookup response from server : %s\n\n",buf);
    
    char *str;
    
    char field[256],field1[256],hostname[256];
    int portno;
    
    str = strstr(buf,"\n");
    printf("%s\n",str);
    sscanf(str,"%s %s %s %d",field,field1,hostname,&portno);
    
    printf("Hostname :%s, portno: %d",hostname,portno);

}

int sendList(int sockfd){
    
    char* buf;
    int numbytes;
    
    reqHeader(sockfd,LISTALL,NULL,NULL);
    buf=(char *)malloc(MAXDATASIZE);
    numbytes = recv(sockfd,buf,MAXDATASIZE-1,0);
    buf[numbytes] = '\0';
    
    printf("List response from server : %s\n",buf);
    
}

int main(int argc,char *argv[]){

	char* PORTNO = "5542";
    char* uploadp = "8059";
	char* SERVERIP = "127.0.0.1";
    
    pid_t pid = fork();
    
    if( pid > 0){
        //Parent process
    
        //Populate the OS name
        if(uname(&sysname)){
            perror("Error obtaining os name");
            exit(-1);
        }
    
        hostname[511]='\0';
        gethostname(hostname,511);
        
        char *buf;
        int status,sockfd,numbytes;
        struct addrinfo hints,getfilestruct,*res,*getfileserver,getstruct,*me;
        socklen_t addr_size;
        char s[INET6_ADDRSTRLEN];
        struct sigaction sa;
        
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
        
        //Initialize client
        initClient(sockfd);
        
        fflush(stdout);
        
        int opt =0;
        int id=0;
        printf("Select an option\n");
        
        while(1) {
            
            printf("1. List\n");
            printf("2. Lookup\n");
            printf("3. Get\n");
            printf("4. Exit\n");
            printf("Enter the option: ");
            
            scanf("%d",&opt);
            
            switch (opt) {
                case 1:
                    sendList(sockfd);
                    break;
                    
                case 2:
                    printf("Enter the rfc id :");
                    scanf("%d",&id);
                    sendLookup(sockfd,id);
                    break;
                    
                case 3:
                    //Send lookup
                    
                    sendLookup(sockfd,id);
                    
                    //Create a socket to connect to the upload server
                    
                    int getfilesocketfd;
                    char *uploadserverport = "8057";
                    printf("Enter the rfc id :");
                    scanf("%d",&id);
                    
                    memset(&getfilestruct,0,sizeof(getfilestruct));
                    getfilestruct.ai_family = AF_UNSPEC;
                    getfilestruct.ai_socktype = SOCK_STREAM;
                    
                    getaddrinfo(SERVERIP,uploadserverport,&getfilestruct,&getfileserver);
                    
                    if((getfilesocketfd = socket(getfileserver->ai_family,getfileserver->ai_socktype,getfileserver->ai_protocol)) == -1){
                        perror("Server:upload socket creation error");
                    }
                    
                    if(connect(getfilesocketfd,getfileserver->ai_addr,getfileserver->ai_addrlen) == -1){
                        close(getfilesocketfd);
                        perror("Client : Connect");
                        exit(1);
                    }
                    
                    sendGet(getfilesocketfd,id);
                    break;
                    
                case 4:
                    printf("Exit yet to be implemented\n");
                    kill(pid,SIGINT);
                    close(sockfd);
                    exit(0);
                    
                default:
                    printf("Please select a valid option\n");
                    break;
            }
            
        }
    }
    
    //Start upload server
    else if(pid == 0){
        
        //Child process
        
        struct addrinfo getstruct,*me;
        
        //Create upload socket
        int newfd;
        memset(&getstruct,0,sizeof(getstruct));
        getstruct.ai_family = AF_UNSPEC;
        getstruct.ai_socktype = SOCK_STREAM;
        
        getaddrinfo(SERVERIP,uploadp,&getstruct,&me);
        
        if((newfd = socket(me->ai_family,me->ai_socktype,me->ai_protocol)) == -1){
            perror("Server:upload socket creation error");
        }
        
        if(bind(newfd,me->ai_addr,me->ai_addrlen) == -1){
            close(newfd);
            perror("Server : upload bind");
        }
        if(listen(newfd,10) == -1){
            perror("upload listen error");
            exit(1);
        }
        
        while(1){
            
            struct sockaddr_storage their_addr;
            socklen_t sin_size;
            sin_size = sizeof(their_addr);
            
            int uploadfd = accept(newfd,(struct sockaddr *)&their_addr, &sin_size);
            
            if(uploadfd == -1){
                perror("upload socket accept error");
            }
            
            char uploadbuf[MAXDATASIZE];
            int n;
            
            n = recv(uploadfd,uploadbuf,MAXDATASIZE-1,0);
            uploadbuf[n] = '\0';
           
            printf("Received %s:",uploadbuf);
            
            char *str,field[256],field1[256];
            int rfcid;

            str = strstr(uploadbuf,"GET");
            sscanf(str,"%s %s %d",field,field1,&rfcid);
            printf("Rfcid: %d:",rfcid);
            
            char *ext=".txt";
            char filename[1024];
            
            sprintf(filename,"%d%s",rfcid,ext);
            FILE *file = fopen(filename,"r");
            
            if(!file){
                perror("File read error\n");
            }
            
            fseek(file,0,SEEK_END);
            int filesize = ftell(file);
            rewind(file);
            char tosend[1],ch;
            
            while ((ch = getc(file)) != EOF) {
                tosend[0] = ch;
                printf("%c",tosend[0]);
                if( (send(uploadfd,&tosend[0],1,0)) == -1){
                    perror("Upload send error");
                }
            }
            
            fclose(file);
            
            close(uploadfd);
            printf("End uploadserver\n");
        }
    }
    
	return 0;
}