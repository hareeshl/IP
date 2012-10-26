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
		printf("%s:%d\t",temp->hostname,temp->uploadportno);
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

bool isHostAvailable(char *hostname){
    
    peernode* temp = peershead;
    
    while(temp != NULL){
        if(strcmp(temp->hostname,hostname) == 0)
            return true;
        temp = temp->next;
    }
    
    return false;
}

/*int main(){
	printf("List test\n");
	
	peernode *temp = malloc(sizeof(peernode));
	temp->hostname="sys1";
	insertFrontPeerList(temp);
    
	peernode *temp2= malloc(sizeof(peernode));
	temp2->hostname="sys2";		
	insertFrontPeerList(temp2);

    
    printf("ishostavailable :%s\n",(isHostAvailable("sys1"))?"true":"false");
    printf("ishostavailable :%s\n",(isHostAvailable("sys2"))?"true":"false");
    printf("ishostavailable :%s\n",(isHostAvailable("sys3"))?"true":"false");
	
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