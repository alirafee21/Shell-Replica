#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

#include "builtins.h"
#include "io_helpers.h"
#include "variables.h"
#include "commands.h"


int main(int argc, char* argv[]) {
    char *prompt = "mysh$ "; // TODO Step 1, Uncomment this.
    char input_buf[MAX_STR_LEN + 1];
    input_buf[MAX_STR_LEN] = '\0';
    char *token_arr[MAX_STR_LEN] = {NULL};
    char *token_arr2[MAX_STR_LEN] = {NULL};
    char input_buf2[MAX_STR_LEN + 1];
    input_buf2[MAX_STR_LEN] = '\0';

    char *token_arr3[MAX_STR_LEN] = {NULL};
    char input_buf3[MAX_STR_LEN + 1];
    input_buf3[MAX_STR_LEN] = '\0';

    char *tokens3[MAX_STR_LEN][MAX_STR_LEN];

    signal(SIGINT, SIGINThandler);
    extern int count;
   


    // extern int prm;
    while (1) {
        // Prompt and input tokenization

        // TODO Step 2:
        // Display the prompt via the display_message function.

        display_message(prompt);
        int ret = get_input(input_buf);
        strcpy(input_buf2, input_buf);
        strcpy(input_buf3, input_buf2);
        size_t token_count = tokenize_input(input_buf, token_arr);
        size_t token_count2 = tokenize_input2(input_buf2, token_arr2);
        tokenize_input3(input_buf3, token_arr3);
        // Clean exit
        
        if (ret != -1 && ((token_count == 0)|| (strncmp("exit", token_arr[0], strlen(token_arr[0])) == 0))) {
            if (ret == 1 && token_count == 0 && token_arr[0] == NULL) {
                
            //   continue;  
            }             
            else{
                break;
            }            
        }

        // PIPES execution
        if (token_count2 > 1){
            int fd[token_count2][2];
            for(int i = 0; i < token_count2; i++){
                if (pipe(fd[i]) == -1){
                perror("pipe");
                return -1;      
                } 
                // int status;
                int child = fork();
                if (child < 0) {
                    display_erro("failed child");
                    return -1;
                }
                if(child == 0){
                if(i == 0 ){
                close(fd[i][0]);
                int child_no;
                for (child_no = 0; child_no < i; child_no += 2) {
                if (close(fd[child_no][0]) == -1) {
                    perror("close reading ends of previously forked children");
                }
                }
                dup2(fd[i][1], STDOUT_FILENO);
                tokenize_input(token_arr2[i], tokens3[i]);
                bn_ptr builtin_fn3 = check_builtin(tokens3[i][0]);
                //Use the function
                size_t index = 0;
                while (tokens3[i][index] != NULL){
                if (tokens3[i][index][0] == '$') { 
                char *location;
                location = strstr(tokens3[i][index], "$");
                char *finds;
                finds = find_node(location+1);   
                if (finds != NULL) {
                tokens3[i][index] = finds;
                }  
                }
                index += 1; 
                }
                if (builtin_fn3 != NULL) {
                ssize_t err = builtin_fn3(tokens3[i]);
                if (err == - 1) {
                    display_error("ERROR: Builtin failed: ", tokens3[i][0]);
                }
                } else {
                if (strstr(tokens3[i][0], "=") != NULL && (tokens3[i][1]) == NULL)
                {
                    var_set(tokens3[i]);                   
                }
                else{
                    if (execvp(tokens3[i][0], tokens3[i]) < 0) {
                        display_error("ERROR: Unrecognized command: ", tokens3[i][0]);
                    }
                }
                }

                // close the write end of pipe
                close(fd[i][1]);
                exit(0);             
                }
                else {
                close(fd[i-1][1]);
                dup2(fd[i-1][0], STDIN_FILENO);
                if ((i+1) != token_count2){
                    close(fd[i-1][0]);
                    dup2(fd[i][1], STDOUT_FILENO);
                }
                tokenize_input(token_arr2[i], tokens3[i]);
                size_t index = 0;
                while (tokens3[i][index] != NULL){
                if (tokens3[i][index][0] == '$') { 
                char *location;
                location = strstr(tokens3[i][index], "$");
                char *finds;
                finds = find_node(location+1);
            
                if (finds != NULL) {
                tokens3[i][index] = finds;
                }  
                }
                index += 1; 
                }
                bn_ptr builtin_fn3 = check_builtin(tokens3[i][0]);
                if (builtin_fn3 != NULL) {
                ssize_t err = builtin_fn3(tokens3[i]);
                if (err == - 1) {
                    display_error("ERROR: Builtin failed: ", tokens3[i][0]);
                }
                } else {
                if (strstr(tokens3[i][0], "=") != NULL && (tokens3[i][1]) == NULL)
                {
                    var_set(tokens3[1]);                   
                }
                 else{
                    if (execvp(tokens3[i][0], tokens3[i]) < 0) {
                        display_error("ERROR: Unrecognized command: ", tokens3[i][0]);
                    }
                }
                }
                if (i + 1 != token_count2){
                    close(fd[i][1]);
                }
                if ((i+1) == token_count2){
                    close(fd[i-1][0]);
                }
                else{
                    close(fd[i][1]);
                }
                exit(0);
                }
                }
                // parent
                else{
                    
                    wait(NULL);
                    close(fd[i][1]);
                    
                    // wait(NULL);
                    
                } 
            }
            // wait(NULL);
            // wait(NULL);
        }
        else if (token_count >= 1) 
        {
            size_t index = 0;
            while (token_arr[index] != NULL){
                if (token_arr[index][0] == '$') { 
                char *location;
                location = strstr(token_arr[index], "$");
                char *finds;
                finds = find_node(location+1);
            
                if (finds != NULL) {
                token_arr[index] = finds;
                }  
            }
             index += 1; 
            }
            bool fork_flag = false;

            if (token_arr[token_count-1][0] == '&'){
                fork_flag = true;
                token_arr[token_count-1] = NULL;
            }

            bn_ptr builtin_fn = check_builtin(token_arr[0]);

            if (builtin_fn != NULL) {
                if (fork_flag) {
                    signal(SIGCHLD, signalHandler);
                    int child = fork();
                    if (child == -1) {
                    perror("fork\n");

                    }
                 
                    
                    
                    if (child == 0) {
                            ssize_t err = builtin_fn(token_arr);
                            if (err == - 1) 
                            {
                             exit(2);
                             return -1;
                            }
                            exit(0);
                           
                        }
                        else
                        {       
                        if (fork_flag) {
                        if (is_empty() == 1){
                            add_count();
                        }
                        char pr[MAX_STR_LEN];
                        sprintf(pr, "[%d]",count);
                        display_message(pr);
                        display_message("  ");
                        char proc_id[MAX_STR_LEN];
                        sprintf(proc_id, "%d", child);
                        display_message(proc_id);
                        display_message("\n");
                        var_sett(child, token_arr[0], "ban", count);
                        }
                        }
                }
                if (!fork_flag){
                ssize_t err = builtin_fn(token_arr);
                if (err == - 1) 
                {
                    display_error("ERROR: Builtin failed: ", token_arr[0]);
                }
                }
            } 
            else 
            {
                if (strstr(token_arr[0], "=") != NULL && (token_arr[1]) == NULL)
                {
                    var_set(token_arr);                   
                }
                
                else
                {
                int status;
                if (fork_flag){
                    signal(SIGCHLD, signalHandler);
                }
                int child = fork();
                if (child == -1) {
                    display_erro("fork");
                }
                
                else if (child == 0){
                    
                    if (execvp(token_arr[0], token_arr) == -1) {
                        exit(EXIT_FAILURE);
                        return -1;
                    }
                    else{
                        exit(0);
                    }

                    
                }
                else 
                {
                     
                    if(!fork_flag){
                        wait(&status);
                    if (WEXITSTATUS(status)){
                        int exit_status = WEXITSTATUS(status); 
                    if (exit_status == 1){
                        display_error("ERROR: Unrecognized command: ", token_arr[0]);

                        }
                    }
                    }

                    if (fork_flag) {
                    if (is_empty() == 1){
                        add_count();
                    }
                  
                    char pr[MAX_STR_LEN];
                    sprintf(pr, "[%d]",count);
                    display_message(pr);
                    display_message("  ");
                    char proc_id[MAX_STR_LEN];
                    sprintf(proc_id, "%d", child);
                    display_message(proc_id);
                    display_message("\n");
                    var_sett(child, token_arr[0], token_arr3[0], count);
                    }
                    
                    }
                }
            }
        }
        display_dones();

    }

    freeNode();
    freeNodes();
    freeNode2();
    bn_closeserver();
    return 0;
}
