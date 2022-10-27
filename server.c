//Used linux man pages and beej's guide to help understand and write server.c
#include <netinet/in.h>
#include <time.h>
#include <strings.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAXLINE     4096    /* max text line length */
#define LISTENQ     1024    /* 2nd argument to listen() */
#define DAYTIME_PORT 3333

char server_port_num[MAXLINE];

struct message{
int addrlen, timelen, msglen;
char addr[MAXLINE];
char currtime[MAXLINE];
char payload[1000];
};



int
main(int argc, char **argv)
{
    //argv[1] is port num
    int     listenfd, connfd;
    struct sockaddr_in servaddr, client_addr;
    struct message m;
    strcpy(m.payload, "");
    FILE *fp;
    char path[MAXLINE];
    time_t ticks;
    socklen_t client_addrlen_size;


    char hbuf[1025], sbuf[20];
    if (argc != 2) {
        printf("usage: server <port_num>\n");
        exit(1);
    }
    strcpy(server_port_num,argv[1]);
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(argv[1])); /* daytime server */

    bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    listen(listenfd, LISTENQ);


    



    client_addrlen_size= sizeof(client_addr);
    for ( ; ; ) {
        connfd = accept(listenfd, (struct sockaddr *)&client_addr, &client_addrlen_size);
        strcpy(m.payload, "");
        fp = popen("hostname -I;who", "r");
        //fp = popen("echo '206.12.16.199 10.17.1.1';echo 'yo 1 2 3 \nyo 1 2 3\nyo 1 2 3'", "r");

        if (fp == NULL)
            printf("[ERROR]Server does not have a hostname and who doesnt exist");

        if (fgets(path, MAXLINE, fp) != NULL)
            strcpy(m.addr,strtok(path," "));

        while (fgets(path, MAXLINE, fp) != NULL){
            strcat(strcat(m.payload, path), "     ");
        }
        pclose(fp);

        ticks = time(NULL);
        strcpy(m.currtime, ctime(&ticks));
        m.addrlen = strlen(m.addr);
        m.msglen = strlen(m.payload);
        m.msglen = m.addrlen + m. timelen + strlen(m.payload);



        write(connfd, &m, sizeof(m));
        getnameinfo((const struct sockaddr * restrict)&client_addr,sizeof(client_addr),hbuf,sizeof(hbuf),sbuf,sizeof(sbuf),0);
        printf("Client Name: %s\n", hbuf);
        printf("IP Address: %s\n", inet_ntoa(client_addr.sin_addr));

        close(connfd);
    }
}

