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
#define EXIT 4

struct utsname sysname;
char hostname[512];

void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

char* reqHeader(int sockfd,int option,int rfcid,char* title,char* uploadport){
	char* temp=NULL;
    char fulltext[2048]="";
    
	if(option == ADD){
		temp = "ADD RFC ";
        
        char rfcid_string[32];
        sprintf(rfcid_string,"%d",rfcid);
        
        //char port_string[32];
        //sprintf(port_string,"%d",uploadport);
        
        strcat(fulltext,temp);
        strcat(fulltext,rfcid_string);
        strcat(fulltext," P2P-CI/1.0\nHost: ");
        strcat(fulltext,hostname);
        strcat(fulltext,"\nPort: ");
        strcat(fulltext,uploadport);
        strcat(fulltext,"\nTitle: ");
        strcat(fulltext,title);
        strcat(fulltext,"\0");
        
        //printf("Sending %s\n",fulltext);
        
	}else if(option == LOOKUP){
        
		temp = "LOOKUP RFC ";
        char rfcid_string[32];
        sprintf(rfcid_string,"%d",rfcid);
        
        //char port_string[32];
        //sprintf(port_string,"%d",uploadport);
        
        strcat(fulltext,temp);
        strcat(fulltext,rfcid_string);
        strcat(fulltext," P2P-CI/1.0\nHost: ");
        strcat(fulltext,hostname);
        strcat(fulltext,"\nPort: ");
        strcat(fulltext,uploadport);
        strcat(fulltext,"\0");
        
        //printf("Sending %s\n",fulltext);
        
	}else if(option == LISTALL){
        
        temp = "LIST ALL ";
        
        //char port_string[32];
        //sprintf(port_string,"%d",uploadport);
        
        strcat(fulltext,temp);
        strcat(fulltext," P2P-CI/1.0\nHost: ");
        strcat(fulltext,hostname);
        strcat(fulltext,"\nPort: ");
        strcat(fulltext,uploadport);
        strcat(fulltext,"\0");
        
        //printf("Sending %s\n",fulltext);
        
        
	}else if(option == GET){
        
        temp = "GET RFC ";
        
        char rfcid_string[32];
        sprintf(rfcid_string,"%d",rfcid);
        
        strcat(fulltext,temp);
        strcat(fulltext,rfcid_string);
        
    }else if(option == EXIT){
        
        temp = "EXIT ";
        
        strcat(fulltext,temp);
        strcat(fulltext,"\n");
        strcat(fulltext,"Host: ");
        strcat(fulltext,hostname);
        
        //printf("Sending %s\n",fulltext);
        
    }
	
    sleep(1);
    
    if(send(sockfd,fulltext,strlen(fulltext),0) == -1)
        perror("send");
    
	return fulltext;
}

//This method is used to initialize the client by sending rfc details to the server
int initClient(int sockfd,char *uploadportno){
    
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
        reqHeader(sockfd,ADD,id,title,uploadportno);
        buf=(char *)malloc(MAXDATASIZE);
        numbytes = recv(sockfd,buf,MAXDATASIZE-1,0);
        buf[numbytes] = '\0';
        
        printf("%s\n",buf);
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

int sendGet(int serversockfd,int rfcid,char *uploadportno){
    
    //Lookup
    char* buf;
    int numbytes;
    
    reqHeader(serversockfd,LOOKUP,rfcid,NULL,uploadportno);
    buf=(char *)malloc(MAXDATASIZE);
    numbytes = recv(serversockfd,buf,MAXDATASIZE-1,0);
    buf[numbytes] = '\0';
    char *str;
    char field[256],field1[256],hostname[256];
    int portno;
    
    str = strstr(buf,"\n");
    //printf("%s\n",str);
    sscanf(str,"%s %s %s %d",field,field1,hostname,&portno);
    
    //printf("Hostname :%s, portno: %d",hostname,portno);
    
    //----End of lookup
    
    //Create a socket to connect to the upload server
    int getfilesocketfd;
    struct addrinfo getfilestruct,*getfileserver;
    
    /*struct hostent* h;
     h = gethostbyname(hostname);
     
     if( h == NULL){
     printf("Unknown host\n");
     }
     
     struct sockaddr_in server;
     
     memcpy(&server.sin_addr,h->h_addr_list[0], h->h_length);
     server.sin_family= AF_INET;
     server.sin_port = htons(portno);*/
    
    char port_string[32];
    sprintf(port_string,"%d",portno);
    
    int status,sockfd;
    struct addrinfo hints,*res,getstruct,*me;
    
    //Server details
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    status = getaddrinfo(hostname,port_string,&hints,&res);
    
    if(status != 0){
        perror("Getaddrinfo");
        return;
    }
    
    //Create a socket
    if((getfilesocketfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol)) == -1){
        perror("Server:socket creating error");
    }
    
    if(connect(getfilesocketfd,res->ai_addr,res->ai_addrlen) == -1){
        close(sockfd);
        perror("Client : Connect");
        exit(1);
    }
    
    
    numbytes=-1;
    
    char received[1];
    
    //printf("Get request for : %d\n",rfcid);
    reqHeader(getfilesocketfd,GET,rfcid,NULL,NULL);
    
    //Create a copy of the rfc
    FILE *file;
    char *ext="(cpy).txt";
    char filename[1024];
    sprintf(filename,"%d%s",rfcid,ext);
    
    file = fopen(filename,"w");
    
    char content[1024];
    
    int bytesread = 0;
    
    while(numbytes != 0){
        numbytes = recv(getfilesocketfd,received,1,0);
        fprintf(file,"%c",received[0]);
        //bytesread += numbytes;
    }
    
    //buf[bytesread] = '\0';
    close(getfilesocketfd);
    fclose(file);
    
    //ADD detail to server
    reqHeader(serversockfd,ADD,rfcid,"title",uploadportno);
    
}

char* sendLookup(int sockfd, int rfcid,char *uploadportno){
    
    char* buf;
    int numbytes;
    
    reqHeader(sockfd,LOOKUP,rfcid,NULL,uploadportno);
    buf=(char *)malloc(MAXDATASIZE);
    numbytes = recv(sockfd,buf,MAXDATASIZE-1,0);
    buf[numbytes] = '\0';
    
    printf("%s\n",buf);
}

int sendList(int sockfd,char* uploadportno){
    
    char* buf;
    int numbytes;
    
    reqHeader(sockfd,LISTALL,NULL,NULL,uploadportno);
    buf=(char *)malloc(MAXDATASIZE);
    numbytes = recv(sockfd,buf,MAXDATASIZE-1,0);
    buf[numbytes] = '\0';
    
    printf("%s\n",buf);
    
}

void sendExit(int sockfd){
    reqHeader(sockfd,EXIT,NULL,NULL,NULL);
    //printf("End of exit\n");
}

int main(int argc,char *argv[]){
    
	char* PORTNO = "5556";
    char* uploadp = argv[2];
	char* SERVERIP = argv[1];
    
    printf("Serverip: %s, uploadport: %s\n",argv[1],argv[2]);
    
    pid_t pid = fork();
    
    if( pid > 0){
        //Parent process
        
        //Populate the OS name
        if(uname(&sysname)){
            perror("Error obtaining os name");
            exit(-1);
        }
        
        hostname[511]='\0';
        char localname[512];
        
        gethostname(localname,511);
        int sx;
        struct addrinfo hints1,*info,*p;
        
        memset(&hints1, 0, sizeof hints1);
        hints1.ai_family = AF_UNSPEC; /*either IPV4 or IPV6*/
        hints1.ai_socktype = SOCK_STREAM;
        hints1.ai_flags = AI_CANONNAME;
        
        if ((sx = getaddrinfo(localname, "http", &hints1, &info)) != 0) {
            //fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_result));
            exit(1);
        }
        
        //for(p = info; p != NULL; p = p->ai_next) {
        printf("canonical hostname: %s\n", info->ai_canonname);
        strcpy(hostname,info->ai_canonname);
        //}
        
        freeaddrinfo(info);
        
        char *buf;
        int status,sockfd,numbytes;
        struct addrinfo hints,*res,getstruct,*me;
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
        
        //inet_ntop(res->ai_family,get_in_addr((struct sockaddr *)res->ai_addr),s,sizeof(s));
        //gethostname(hostname,1023);
        printf("Hostname : %s\n",hostname);
        //printf("Client connecting to : %s\n",s);
        
        //Initialize client
        initClient(sockfd,uploadp);
        
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
                    
                    //List all
                case 1:
                    sendList(sockfd,uploadp);
                    break;
                    
                    //Lookup
                case 2:
                    printf("Enter the rfc id :");
                    scanf("%d",&id);
                    sendLookup(sockfd,id,uploadp);
                    printf("\n");
                    break;
                    
                    //Get
                case 3:
                    
                    printf("Enter the rfc id :");
                    scanf("%d",&id);
                    sendGet(sockfd,id,uploadp);
                    printf("\n");
                    break;
                    
                    //Exit
                case 4:
                    sendExit(sockfd);
                    kill(pid,SIGINT);
                    close(sockfd);
                    exit(0);
                    
                    break;
                    
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
        getstruct.ai_socktype = AI_PASSIVE;
        
        getaddrinfo(NULL,uploadp,&getstruct,&me);
        
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
            
            //printf("Received %s:",uploadbuf);
            
            char *str,field[256],field1[256];
            int rfcid;
            
            str = strstr(uploadbuf,"GET");
            sscanf(str,"%s %s %d",field,field1,&rfcid);
            //printf("Rfcid: %d:",rfcid);
            
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
                //printf("%c",tosend[0]);
                if( (send(uploadfd,&tosend[0],1,0)) == -1){
                    perror("Upload send error");
                }
            }
            
            fclose(file);
            
            close(uploadfd);
            //printf("End uploadserver\n");
        }
    }
    
	return 0;
}