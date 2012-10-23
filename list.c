#include "list.h"

peernode *peershead = NULL;
rfcdetailnode *rfcdetailhead = NULL;

int insertFrontPeerList(peernode* newnode){
	if(peershead != NULL){
		peernode* temp = peershead;
		newnode->next = temp;
		peershead = newnode;
	}else{
		peershead = newnode;
		peershead->next = NULL;
	}
}

int insertFrontrfcList(rfcdetailnode* newnode){
	if(rfcdetailhead != NULL){
		rfcdetailnode* temp = rfcdetailhead;
		newnode->next = temp;
		rfcdetailhead = newnode;
	}else{
		rfcdetailhead = newnode;
		rfcdetailhead->next = NULL;
	}
}

void printAll(){
	peernode* temp = peershead;
	printf("Peers : ");
	while(temp != NULL){
		printf("%s\t",temp->hostname);
		temp = temp->next;
	}
	printf("\n");
	
	rfcdetailnode* temp1 = rfcdetailhead;
	printf("rfc's : \n");
	while(temp1 != NULL){
		printf("%d : %s : %s \n",temp1->rfcno,temp1->rfctitle,temp1->hostname);
		temp1 = temp1->next;
	}
	printf("\n");
}

/*int main(){
	printf("List test\n");
	
    char hstname[256]="sys1";
	peernode temp;
	strcpy(temp.hostname,hstname);
	insertFrontPeerList(&temp);
    
	char hstname1[256]="sys2";
	peernode temp2;
	strcpy(temp2.hostname,hstname1);
	insertFrontPeerList(&temp2);

	
	rfcdetailnode *temp1 = malloc(sizeof(rfcdetailnode));
	temp1->rfcno =1;
	temp1->rfctitle = "Test";
	temp1->hostname = "sys1";
	insertFrontrfcList(temp1);
	
	rfcdetailnode *temp3 = malloc(sizeof(rfcdetailnode));
	temp3->rfcno =2;
	temp3->rfctitle = "choke";
	temp3->hostname = "sys2";
	insertFrontrfcList(temp3);
	
	printAll();
	
	return 0;
}*/