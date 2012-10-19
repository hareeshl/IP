#include <stdio.h>

typedef struct{
	char *hostname;
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