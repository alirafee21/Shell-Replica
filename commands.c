#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "io_helpers.h"
#include "commands.h"
#include <unistd.h>
#include <sys/wait.h>

Node1 *psfront = NULL;
int prm = 0;
int count = 1;
Node1 *sfront = NULL;

Node1 *make_node(int k, char *v, int v2, char *v3, Node1 *nex) {
    Node1 *node3 = malloc(sizeof(Node1)); 
    char *app = malloc(sizeof(char) * 64);
    char *dest2 = malloc(sizeof(char) * 64);
    
    strncpy(app, v, 64); 
    strncpy(dest2, v3, 64); 
    node3->key = k;
    node3->value = app;
    node3 ->value2 = v2;
    node3->value3 = dest2;
    node3->next = nex;
    return node3;
}

extern int client_soc; 
void SIGINThandler(int code)
{
    if (client_soc > 0)
    {
        close(client_soc);
    }
    display_message("\n");
    display_message("mysh$ ");
}



int is_empty(){
    if (psfront == NULL){
        count = 1;
        return 0;
    }   
    return 1;
}


void add_count(){
    count +=1;
}




Node1 *find_node1(int k) {
        Node1 *curr = psfront;
        while (curr != NULL &&  k != curr->key) {    
        curr = curr->next;
        }
        if (curr != NULL){
            return curr;
        }
    return NULL;
}


int find_a (int k) {
        Node1 *curr = psfront;
        while (curr != NULL && k != curr->key) 
        {      
        curr = curr->next;
        }
        if (curr != NULL){
            return 0;

        }
    return -1;
}

void delete (int k, Node1 **front_ptr) {
     Node1 *curr = *front_ptr;
     Node1 *prev = *front_ptr;
    if (curr == NULL){
        display_erro("pid not found");
    }
    
    if((*front_ptr) != NULL && (*front_ptr)->key == k){
        
        (*front_ptr) = curr->next;
        free(curr->value);
        free(curr->value3);
        free(curr);
        return;
    }

    while (curr != NULL && curr->key != k){
        prev = curr;
        curr = curr->next;
    }

    if (curr == NULL){
        display_erro("failure on del");
        return;
    }
    
    
    prev->next = curr->next;
    free(curr->value);
    free(curr->value3);
    free(curr);

    


}


void display_dones(){
    Node1 *curr = sfront;

    while (curr != NULL)
    {
    char numb[MAX_STR_LEN];
    sprintf(numb, "[%d]", curr->value2);
    display_message(numb);
    display_message("+  Done             ");
    display_message(curr->value3);
    display_message("\n");

    curr = curr->next;
       
    }

    freeNode2();

}
void displayps()
{
        Node1 *curr = psfront;
        while (curr != NULL )  {   
        display_message(curr->value);
        display_message(" ");
        char proc_id[MAX_STR_LEN];
        sprintf(proc_id, "%d", curr->key);
        display_message(proc_id);   
        display_message("\n");
        curr = curr->next;
        }
        
}


int add(int k, char *v, int v2, char *v3, Node1 **front_ptr) {

    *front_ptr = make_node(k, v, v2, v3, *front_ptr);
    return 0;
}

void var_sett(int pidd, char *tokens, char *v3, int v2) {
    add(pidd, tokens, v2, v3, &psfront);
}



int prmptc = 0;
void signalHandler(int code){
	int pid = 0;

	while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
        
        if (find_a(pid) == -1){
            return;
        }  
        else{
             Node1 *ap = find_node1(pid);

        if (ap != NULL){
            add(ap->key, ap->value, ap->value2, ap->value3, &sfront);
        }
        delete(pid, &psfront); 

        }

       

	}

    }



int getprmptc(){
    return prmptc;
}



void freeNode(){
    Node1 *prev;
    while (psfront != NULL){
        prev = psfront;
        psfront = psfront->next;
        free(prev->value);
        free(prev->value3);
        free (prev);
    }
}
void freeNode2(){

    Node1 *prev;
    while (sfront != NULL){
        prev = sfront;
        sfront = sfront->next;
        free(prev->value);
        free(prev->value3);
        free (prev);
    }

}
