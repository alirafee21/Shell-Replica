#ifndef __COMMANDS_H__
#define __COMMANDS_H__

typedef struct node1 {
    int key;
    char *value;
    int value2; 
    char *value3;
    struct node1 *next;
    } Node1;

char *find_nod(char *k);
void var_sett(int pidd, char *tokens, char *v3, int v2);
void freeNode();
void displayps();
void signalHandler(int code);
int find_a(int k);
int getprmptc();
int is_empty();
void add_count();
void SIGINThandler(int code);
void display_dones();
void freeNode2();

#endif