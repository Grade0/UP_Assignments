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
#include <time.h>
#include <math.h>

#define PORTNUM "49999"    /* Port number for server */
#define BACKLOG 10
#define BUFSIZE 32

#define MAX_VAL 1000
#define MIN_VAL 1

//Generate 2 dimension random double matrix
double** randomMatrix(int dim); 
void printM(double** M, int row, int col);
void splitM(double** A, double** B, int init, int row, int col);

int main(int argc, char* argv[]) {

	int n, p, r, count;
	int index = 0;
	printf("Insert the value of n...\n");
	scanf("%d", &n);
	puts("");
	printf("Insert the value of p...\n");
	scanf("%d", &p);
	puts("");
	r = n/p;
	double** A = randomMatrix(n);
	double** B = randomMatrix(n);
	double** C = malloc(n * sizeof(double*));
	printf("The matrix A:\n");
	printM(A, n, n);
	printf("The matrix B:\n");
	printM(B, n, n);

	struct addrinfo hints;
    struct addrinfo *result, *rp;



    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_canonname = NULL; 
    hints.ai_addr = NULL;
    hints.ai_next = NULL; 
    hints.ai_family = AF_UNSPEC; 
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV;


    for(int k = 0; k < n; k += r) {

    	index++;

    	if (getaddrinfo(argv[1], PORTNUM, &hints, &result) != 0)
       		exit(-1);

    	int cfd;
    	for (rp = result; rp != NULL; rp = rp->ai_next) {
        	 cfd = socket(rp->ai_family, rp->ai_socktype, 
                      rp->ai_protocol);

         	if (cfd == -1)
            	continue;   /* On error, try next address */

         	if (connect(cfd, rp->ai_addr, rp->ai_addrlen) != -1)
            	break; /* Success */

         	/* close() failed, close this socket, try next address */
         	close(cfd);
    	}

    	if (rp == NULL)
       		exit(-1);

    	freeaddrinfo(result);

    	{	
        	int r1 = htonl(r);
    		ssize_t flag = write(cfd, &r1, sizeof(r));
    		if (flag <= 0) { 
    			fprintf(stderr, "Write error.\n");
            	exit(EXIT_FAILURE);
    		}

    		int n1 = htonl(n);
    		flag = write(cfd, &n1, sizeof(r));
    		if (flag <= 0) { 
    			fprintf(stderr, "Write error.\n");
            	exit(EXIT_FAILURE);
    		}

    		double** sliceM = malloc(r * sizeof(double*));
    		for(int i = 0; i < r; i++) {
    			sliceM[i] = malloc(n * sizeof(double));
    		}

    		count = 1;
    		splitM(A, sliceM, k, r, n);
    		printf("Sending partition number %d of Matrix A to localhost:%s...\n", index, PORTNUM);
 			puts("");

    		char bufw[BUFSIZE];
        	for (int i = 0; i < r; i++) {
        		for (int j = 0 ; j < n; j++) {
        			sprintf(bufw, "%.2f", sliceM[i][j]);

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

        	for(int i = 0; i < n; i++) {
        		for(int j = 0; j < n; j++) {
        			sprintf(bufw, "%.2f", B[i][j]);

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
        	printf("Sending Matrix B...\n");
        	puts("");


        	char bufr[BUFSIZE];
        	for(int i = k; i < k+r; i++) {
          		C[i] = malloc(n * sizeof(double));
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
              	C[i][j] = atof(bufr);
          	}
        	}
        	printf("Received partition number %d of Matrix C.\n", index);
        	puts("-----------------");
        	puts("");

    	}

    	if (close(cfd) == -1) /* Close connection */
    	{
        	fprintf(stderr, "close error.\n");
        	exit(EXIT_FAILURE);
    	}
	}

	printf("The result of Matrix C is:\n");
	printM(C, n, n);

    exit(EXIT_SUCCESS);
}

 
double** randomMatrix(int dim) {
    /* Generate 2 dimension random double matrix */

    double** matrix = malloc(dim * sizeof(double*));

    for(int i=0; i<dim; i++) {
        matrix[i] = malloc(dim* sizeof(double));
    }
    
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
            matrix[i][j] = 2;
        } 
    }

    return matrix;
}

void printM(double** M, int row, int col) {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            printf("%.2f   ", M[i][j]);
        } 
        puts("");
    }
    puts("");
}

void splitM(double** A, double** B, int init, int row, int col) {
    for(int i = init, t = 0; i < init+row, t < row; i++, t++) {
        for(int j = 0; j < col; j++) {
            B[t][j] = A[i][j];
        }
    }
}