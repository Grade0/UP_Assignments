#include <pthread.h>
#include <semaphore.h>

/**
 * Struttura dati buffer
 */
typedef struct buffer {
	void* buf;
	pthread_mutex_t mutex;
	pthread_cond_t bfully;
	pthread_cond_t bempty;
	sem_t cons;
	sem_t prod;
} B_Sync;


/**
 * Alloca ed inizializza un buffer di dimensione 1
 * Dev'essere chiamata dal thread main
 */
B_Sync* init_buffer();

/** 
 * Cancella un buffer allocata con init_buffer. 
 * Dev' essere chiamata dal thread main
 *  
 *   \param b puntatore al buffer da cancellare
 */
void deleteBuffer(B_Sync *b);

/**
 * Metodo bloccante che inserisce un dato nel buffer
 *
 *   \param item puntatore al dato da inserire
 *   \param item puntatore a cui restituire il dato
 *   \param idTh l'id del thread chiamante
 *	
 *	 \retval 0 se successo
 *	 \retval -1 se errore (errno settato opportunatamente)
 */
int put(B_Sync* b, void* item, int idTh);

/**
 * Metodo bloccante che estrare un dato dal buffer
 *   
 *   \param b il buffer sincrono su cui fare put
 *	
 *	 \retval 0 se successo
 *	 \retval -1 se errore (errno settato opportunatamente)
 */
int get(B_Sync* b, void** item);

/**
 * Inserisce un dato nel buffer se questo e' vuoto
 *
 *   \param item puntatore al dato da inserire
 *   \param item puntatore a cui restituire il dato
 *   \param idTh l'id del thread chiamante
 *	
 *	 \retval 0 se successo
 *	 \retval -1 se errore (errno settato EAGAIN)
 */
int tryput(B_Sync* b, void* item, int idTh);

/**
 * Estrare un dato dal buffer se questo e' pieno
 *
 *   \param item puntatore a cui restituire il dato
 *	
 *	 \retval 0 se successo
 *	 \retval -1 se errore (errno settato EAGAIN)
 */
int tryget(B_Sync* b, void** item);
