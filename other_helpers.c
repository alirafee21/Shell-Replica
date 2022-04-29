#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>     /* inet_ntoa */
#include <netdb.h>         /* gethostname */
#include <netinet/in.h>  


#include "other_helpers.h"
#include "io_helpers.h"

int setup_server_socket(struct listen_sock *s, int port) {
    //malloc for socket address
    if(!(s->addr = malloc(sizeof(struct sockaddr_in)))) {
        perror("malloc");
       return -1;
    }



    // create socket file descriptor
    s->sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    //initialize server address
    s->addr->sin_family = AF_INET;
    s->addr->sin_port = htons(port);
    memset(&(s->addr->sin_zero), 0, 8);
    s->addr->sin_addr.s_addr = INADDR_ANY;

    if (s->sock_fd < 0) {
        display_erro("ERROR: server socket");
        return -1;
    }

    //reuse port after server terminate and allows socket for multiple connections
    int on = 1;
    int status = setsockopt(s->sock_fd, SOL_SOCKET, SO_REUSEADDR,
        (const char *) &on, sizeof(on));
    
    if (status < 0) {
        display_erro("ERROR: setsockopt");
        return -1;
    }


    // Bind the selected port to the socket. “assigning a name to a socket”
    if (bind(s->sock_fd, (struct sockaddr *)s->addr, sizeof(*(s->addr))) < 0) {
        display_erro("ERROR: server: bind");
        close(s->sock_fd);
        return -1;
    }

    // setup queue to hold pending connections on this socket.
    if (listen(s->sock_fd, MAX_BACKLOG) < 0) {
        display_erro("ERROR: server: listen");
        close(s->sock_fd);
        return -1;
    }
return s->sock_fd;

}

int accept_connection(int start_socket)
{

    struct sockaddr_in client_addr;
    client_addr.sin_family = AF_INET;
    unsigned int client_len = sizeof(client_addr);

    //  Wait for and accept a new connection.
    int client_socket = accept(start_socket, (struct sockaddr *)&client_addr, &client_len);
    if (client_socket == -1) 

    {
        display_erro("ERROR: accept");
        return -1;
    } 

    return client_socket;
}

int setup_client_socket(struct server_sock *s, char *host, int port)
{
  //malloc for socket address
    if(!(s->addr = malloc(sizeof(struct sockaddr_in)))) {
        perror("malloc");
       return -1;
    }


    struct hostent *hos;
    // create socket file descriptor
    s->sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    //initialize server address
    s->addr->sin_family = AF_INET;
    s->addr->sin_port = htons(port);
    
    memset(&(s->addr->sin_zero), 0, 8);
    // struct addrinfo *ai;
   // getaddrinfo(host, NULL, NULL, &ai);
     
    // s->addr->sin_addr  = ((struct sockaddr_in *) ai->ai_addr)->sin_addr;
    // freeaddrinfo(ai);

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);


    if (!(hos = gethostbyname (host))) {
        display_erro("ERROR: invalid host");
        return -1;
    }

    memcpy (&server.sin_addr, hos->h_addr, sizeof (server.sin_addr));


    if (s->sock_fd < 0) {
        display_erro("ERROR: server socket");
        return -1;
    }
    //connect sock_fd to address and port specified in addr
    if (connect(s->sock_fd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        display_erro("ERROR: Failed to connect");
        return -1;
    }
    return s->sock_fd;
}

void recieve(int fd) 
{
    char buff[64];
    bzero(buff, 64);
    if (recv(fd, buff, sizeof(buff), 0) > 0)
    {
    display_message(buff);
    display_message("mysh$ ");
    }
 
}


