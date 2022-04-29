#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "variables.h"
#include "io_helpers.h"


Node *front = NULL;
Node *create_node(char *k, char *v, Node *nex) {
    Node *new_node = malloc(sizeof(Node)); 
    char *dest = malloc(sizeof(char) * 64);
    char *app = malloc(sizeof(char) * 64);
    strncpy(dest, k, 64);
    strncpy(app,v, 64); 
    new_node->key = dest;
    new_node->value = app;
    new_node->next = nex;
    return new_node;
}

char *find_node(char *k) {
        Node *curr = front;
        while (curr != NULL && strncmp(curr->key, k,  MAX_STR_LEN) != 0) {   
        // printf("%s %s\n", "key node", curr->key); 
        // printf("%s %s\n","sent key", k); 
        // printf("%s %s\n", "value node", curr->value);   
        curr = curr->next;
        }
        if (curr != NULL){
            return curr->value;
            // printf("sucess");
        }
    return NULL;
}

int insert(char *k, char *v, Node **front_ptr) {

    *front_ptr = create_node(k, v, *front_ptr);
    return 0;
}

void var_set(char **tokens) {

    char *location;
    location = strstr(tokens[0], "=");
    *location = '\0';
    location = location + 1;

    
    if (location[0] == '$'){
        char *retu = find_node(location + 1);
        printf("%s", retu);
        if (retu != NULL){
            insert(tokens[0], retu, &front);
            return;
        }
    } 
    insert(tokens[0],location, &front);

}

void freeNodes(){

    Node *prev;
    while (front != NULL){
        prev = front;
        front = front->next;
        free(prev->key);
        free(prev->value);
        free (prev);
    }
    
    
}
