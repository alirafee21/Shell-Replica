#ifndef __VARIABLES_H__
#define __VARIABLES_H__

#define VARCAL "$"
#define EQUAL "=" 

typedef struct node{
    char *key;
    char *value;
    struct node *next;
    } Node;

char *find_node(char *k);
void var_set(char **tokens);
void freeNodes();

#endif