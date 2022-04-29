#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <dirent.h>
#include <stdbool.h>
#include <limits.h>
#include <poll.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>    /* Internet domain header */
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <arpa/inet.h>
  #include <fcntl.h>
#include "io_helpers.h"
#include "variables.h"
#include "builtins.h"
#include "commands.h"
#include "other_helpers.h"
// ====== Command execution =====

int saved_pid = 0;
/* Return: index of builtin or -1 if cmd doesn't match a builtin
 */
bn_ptr check_builtin(const char *cmd) {
    ssize_t cmd_num = 0;
    
    while (cmd_num < BUILTINS_COUNT &&
        strncmp(BUILTINS[cmd_num], cmd, MAX_STR_LEN) != 0) {
        cmd_num += 1;
    }

    return BUILTINS_FN[cmd_num];

}
// ===== Builtins =====

/* Prereq: tokens is a NULL terminated sequence of strings.
 * Return 0 on success and -1 on error ... but there are no errors on echo. 
 */
ssize_t bn_echo(char **tokens) {
    ssize_t index = 1;
    // printf("%c", tokens[index][0]);
   if (tokens[index] == NULL) { 
        display_message("\n");
        return 0;

    }  

    while (tokens[index] != NULL) {

        display_message(tokens[index]);

        if(tokens[index + 1] != NULL) {
        char *space = " ";
        display_message(space); 
        }

        index += 1;     
    }

    display_message("\n");

    return 0;
}

int pretraversal (char *filnom, int save, bool ls_flag, char *ff) {
    
    DIR *dfs;
    dfs = opendir(filnom);
     if (dfs == NULL) 
    {
        display_erro("ERROR: Invalid path");
        return -1;
    }

    while (1){
    struct dirent *aws; 
    char * d_name;
    aws = readdir(dfs);

    if (!aws || save == 0) {
        break;
    }

    d_name = aws->d_name;
    if(ls_flag){
        if(strlen(ff) <= strlen(d_name) && (strstr (d_name, ff)) != NULL){
            display_message(d_name);
            display_message("\n");
        }
    }

    if(!ls_flag){
       display_message(d_name);
       display_message("\n"); 
    }

    if (aws->d_type & DT_DIR) {   
    if (
        strcmp (d_name, ".") != 0 && strcmp (d_name, "..") != 0) {
        int p_len;
        char path[PATH_MAX];
        char path2[500];
        strcpy(path2, filnom);
        p_len = snprintf (path, PATH_MAX, "%s/%s", path2, d_name);
        if (p_len >= PATH_MAX) {
            display_erro("ERROR: Path length excceeds limit");
            return -1;
        }
        pretraversal (path, save - 1, ls_flag, ff);
    }
	}
    }
    closedir(dfs);
    return 0;
}

ssize_t bn_ls(char **tokens){
    struct dirent *aws; 
    ssize_t index = 1;
    DIR *dfs;
    if (tokens[index] == NULL)
    {
    dfs = opendir(".");
    if (dfs == NULL) 
    {
        display_erro("ERROR: Invalid path");
        return -1;
    }
    while ((aws = readdir(dfs)) != NULL){
            display_message(aws->d_name);
            display_message("\n");
    }
     closedir(dfs); 
     return 0;
    }
    bool ls_flag = false;
    bool rec_flag = false;
    bool d_flag = false;
    bool file_flag = false;
    char ff[MAX_STR_LEN];
    char file_place[500];
    int d = 0;
    while (tokens[index] != NULL) {

        if ((strcmp (tokens[index],"--f"))==0) {   
            index += 1;
            if (tokens[index]  != NULL){
                strcpy(ff, tokens[index]);
                ls_flag = true;             
            }
            else{
                ls_flag = false;
                display_erro("ERROR: provide a substring");
                return -1;
            }                  
        } 

        else if ((strcmp (tokens[index],"--rec"))==0)
        {
            if ((tokens[index + 1] != NULL) && ((strcmp (tokens[index + 1],"--d")) != 0) 
            &&  (strcmp (tokens[index],"--f"))!=0) {
             if(file_flag)
                {
                display_erro("ERROR: too many arguments");
                return -1;
                }

                index ++;
                strcpy(file_place, tokens[index]);
                rec_flag = true;
                file_flag = true;
            }
            else{
                rec_flag = true;   
            }            
        }

        else if ((strcmp (tokens[index],"--d"))== 0)
        {
            if  (tokens[index + 1] == NULL){
                display_erro("ERROR: invalid argument");
                return -1;
            }

            int num;
            index ++;
            for (int i=0; tokens[index][i] != '\0'; i++){
                if (isdigit(tokens[index][i]) != 0){
                    num ++;
                }
            }
            if(num < (strlen(tokens[index]))) {
                display_erro("ERROR: invalid argument");
                return -1;
            }
            else{
                d_flag = true;
                d=atoi(tokens[index]);
            }
        }
        
        else{
            strcpy(file_place, tokens[index]);
            if(file_flag){
                display_erro("ERROR: too many arguments");
                return -1;
            }
            file_flag = true;
        } 
        index ++;
            
    }
    if ((rec_flag && !d_flag) ||(d_flag && !rec_flag)){
        if(file_flag){
            display_erro("ERROR: Flag is missing");
            return -1;
        }
    }

    if (rec_flag && d_flag){
        if(!file_flag){
            strcpy(file_place, "."); 
        }
        
        if(pretraversal(file_place, d, ls_flag, ff) == 0){
            return 0;
        }
        else{
            return -1;
        }
    }

    ssize_t siz = strlen(ff);
    if(ls_flag) {    

    if (!file_flag){
    dfs = opendir(".");
    }
    
    if(file_flag){
        dfs = opendir(file_place); 
    }

    if (dfs == NULL) 
    {
        display_erro("ERROR: Invalid path");
        return -1;
    }

    while ((aws = readdir(dfs)) != NULL){
        
        if(siz <= strlen(aws->d_name) && (strstr (aws->d_name, ff)) != NULL){
        display_message(aws->d_name);
        display_message("\n");
        }
        
    }

    closedir(dfs);
    return 0;
    }  
    dfs = opendir(file_place);
    
    if (dfs == NULL) 
    {
        display_erro("ERROR: Invalid path");
        return -1;
    }
    while ((aws = readdir(dfs)) != NULL){
        display_message(aws->d_name);
        display_message("\n");
    } 
    closedir(dfs);
    return 0;
}


ssize_t bn_cd(char **tokens){
    ssize_t index = 1;
    if(tokens[index] == NULL){
        chdir(getenv("HOME"));
        return 0;
    }
    if (tokens[index + 1] != NULL) {
        display_erro("ERROR: too many arguments");
        return -1;
    }
    ssize_t i = 0;
    while (i < strlen(tokens[index]))
    {
        if (tokens[index][i] == '.'){         
            i ++;
        }
        else{
            i=0;
            break;
        }
    }
    if (i == 1){
        display_erro("ERROR: Invalid path");
        return -1;
    }


    if (i > 1){
        int count = 1;
        while (count < i){
        if ((chdir("..")) == -1){
            display_erro("ERROR: Invalid path");
            return -1;
            }
            
            count ++;
        }
        return 0;
    }
    if (chdir(tokens[index]) == -1){
        display_erro("ERROR: Invalid path");
        return -1;
    }
    return 0;
}

ssize_t bn_cat(char **tokens){
    ssize_t index = 1;
    int error;
    bool stdin_flag = false;
    FILE *file = NULL;
    struct pollfd fds;
    fds.fd = 0;     
    fds.events = POLLIN; 
    int ret = poll(&fds, 1, 10); 
    file = fopen(tokens[index], "rb"); 
    if (file == NULL){
        if (ret == 0) 
        {
        if (tokens[index] != NULL){
         display_erro("ERROR: Cannot open file");
         return -1;
        }
        display_erro("ERROR: No input source provided");
        // display_erro("ERROR: Cannot open file");
        return -1;
        } 
        else {
        file = stdin;
        stdin_flag=true;
        }
    }


    char var;
    char ap[2]; 
    
    while ((var=getc(file))!=EOF)

    {
    sprintf(ap, "%c", var);
    display_message(ap);
    }
    if (stdin_flag == false){
    error = fclose(file);
    if (error != 0) {
        display_erro("ERROR: failed to close");
        return -1;
    }

    }

    return 0;
}

ssize_t bn_wc(char **tokens){
    ssize_t index = 1;
    bool stdin_flag = false;
    FILE *file = NULL;
    struct pollfd fds;
    fds.fd = 0;     
    fds.events = POLLIN; 
    int ret = poll(&fds, 1, 10); 
    file = fopen(tokens[index], "rb"); 
    if (file == NULL){
        if (ret == 0) 
        {
        if (tokens[index] != NULL){
         display_erro("ERROR: Cannot open file");
         return -1;
        }
        display_erro("ERROR: No input source provided");
        // display_erro("ERROR: Cannot open file");
        return -1;
        } 
        else {
        file = stdin;
        stdin_flag=true;
        }
    }


    int error;
    int  lines = 0;
    int words = 0;
    int ch = 0;
    char var;
    char slines[MAX_STR_LEN];
    char swords[MAX_STR_LEN];
    char schars[MAX_STR_LEN];
    int space = 0;
    bool extra =false;
    while ((var=fgetc(file))!=EOF)
    {
      
      ch ++;
      
      if (isalnum(var)){
          space = 1;
          extra = false;
      }
      if (space == 1 && (var == ' ' || var == '\t' || var == '\0' || var == '\n')){

          words ++;
          space = 0;
          extra = true;
      }
      if (var == '\n' || var == '\0') { 
        
        lines ++;
        }


    }
    if (ch > 0 && !extra){
        words ++;
    }
    
    sprintf(slines, "%d\n", lines);
    sprintf(swords, "%d\n", words);
    sprintf(schars, "%d\n", ch);


    display_message("word count ");
    display_message(swords);
    
    display_message("character count ");
    display_message(schars);
   
    display_message("newline count ");
    display_message(slines);
 

    if (stdin_flag == false){
    error = fclose(file);
    if (error != 0) {
        display_erro("ERROR: failed to close");
        return -1;
    }

    }


    return 0;
}

ssize_t bn_kill(char **tokens)
{
 ssize_t index = 1;
 if (tokens[index] == NULL) { 
        display_erro("ERROR: The process does not exist");
        return -1;
    }

    int val = (int)strtol(tokens[index], NULL, 10);
  
    index += 1;
        if (tokens[index] == NULL){
            
            if(kill(val, SIGTERM)==-1){
                display_erro("ERROR: The process does not exist");
                return -1;
            }
            return 0;
        }
        else{
            int v1 = (int)strtol(tokens[index], NULL, 10);
            if(v1 < 1 || v1 > 31) 
            {   
                display_erro("ERROR: Invalid signal specified");
                return -1;
            }
            if (kill(val, v1)==-1){
                display_erro("ERROR: The process does not exist");
                return -1;
            }
            
            return EXIT_SUCCESS;
        

        
    }

return 0;
}



ssize_t bn_ps()
{
displayps();
return 0;
}



ssize_t bn_startserver(char **tokens)
{

    if (tokens[1] == NULL) 
    {
        display_erro("ERROR: No port provided");
        return -1;
    }

    if (saved_pid > 0) {
        display_erro("a server is already in process");
        return -1;

    }
    // // client socket
    int child = fork();
    if (child < 0){
        display_erro("failed child");
        return -1;
    }

    if (child == 0)
    {

    int port = (int) strtol(tokens[1], NULL,10); 
    struct listen_sock s;
    int set = setup_server_socket(&s, port);
    if (set < 0)
    {
        return -1;
    }
    int max_fd = s.sock_fd;

    //file descriptor set 
    fd_set all_fds, listen_fds;
    
    FD_ZERO(&all_fds);
    FD_SET(s.sock_fd, &all_fds);
    int client_fd = 0;
    int data[64];
    ssize_t ind = 0;
    do {

        listen_fds = all_fds;
        select(max_fd + 1, &listen_fds, NULL, NULL, NULL);


          //accept connection from clients 
          if (FD_ISSET(s.sock_fd, &listen_fds)) {
            client_fd = accept_connection(s.sock_fd);
            
            if (client_fd < 0) {
                return -1;
                
            }
            data[ind] = client_fd;
            ind ++;

            if (client_fd > max_fd) {
                max_fd = client_fd;
  
            }
            FD_SET(client_fd, &all_fds);
          }
            for(int i = 0;i<ind; i++){
                if (data[i] > 0){

                if(FD_ISSET(data[i], &listen_fds))
                {   
                char buff[64];
                bzero(buff, 64);
                // if (recv(fd, buffer, sizeof(buffer), MSG_PEEK | MSG_DONTWAIT) == 0) {
                int ret = read(data[i], buff, sizeof(buff));
                    if (ret == 0 || ret < 0)
                    {
                        close(data[i]);
                        FD_CLR(data[i], &listen_fds);
                        data[i] = -1;
                    }
                    if (ret > 0)
                    {
                    display_message(buff);
                    
                    }
                }

                }
                }   
            
            
         
           
    } while(1);
    free(s.addr);
    close(s.sock_fd);
    exit(0);
    }
    else 
    {
        saved_pid = child;

    }

    
    
return 0;
}


ssize_t bn_closeserver()
{

    
    if (saved_pid > 0)
    {
        kill(saved_pid, 9);
        saved_pid = 0;
        return 0;
    }
    
else{
    display_erro("ERROR: No Server to close");
    return -1;
}
}
ssize_t bn_send(char **tokens)
{
if (tokens[1] == NULL) 
    {
        display_erro("ERROR: No port provided");
        return -1;
    }
    if (tokens[2] == NULL) 
    {
        display_erro("ERROR: No hostname provided");
        return -1;
    }
    if (tokens[3] == NULL)
    {
        display_erro("ERROR: No tokens provided");
        return -1;
    }


struct server_sock s;
int port = (int) strtol(tokens[1], NULL,10); 
int client_s = setup_client_socket(&s, tokens[2], port);
if(client_s < 0)
{
return -1;
}

int inde = 3;
char strstore[65];
strcpy(strstore, "mysh$ ");
strcat(strstore, "\n");
strcat(strstore, tokens[inde]);
inde ++;

while (tokens[inde] != NULL)
{
    strcat(strstore, " ");
    strcat(strstore, tokens[inde]);
    inde ++;
}
strcat(strstore, "\n");
write(client_s, strstore, 65);


close(client_s);
free(s.addr);
return 0;
}


int client_soc = 0;

ssize_t bn_startclient(char **tokens)

{
if (tokens[1] == NULL) 
    {
        display_erro("ERROR: No port provided");
        return -1;
    }
  if (tokens[2] == NULL) 
    {
        display_erro("ERROR: No hostname provided");
        return -1;
    }


struct server_sock s;
int port = (int) strtol(tokens[1], NULL,10); 
client_soc = setup_client_socket(&s, tokens[2], port);
if(client_soc < 0)
{
return -1;
}

while (!feof(stdin)) 

    {
    char saveas[250]; 
    fgets(saveas, sizeof(saveas), stdin);
    write(client_soc, saveas, 64);
    }

close(client_soc);
free(s.addr);
return 0;
}