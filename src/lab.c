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
        queue_t q = (queue_t)malloc(sizeof(struct queue) + capacity * sizeof(void *));
        if (q == NULL) {
            printf("Memory allocation failed\n");
            exit(1);
        }
        q->capacity = capacity;
        q->numItems = 0;
        q->frontOfQueue = -1;
        q->rearOfQueue = -1;
        q->shutdown = false;

        return q;
    }

    void queue_destroy(queue_t q)
    {
        free(q);
    }

    void enqueue(queue_t q, void *data)
    {
        if (q->numItems >= q->capacity) {
            printf("Attempted to add an item when queue is full.\n");
            exit(1);
        } else{
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
        }
    }

    void *dequeue(queue_t q)
    {
        if (q->numItems <= 0)
        {
            printf("Attempted to take an item from an empty queue.");
            exit(1);
        } else
        {
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
            return returnMe;
        }
    }

    void queue_shutdown(queue_t q)
    {
        q->shutdown = true;
    }

    bool is_empty(queue_t q)
    {
        return (q->numItems == 0);
    }

    bool is_shutdown(queue_t q)
    {
        return q->shutdown;
    }

#ifdef __cplusplus
} // extern "C"
#endif

#endif