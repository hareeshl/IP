#include <stdio.h>

#define bool int
#define true 1
#define false 0

typedef struct{
	char *hostname;
    int uploadportno;
	struct peernode *next;
}peernode;

typedef struct{
	int rfcno;
	char *rfctitle;
	char *hostname;
	struct rfcdetailnode *next;
}rfcdetailnode;

int insertFrontPeerList(peernode* newnode);
int insertFrontrfcList(rfcdetailnode* newnode);
void printAll();
bool isHostAvailable(char *hostname);