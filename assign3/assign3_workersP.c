/*
name: Chen Davide
student n.: 19207819
email: davide.chen@ucdconnect.ie
*/

#define _DEFAULT_SOURCE /* For NI_MAXHOST and NI_MAXSERV */
#include <netdb.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PORTNUM "49999"    /* Port number for server */
#define BACKLOG 10
#define BUFSIZE 32


static void
reapChild(int sig)
{
    int savedErrno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0)
        continue;
    errno = savedErrno;
}

static void
handleRequest(int cfd)
{
    int r, n;
        ssize_t flag = read(cfd, &r, sizeof(r));
        if(flag == -1) {
          fprintf(stderr, "Read error.\n");
        }
        r = ntohl(r);
        printf("Received value of r: %d\n", r);

        flag = read(cfd, &n, sizeof(n));
        if(flag == -1) {
          fprintf(stderr, "Read error.\n");
        }
        n = ntohl(n);
        printf("Received value of n: %d\n", n);
        puts("");

        
        double** sliceM = malloc(r * sizeof(double*));
        double** B = malloc(n * sizeof(double*));
        double** C = malloc(n* sizeof(double*));

        char bufr[BUFSIZE];
        for(int i = 0; i < r; i++) {
          sliceM[i] = malloc(n * sizeof(double));
          for (int j = 0; j < n; j++) {
              ssize_t numRead = read(cfd, bufr, sizeof(bufr));
              if (numRead == 0)
                break;
              if (numRead == -1 || numRead == 0) {
                 if (errno == EINTR)
                    continue;
                else {
                    fprintf(stderr, "Read error.\n");
                }
              }
              sliceM[i][j] = atof(bufr);
          }
        }
        printf("Received a partition of Matrix A.\n");
        puts("");

        for(int i = 0; i < n; i++) {
          B[i] = malloc(n * sizeof(double));
          for (int j = 0; j < n; j++) {
              ssize_t numRead = read(cfd, bufr, sizeof(bufr));
              if (numRead == 0)
                break;
              if (numRead == -1 || numRead == 0) {
                 if (errno == EINTR)
                    continue;
                else {
                    fprintf(stderr, "Read error.\n");
                }
              }
              B[i][j] = atof(bufr);
          }
        }
        printf("Received matrix B.\n");
        puts("");

        int sum = 0;
        for(int i = 0; i < r; i++) {
          C[i] = malloc(n * sizeof(double));
          for(int j = 0; j < n; j++) {
            for(int t = 0; t < n; t++) {
              sum = sum + sliceM[i][t] * B[t][j];
            }
            C[i][j] = sum;
            sum = 0;
          }
        }

        char bufw[BUFSIZE];
        for (int i = 0; i < r; i++) {
          for (int j = 0 ; j < n; j++) {
            sprintf(bufw, "%.2f", C[i][j]);

              ssize_t numWritten = write(cfd, bufw, sizeof(bufw));
              if (numWritten <= 0) {
                  if (numWritten == -1 && errno == EINTR)
                      continue;
                  else {
                      fprintf(stderr, "Write error.\n");
                      exit(EXIT_FAILURE);
                  }
              }
          }
        }
        printf("Sending the partition of Matrix C.\n");
        puts("-----------------");
        puts("");
}

int main(int argc, char *argv[]) 
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = reapChild;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
       exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_canonname = NULL; 
    hints.ai_addr = NULL;
    hints.ai_next = NULL; 
    hints.ai_family = AF_UNSPEC; 
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;

    if (getaddrinfo(NULL, PORTNUM, &hints, &result) != 0)
       exit(-1);

    int lfd, optval = 1;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
         lfd = socket(rp->ai_family, rp->ai_socktype, 
                      rp->ai_protocol);

         if (lfd == -1)
            continue;   /* On error, try next address */

         if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, 
                        &optval, sizeof(optval)) == -1)
            exit(-1);

         if (bind(lfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break; /* Success */

         /* bind() failed, close this socket, try next address */
         close(lfd);
    }

    if (rp == NULL)
       exit(-1);

    {
       char host[NI_MAXHOST];
       char service[NI_MAXSERV];
       if (getnameinfo((struct sockaddr *)rp->ai_addr, rp->ai_addrlen,
                 host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
          fprintf(stdout, "Listening on (%s, %s)\n", host, service);
       /*else
          exit(-1);*/
    }

    freeaddrinfo(result);

    if (listen(lfd, BACKLOG) == -1)
       exit(-1);

    for (;;)
    {
        int cfd = accept(lfd, NULL, NULL);
        if (cfd == -1) {
           continue;   /* Print an error message */
        }

        switch (fork()) {
        case -1:
           close(cfd);
           break;
        case 0: /* Child */
           close(lfd); /* Unneeded copy of listening socket */
           handleRequest(cfd);
           _exit(EXIT_SUCCESS);
        default: /* Parent */
           close(cfd); /* Unneeded copy of connected socket */
           break; /* Loop to accept next connection */
        }
    }

    exit(EXIT_SUCCESS);
}