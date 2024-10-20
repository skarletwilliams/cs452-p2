#ifndef LAB_C
#define LAB_C
#include <stdlib.h>
#include <stdbool.h>
#include "lab.h"

#ifdef __cplusplus
extern "C"
{
#endif

    queue_t queue_init(int capacity)
    {
        //Initialize queue
        queue_t q = (queue_t)malloc(sizeof(struct queue) + capacity * sizeof(void *));
        if (q == NULL) {
            printf("Memory allocation failed\n");
            exit(1);
        }
        //Initialize all variables
        q->capacity = capacity;
        q->numItems = 0;
        q->frontOfQueue = -1;
        q->rearOfQueue = -1;
        q->shutdown = false;

        //Initialize mutex and race conditions
        pthread_mutex_init(&q->lock, NULL);
        pthread_cond_init(&q->not_empty, NULL);
        pthread_cond_init(&q->not_full, NULL);
        return q;
    }

    void queue_destroy(queue_t q)
    {
        pthread_mutex_destroy(&q->lock);
        pthread_cond_destroy(&q->not_empty);
        pthread_cond_destroy(&q->not_full);
        free(q);
    }

    void enqueue(queue_t q, void *data)
    {
        pthread_mutex_lock(&q->lock);

        while (q->numItems >= q->capacity) {
            //Wait until there is space in the queue to add
            pthread_cond_wait(&q->not_full, &q->lock);
        } 

        //Initial entry to queue
        if((q->frontOfQueue == -1) && (q->rearOfQueue == -1)) 
        {
            q->queue[0] = data;
            q->frontOfQueue = 0;
            q->rearOfQueue = 0;
            q->numItems = 1;
        } else 
        {
            //increment rear of queue and handle circular logic
            q->rearOfQueue = (q->rearOfQueue + 1) % q->capacity; 
            q->queue[q->rearOfQueue] = data;
            q->numItems++;
        }

        pthread_cond_signal(&q->not_empty);
        pthread_mutex_unlock(&q->lock);
    }

    void *dequeue(queue_t q)
    {
        pthread_mutex_lock(&q->lock);
        
        while (q->numItems <= 0)
        {
            //If the queue is empty and shut down, it can no longer be added to, unlock and return null
            if (q->shutdown) {
                pthread_mutex_unlock(&q->lock); 
                return NULL;
            }  
            //Otherwise, Wait until there are items in the queue to remove
            pthread_cond_wait(&q->not_empty, &q->lock);
        } 

        void *returnMe = q->queue[q->frontOfQueue];
        //Increment front and handle circular logic
        q->frontOfQueue = (q->frontOfQueue + 1) % q->capacity; 
        q->numItems--;
        //If empty reset to initial values
        if (q->numItems == 0) 
        {
            q->frontOfQueue = -1;
            q->rearOfQueue = -1;
        }

        pthread_cond_signal(&q->not_full);
        pthread_mutex_unlock(&q->lock);

        return returnMe;
    }

    void queue_shutdown(queue_t q)
    {
        pthread_mutex_lock(&q->lock);
        q->shutdown = true;
        pthread_cond_broadcast(&q->not_empty);//Broadcast to any waiting consumers to empty queue.
        pthread_mutex_unlock(&q->lock);
    }

    bool is_empty(queue_t q)
    {
        pthread_mutex_lock(&q->lock);
        bool returnMe = (q->numItems == 0);
        pthread_mutex_unlock(&q->lock);
        return returnMe;
    }

    bool is_shutdown(queue_t q)
    {
        pthread_mutex_lock(&q->lock);
        bool returnMe = q->shutdown;
        pthread_mutex_unlock(&q->lock);
        return returnMe;
    }

#ifdef __cplusplus
} // extern "C"
#endif

#endif