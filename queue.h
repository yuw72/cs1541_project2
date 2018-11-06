// C program for index implementation of queue 
#include <stdio.h> 
#include <stdlib.h> 
#include <limits.h> 
  
// A structure to represent a queue 
struct Queue 
{ 
    int front, rear, size; 
    unsigned capacity; 
    int* index; 
    int* tag;
}; 
  
// function to create a queue of given capacity.  
// It initializes size of queue as 0 
struct Queue* createQueue(unsigned capacity) 
{ 
    struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue)); 
    queue->capacity = capacity; 
    queue->size = 0; 
    queue->front =  0;
    queue->rear = capacity - 1;  // This is important, see the enqueue 
    queue->index = (int*) malloc(queue->capacity * sizeof(int)); 
    queue->tag = (int*) malloc(queue->capacity * sizeof(int));
    return queue; 
} 
  
// Queue is full when size becomes equal to the capacity  
int isFull(struct Queue* queue) 
{  return (queue->size == queue->capacity);  } 
  
// Queue is empty when size is 0 
int isEmpty(struct Queue* queue) 
{  return (queue->size == 0); } 
  
// Function to add an item to the queue.   
// It changes rear and size 
int enqueue(struct Queue* queue, int indx, int tg) 
{ 
    if (isFull(queue)) 
        return -1; 
    queue->rear = (queue->rear + 1)%queue->capacity; 
    queue->index[queue->rear] = indx;
    queue->tag[queue->rear] = tg;
    queue->size = queue->size + 1; 
     //printf("%d - %d enqueued to queue\n", indx,tg); 
    return 1;
   
} 
  
// Function to remove an item from queue.  
// It changes front and size 
int dequeue(struct Queue* queue) 
{ 
    if (isEmpty(queue)) 
        return INT_MIN; 


    int item = queue->tag[queue->front]; 
    //printf("%d - %d dequeued to queue\n", queue->index[queue->front], item);
    queue->front = (queue->front + 1)%queue->capacity; 
    queue->size = queue->size - 1; 
     
    return item; 
} 

int deleteNode(struct Queue* queue, int indx, int tg)
{
    int i;
    int max = queue->size;
    int position = search_write_buffer(queue, indx, tg);
    if(position == -1) return -1;
   //printf("%d - %d deleteNode to queue\n", queue->index[position], queue->tag[position]);
    for(i=position-1; i>=0;i--)
    {
        queue->index[i+1] = queue->index[i];
        queue->tag[i+1] = queue-> tag[i];
        
    }
    
    queue->front = (queue->front + 1)%queue->capacity; 
    queue->size = queue->size - 1; 
    return 0;
}
// Function to get front of queue 
int front(struct Queue* queue) 
{ 
    if (isEmpty(queue)) 
        return INT_MIN; 
    return queue->index[queue->front]; 
} 
  
// Function to get rear of queue 
int rear(struct Queue* queue) 
{ 
    if (isEmpty(queue)) 
        return INT_MIN; 
    return queue->index[queue->rear]; 
} 
  
int search_write_buffer(struct Queue* queue, int indx2, int tg2) 
{ 
    int i;
    int max = queue->size;
    int front = queue->front;
    for(i=front; i< front + max; i++)
    {
      int indx = queue->index[i];
      int tg = queue->tag[i];
      if(indx == indx2 && tg == tg2)
        return i;
    }
    return -1;
} 

void print_list(struct Queue* queue) 
{ 
    int i;
    int max = queue->size;
    int front = queue->front;
    
    for(i=front; i<front+max; i++)
    {
      int indx = queue->index[i];
      int tg = queue->tag[i];
      //printf("index is %d   tag is %d \n",indx, tg);
    }
    return ;
} 

int get_size(struct  Queue* queue)
{
     return queue->size;
}