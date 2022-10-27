//Used linux man pages and beej's guide to help understand and write tunnel.c
#include <netinet/in.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>

#define MAXLINE     4096    /* max text line length */
#define LISTENQ     1024

struct message{
int addrlen, timelen, msglen;
char addr[MAXLINE];
char currtime[MAXLINE];
char payload[MAXLINE];
};


struct tunnel_message{
char server_ip_address[MAXLINE];
char server_port_num[MAXLINE];
};


static int tunnel_port_num;
struct message tun_message;

void* SendToX(struct tunnel_message tun);

void* RecFrom() {
    //argv[1] is port num
    int     listenfd, connfd, n;
    struct sockaddr_in servaddr, client_addr;
    char tun_ip[MAXLINE];
    struct tunnel_message tun;
    strcpy(tun_message.payload, "");
    FILE *fp;
    char path[MAXLINE];
    socklen_t client_addrlen_size;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(tunnel_port_num); /* daytime server */

    bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    listen(listenfd, LISTENQ);

    tun_message.msglen = strlen(tun_message.payload);
    
        fp = popen("hostname -I", "r");
        //fp = popen("echo '206.12.16.199 10.17.1.1';echo 'yo 1 2 3 \nyo 1 2 3\nyo 1 2 3'", "r");

        if (fp == NULL)
            printf("[ERROR]Server does not have a hostname");

        if (fgets(path, MAXLINE, fp) != NULL)
            strcpy(tun_ip,strtok(path," "));

        pclose(fp);

    client_addrlen_size= sizeof(client_addr);
    for ( ; ; ) {
        connfd = accept(listenfd, (struct sockaddr *)&client_addr, &client_addrlen_size);
        strcpy(tun_message.payload, "");
        strcpy(tun_message.currtime, "");
        strcpy(tun_message.addr, "");

        n = recv(connfd, (void*)&tun, sizeof(tun),MSG_WAITALL);

        if (n < 0) {
            printf("read error\n");
            exit(1);
        }

        printf("Server IP: %s\n", tun.server_ip_address);
        printf("Server Port: %s\n", tun.server_port_num);

        SendToX(tun);
        strcpy(tun_message.payload,tun_ip);

        write(connfd, &tun_message, sizeof(tun_message));

        close(connfd);
    }
}


void* SendToX(struct tunnel_message tun) {

    //argv[1] is ip address
    //argv[2] is port number

    struct addrinfo hints;
    struct addrinfo *result, *res;
    int     n, s, sockfd;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;    
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;         

    s = getaddrinfo(tun.server_ip_address, tun.server_port_num, &hints, &result);
    if (s != 0) {
       fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
       exit(EXIT_FAILURE);
    }


    for(res = result; res != NULL; res = res->ai_next) {
            if ((sockfd = socket(res->ai_family, res->ai_socktype,
                    res->ai_protocol)) == -1) {
                perror("client: socket");
                continue;
            }

            if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
                close(sockfd);
                perror("client: connect");
                continue;
            }

            break;
        }

    
    
    if (res == NULL) {              
        fprintf(stderr, "Could not connect\n");
        exit(EXIT_FAILURE);
    }


    freeaddrinfo(result);

    n = recv(sockfd, (void*)&tun_message, sizeof(tun_message),MSG_WAITALL);

    if (n < 0) {
        printf("read error\n");
        exit(1);
    }

    return 0;
}


int
main(int argc, char **argv)
{

    if (argc != 2) {
        printf("usage: tunnel <port_num>\n");
        exit(1);
    }   

    tunnel_port_num = atoi(argv[1]);


    RecFrom();

    exit(0);
}

