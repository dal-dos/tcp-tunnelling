//Used linux man pages and beej's guide to help understand and write client.c
#include <netinet/in.h>
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
#define DAYTIME_PORT 3333

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

int
main(int argc, char **argv)
{
    //argv[1] is ip address
    //argv[2] is port number
    struct tunnel_message tun;
    struct addrinfo hints;
    struct addrinfo *result, *res;
    int     n, s, sockfd;
    struct message m;
    char hbuf[1025], sbuf[32];
    struct sockaddr_in serveraddr;
    if (argc != 3) {
        if(argc != 5){
            printf("usage: client <server_ip_address> <server_port_num> OR\nusage: client <tunnel_ip_address> <tunnel_port_num> <server_ip_address> <server_port_num>\n");
            exit(1);
        }
    }



    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;    
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;         

    s = getaddrinfo(argv[1], argv[2], &hints, &result);
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

    if(argc != 3){
        strcpy(tun.server_ip_address,argv[3]);
        strcpy(tun.server_port_num,argv[4]);
        write(sockfd, &tun, sizeof(tun));


        while ( (n = recv(sockfd, (void*)&m, sizeof(m),MSG_WAITALL)) > 0) {

        serveraddr.sin_port = atoi(tun.server_port_num);
        serveraddr.sin_addr.s_addr = inet_addr(m.addr);
        serveraddr.sin_family = AF_INET;
        getnameinfo((const struct sockaddr * restrict)&serveraddr,sizeof(serveraddr),hbuf,sizeof(hbuf),sbuf,sizeof(sbuf),0);
        printf("Server Name: %s\n", hbuf);
        printf("IP address: %s\n", m.addr);
        printf("Time: %s\n", m.currtime);

        serveraddr.sin_port = atoi(argv[2]);
        serveraddr.sin_addr.s_addr = inet_addr(m.payload);
        serveraddr.sin_family = AF_INET;
        getnameinfo((const struct sockaddr * restrict)&serveraddr,sizeof(serveraddr),hbuf,sizeof(hbuf),sbuf,sizeof(sbuf),0);
        printf("Via Tunnel: %s\n", hbuf);
        printf("IP address: %s\n", m.payload);
        printf("Port Number: %s\n", argv[2]);

        }

        if (n < 0) {
            printf("read error\n");
            exit(1);
        }

    }
    
    while ( (n = recv(sockfd, (void*)&m, sizeof(m),MSG_WAITALL)) > 0) {
        serveraddr.sin_port = atoi(argv[2]);
        serveraddr.sin_addr.s_addr = inet_addr(m.addr);
        serveraddr.sin_family = AF_INET;
        getnameinfo((const struct sockaddr * restrict)&serveraddr,sizeof(serveraddr),hbuf,sizeof(hbuf),sbuf,sizeof(sbuf),0);
        printf("Server Name: %s\n", hbuf);
        printf("IP address: %s\n", m.addr);
        printf("Time: %s", m.currtime);
        printf("Who: %s", m.payload);
    }

    if (n < 0) {
        printf("read error\n");
        exit(1);
    }

    exit(0);
}

