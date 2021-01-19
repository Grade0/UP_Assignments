#include <errno.h>
#include <pthread.h>
#include <syncbuffer.h>
#include <stdlib.h>
#include <stdio.h>


/**
 * @file boundedqueue.c
 * @brief File di implementazione dell'interfaccia per la coda di dimensione finita
 */


/* ------------------- funzioni di utilita' -------------------- */

static inline void LockBuffer(B_Sync *b)          { pthread_mutex_lock(&b->mutex);   }
static inline void UnlockBuffer(B_Sync *b)        { pthread_mutex_unlock(&b->mutex); }
static inline void WaitToProduce(B_Sync *b)      { pthread_cond_wait(&b->bfully, &b->mutex); }
static inline void WaitToConsume(B_Sync *b)      { pthread_cond_wait(&b->bempty, &b->mutex); }
static inline void SignalProducer(B_Sync *b)     { pthread_cond_signal(&b->bfully); }
static inline void SignalConsumer(B_Sync *b)     { pthread_cond_signal(&b->bempty); }
static inline void V_Prod(B_Sync *b)             { sem_post(&b->prod); }
static inline void P_Prod(B_Sync *b)             { sem_wait(&b->prod); }
static inline void V_Cons(B_Sync *b)             { sem_post(&b->cons); }
static inline void P_Cons(B_Sync *b)             { sem_wait(&b->cons); }
static inline int b_isEmpty(B_Sync *b)           { return b->buf == NULL; }


/* ------------------- interfaccia della coda ------------------ */

B_Sync* init_buffer() {
	B_Sync* b = (B_Sync*) malloc(sizeof(B_Sync));

	b -> buf = NULL;

	if(pthread_mutex_init(&b->mutex, NULL) != 0) {
		perror("pthread_mutex_init");
		goto error;
	}

	if (pthread_cond_init(&b->bfully, NULL) != 0) {
		perror("pthread_cond_init full");
		goto error;
    }

    if (pthread_cond_init(&b->bempty, NULL) != 0) {
		perror("pthread_cond_init empty");
		goto error;
    }

    if(sem_init(&b->prod, 0, 0) != 0) {
    	perror("sem_init prod");
    	goto error;
    }

    if(sem_init(&b->cons, 0, 0) != 0) {
    	perror("sem_init cons");
    	goto error;
    }

    return b;

 error:
 	if (!b) return NULL; 
    int myerrno = errno;
    if (b->buf) free(b->buf);
    if (&b->mutex) pthread_mutex_destroy(&b->mutex);
    if (&b->bfully) pthread_cond_destroy(&b->bfully);
    if (&b->bempty) pthread_cond_destroy(&b->bempty);
    if (&b->prod) sem_destroy(&b->prod);
    if (&b->cons) sem_destroy(&b->cons);
    free(b);
    errno = myerrno;
    return NULL;
}

void deleteBuffer(B_Sync* b) {
    if (!b) {
	errno = EINVAL;
	return;
    }   
    
    if (b->buf) free(b->buf);
    if (&b->mutex) pthread_mutex_destroy(&b->mutex);
    if (&b->bfully) pthread_cond_destroy(&b->bfully);
    if (&b->bempty) pthread_cond_destroy(&b->bempty);
    if (&b->prod) sem_destroy(&b->prod);
    if (&b->cons) sem_destroy(&b->cons);
    free(b);
}


int put(B_Sync* b, void* item, int idTh) {
	if(!b || !item) {
		errno = EINVAL;
		return -1;
	}

	LockBuffer(b);
	while(!b_isEmpty(b)) {
		WaitToProduce(b);
	}
	b->buf = item;
	printf("Producer %d: %d inserito nel buffer\n", idTh, *(int*)(b->buf));
	V_Prod(b);

	SignalConsumer(b);
	UnlockBuffer(b);
	
	P_Cons(b);

	return 0;
}


int get(B_Sync* b, void** item) {
	if(!b) {
		errno = EINVAL;
		return -1;
	}

	LockBuffer(b);
	while(b_isEmpty(b)) {
		WaitToConsume(b);
	}
	P_Prod(b);

	*item = b->buf;

	b->buf = NULL;

	V_Cons(b);
	SignalProducer(b);
	UnlockBuffer(b);

	return 0;
}



int tryput(B_Sync* b, void* item, int idTh) {
	if(!b || !item) {
		errno = EINVAL;
		return -1;
	}

	int esito = -1;

	LockBuffer(b);
	
	if(b_isEmpty(b)) {
		
		b->buf = item;

		esito = 0;
		printf("Producer %d: %d inserito nel buffer\n", idTh, *(int*)(b->buf));
		V_Prod(b);

	} else {
		errno = EAGAIN;
	}

	SignalConsumer(b);
	UnlockBuffer(b);
	
	if(esito == 0) {
		P_Cons(b);
	}

	return esito;
}


int tryget(B_Sync* b, void** item) {
	int esito = -1;

	LockBuffer(b);
	
	if(!b_isEmpty(b)) {

		P_Prod(b);
		*item = b->buf;
		b->buf = NULL;
		esito = 0;

		V_Cons(b);		
	} else {
		errno = EAGAIN;
	}

	SignalProducer(b);
	UnlockBuffer(b);
	
	return esito;
}