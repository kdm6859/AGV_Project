#ifndef _CIRCLE_QUEUE_H_
#define _CIRCLE_QUEUE_H_

#include <stdint.h>
#include <stdio.h>

#define TRUE 1
#define FALSE 0
#define ERROR -1
#define MAX_SIZE 256//64

typedef uint8_t element;
typedef int boolean;

typedef struct{
  int rear;
  int front;
  element data[MAX_SIZE];
}Queue;

void init_circle_queue(Queue *q);
boolean is_empty(Queue *q);
void enqueue(Queue *q, element data);
element dequeue(Queue *q);
element peek(Queue *q);
boolean is_full(Queue *q);
void queue_print(Queue *q);
int queue_available(Queue *q);

#endif
