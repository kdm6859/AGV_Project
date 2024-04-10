#include "circle_queue.h"

void init_circle_queue(Queue *q){
  q->front = 0;
  q->rear = 0;
  //q->data = (element*)malloc(sizeof(element)*MAX_SIZE);
}

boolean is_empty(Queue *q){
  if(q->front == q->rear)
    return TRUE;
  else
    return FALSE;
}

void enqueue(Queue *q, element data){
  if(is_full(q)){
    //printf("\n포화 큐\n");
    //return;
    q->front = (q->front + 1) % (MAX_SIZE);
  }
  q->rear = (q->rear + 1) % (MAX_SIZE);
  q->data[q->rear] = data;
  
  return;
}

element dequeue(Queue *q){
  if(is_empty(q)){
    //printf("공백 큐\n");
  }
  q->front = (q->front + 1) % (MAX_SIZE);
  return q->data[q->front];
}

element peek(Queue *q){
  if(is_empty(q)){
    //printf("공백 큐\n");
  }
  return q->data[(q->front + 1) % MAX_SIZE];
}

boolean is_full(Queue *q){
  if(((q->rear + 1) % MAX_SIZE) == q->front)
    return TRUE;
  else
    return FALSE;
}

void queue_print(Queue *q){
  printf("QUEUE(front=%d rear=%d) = ", q->front, q->rear);
  if(!is_empty(q)){
    int i = q->front;
    do{
      i = (i + 1) % (MAX_SIZE);
      printf("%c | ", q->data[i]);
      if(i == q->rear)
        break;
    }while(i != q->front);
  }
  printf("\n");
}

int queue_available(Queue *q){
  int length;
  
  if(q->front > q->rear){
    length = ((MAX_SIZE) - q->front) + q->rear;
  }
  else
    length = q->rear - q->front;
  
  return length;
}



























