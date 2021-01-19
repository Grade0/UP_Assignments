#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <semaphore.h>
#include <syncbuffer.h> 
#include <unistd.h>
#include <getopt.h>

typedef struct threadArgs {
	int th_id;
	B_Sync *b;
} threadArgs_t;

void* Producer(void* arg) {
	//riceve il buffer
	B_Sync* b = ((threadArgs_t*)arg)->b;
	//assegnazione id al thread
	int myid = ((threadArgs_t*)arg)->th_id;


	//si genera sul valore random int (range 1-100) da inserire nel buffer
	int data = 1 + rand() % 100;
	void* item = (int*) &data;
	
	//uso del metodo bloccante put
	if(put(b, item, myid) == -1) {
		fprintf(stderr, "Error: put\n");
		pthread_exit(NULL);
	}

	//si cicla sul metodo non bloccante tryput finche' non restituisce esito positivo
	/*while(tryput(b, item, myid) == -1 && errno == EAGAIN) {
		fprintf(stderr, "Producer %d: tryput fallito\n", myid);
	}*/
	printf("Producer %d: exits\n", myid);

	pthread_exit(NULL);

}

void* Consumer(void* arg) {
	//riceve il buffer
	B_Sync* b = ((threadArgs_t*)arg)->b;
	//assegnazione id al thread
	int myid = ((threadArgs_t*)arg)->th_id;

	void* item = NULL;
	//uso del metodo bloccante get
	if(get(b, &item) == -1) {
		fprintf(stderr, "Error: get\n");
		pthread_exit(NULL);
	}

	//si cicla sul metodo non bloccante tryget finche' non restituisce esito positivo
	/*while(tryget(b, &item) == -1) {
		fprintf(stderr, "Consumer %d: tryget fallito\n", myid);
	}*/
	printf("Consumer %d: estratto %d\n", myid, *(int*)item);

	printf("Consumer %d: exits\n", myid);
	pthread_exit(NULL);
}


//stampa guida all'esecuzione del programma nel caso di dati di input errato
void usage(char *pname) {
    fprintf(stderr, "\nEseguire il programma nel seguente formato: \n\n%s -p <num-producers> -c <num-consumers>\n\n", pname);
    exit(EXIT_FAILURE);
}


int main(int argc, char*argv[]) {
	extern char* optarg;
	int p = 0, c = 0, opt;

	while((opt = getopt(argc, argv, "p:c:")) != -1) {
		switch(opt) {
			case 'p':
				p=atoi(optarg);
				break;
			case 'c':
				c=atoi(optarg);
				break;
			default:
				usage(argv[0]);
				break;
		}
	}

	if(p==0 || c==0) usage(argv[0]);

	printf("num Producers = %d, num Consumer = %d\n", p, c);

	pthread_t *th;
	threadArgs_t *thARGS;

	th     = malloc((p+c)*sizeof(pthread_t));
    thARGS = malloc((p+c)*sizeof(threadArgs_t));

    if (!th || !thARGS) {
		fprintf(stderr, "malloc fallita\n");
		exit(EXIT_FAILURE);
    }

    //creazione del buffer sincrono
    B_Sync* b = init_buffer();

    if(!b) {
    	fprintf(stderr, "init_buffer fallita\n");
		exit(errno);
    }

    for(int i=0; i<p; i++) {
    	thARGS[i].th_id = i;
    	thARGS[i].b = b;

    }

    for(int i=p;i<(p+c); ++i) {	
	thARGS[i].th_id = i-p;
	thARGS[i].b = b;
    }

    for(int i=0;i<p; ++i)
	if (pthread_create(&th[i], NULL, Producer, &thARGS[i]) != 0) {
	    fprintf(stderr, "pthread_create failed (Producer)\n");
	    exit(EXIT_FAILURE);
	}

    for(int i=0; i<c; i++)
	if (pthread_create(&th[p+i], NULL, Consumer, &thARGS[p+i]) != 0) {
	    fprintf(stderr, "pthread_create failed (Consumer)\n");
	    exit(EXIT_FAILURE);
	}
    
    // aspetto prima tutti i produttori
    for(int i=0;i<p; ++i)
	pthread_join(th[i], NULL);

	// aspetto la terminazione di tutti i consumatori
    for(int i=0;i<c; ++i)
	pthread_join(th[p+i], NULL);

	// libero memoria
    deleteBuffer(b);
    free(th);
    free(thARGS);
    
    return 0;   
}