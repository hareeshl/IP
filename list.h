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
    int uploadportno;
	struct rfcdetailnode *next;
}rfcdetailnode;

typedef struct{
	int rfcno;
	char *rfctitle;
	char *hostname;
    int uploadportno;
	struct rfcdetailnode *next;
}rfc;

int insertFrontPeerList(peernode* newnode);
int insertFrontrfcList(rfcdetailnode* newnode);
void printAll();
bool isHostAvailable(char *hostname);
rfcdetailnode* getHostwithRFC(int rfcid);
rfcdetailnode* getList();
