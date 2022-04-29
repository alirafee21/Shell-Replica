#ifndef __OTHER_HELPERS_H__
#define __OTHER_HELPERS_H__

#ifndef MAX_BACKLOG
    #define MAX_BACKLOG 100
#endif

struct listen_sock {
    struct sockaddr_in *addr;
    int sock_fd;
};

struct server_sock {
    int sock_fd;
    struct sockaddr_in *addr;
    int inbuf;
};

int setup_server_socket(struct listen_sock *s, int port);

int accept_connection(int start_socket);
int setup_client_socket(struct server_sock *s, char *host, int port);
void recieve(int fd); 
#endif