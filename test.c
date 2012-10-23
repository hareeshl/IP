
#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include"list.h"

int addClientToPeer(char* hostname){
    peernode *temp = (peernode *)malloc(sizeof(peernode));
	temp->hostname = hostname;
	insertFrontPeerList(temp);
}

int main(){
    
    char *ch;
    char field[256],*hostname;
    int field2;
    char *buf = (char *)malloc(sizeof(512));
    hostname = (char *)malloc(sizeof(256));
    memset(hostname,'\0',strlen(hostname));
    buf="Host: nom0061218.nomadic.ncsu.edusfsdfs\n";
    int i=0;
    
    ch = strstr(buf,"Host:");
    sscanf(ch,"%[^' '] %[^\n]",field,hostname);
    
    printf("Substring =>%s,%s\n",field,hostname);
    
    addClientToPeer(hostname);
    
    buf = (char *)malloc(sizeof(512));
    hostname = (char *)malloc(sizeof(256));
    
    buf="Host: sfsdsssssadadasdasdadasdadfssdfsdfsdfsf\n";

    ch = strstr(buf,"Host:");
    sscanf(ch,"%[^' '] %[^\n]",field,hostname);
    
    printf("Substring =>%s,%s\n",field,hostname);
    
    addClientToPeer(hostname);
    
    printAll();
}