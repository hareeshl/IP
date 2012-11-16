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
		printf("%d : %s : %s : %d\n",temp1->rfcno,temp1->rfctitle,temp1->hostname,temp1->uploadportno);
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

bool deletePeer(char *hostname){
    
    peernode* temp = peershead;
    peernode* prev;
    
    while(temp != NULL){
        
        if(strcmp(temp->hostname,hostname) == 0){
            
            if(temp == peershead){
                peershead = temp->next;
                free(temp);
                break;
            }
            
            prev->next = temp->next;
            free(temp);
        }
        
        prev = temp;
        temp = temp->next;
    }
}

bool deleteRFCdetailnode(char *hostname){
    rfcdetailnode* temp = rfcdetailhead;
    rfcdetailnode* prev = NULL;
    
    while(temp != NULL){
        
        if(strcmp(temp->hostname,hostname) == 0){
            
            if(temp == rfcdetailhead){
                rfcdetailhead = temp->next;
            }else{
                prev->next = temp->next;
            }
        }else{
            prev = temp;
        }
            temp = temp->next;
    }
}

rfcdetailnode* getList(){
    return rfcdetailhead;
}

//Get hosts with rfcid
rfcdetailnode* getHostwithRFC(int rfcid){
    rfcdetailnode* temp = rfcdetailhead;
    rfcdetailnode* nodes = NULL;
    rfcdetailnode* nodescont= NULL;
    
    while(temp != NULL){
        if(temp->rfcno == rfcid){
            if(nodes == NULL){
                nodes = (struct rfcdetailnode *)malloc(sizeof(rfcdetailnode));
                nodes->rfcno = rfcid;
                nodes->hostname=temp->hostname;
                nodes->rfctitle=temp->rfctitle;
                nodes->uploadportno = temp->uploadportno;
                nodes->next = NULL;
            }else{
                nodescont = (struct rfcdetailnode *)malloc(sizeof(rfcdetailnode));
                nodescont->rfcno = rfcid;
                nodescont->hostname=temp->hostname;
                nodescont->rfctitle=temp->rfctitle;
                nodescont->uploadportno = temp->uploadportno;
                nodescont->next = nodes;
                nodes = nodescont;
            }
        }
            
        temp = temp->next;
    }
    
    if(nodescont == NULL)
        return nodes;
    else if(nodes == NULL)
        return NULL;
    else 
        return nodescont;
}
/*
int main(){
	printf("List test\n");
	
	peernode *temp = malloc(sizeof(peernode));
	temp->hostname="sys1";
	insertFrontPeerList(temp);
    
	peernode *temp2= malloc(sizeof(peernode));
	temp2->hostname="sys2";		
	insertFrontPeerList(temp2);
    
    peernode *tempa= malloc(sizeof(peernode));
	tempa->hostname="sys3";
	insertFrontPeerList(tempa);
    
    printf("ishostavailable :%s\n",(isHostAvailable("sys1"))?"true":"false");
    printf("ishostavailable :%s\n",(isHostAvailable("sys2"))?"true":"false");
    printf("ishostavailable :%s\n",(isHostAvailable("sys3"))?"true":"false");
	
	rfcdetailnode *temp1 = malloc(sizeof(rfcdetailnode));
	temp1->rfcno =1;
	temp1->rfctitle = "Test";
	temp1->hostname = "sys1";
    temp1->uploadportno = 1000;
	insertFrontrfcList(temp1);
	
	rfcdetailnode *temp3 = malloc(sizeof(rfcdetailnode));
	temp3->rfcno =2;
	temp3->rfctitle = "choke";
	temp3->hostname = "sys1";
    temp3->uploadportno = 1001;
	insertFrontrfcList(temp3);
    
    temp3 = malloc(sizeof(rfcdetailnode));
	temp3->rfcno =2;
	temp3->rfctitle = "choke";
	temp3->hostname = "sys2";
    temp3->uploadportno = 1002;
	insertFrontrfcList(temp3);
	
    temp3 = malloc(sizeof(rfcdetailnode));
	temp3->rfcno =2;
	temp3->rfctitle = "Test";
	temp3->hostname = "sys3";
    temp3->uploadportno = 1003;
	insertFrontrfcList(temp3);
    
	printAll();
	
    deletePeer("sys1");
    
    deleteRFCdetailnode("sys3");
    
    printAll();
    
	return 0;
}*/