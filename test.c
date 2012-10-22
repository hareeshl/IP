
#include <stdio.h>
#include<string.h>

int main(){
    
    char *ch;
    char field[256],field1[256];
    int field2;
    char buf[512]="Title: Domain Names and Company Name Retrieval\n";
    
    ch = strstr(buf,"Title:");
    sscanf(ch,"%[^' '] %[^\n]",field,field1);
    
    printf("Substring =>%s,%s\n",field,field1);
}