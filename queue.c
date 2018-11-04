#include <stdio.h>
#include <stdlib.h>

typedef struct node {
    unsigned long index;
    unsigned long tag;
    struct node *next;
} node_t;

int enqueue(node_t **head, unsigned long index,unsigned long tag,int max) {
    if(get_size(*head) >= max) return -1;
    node_t *new_node = malloc(sizeof(node_t));
    if (!new_node) return -1;

    new_node->index = index;
    new_node->tag = tag;
    new_node->next = *head;

    *head = new_node;

    return 1;
}

int dequeue(node_t **head) {
    node_t *current, *prev = NULL;
    int retval = -1;

    if (*head == NULL) return -1;

    current = *head;
    while (current->next != NULL) {
        prev = current;
        current = current->next;
    }

    retval = current->index;
    free(current);
    
    if (prev)
        prev->next = NULL;
    else
        *head = NULL;
    
    return retval;
}

  
int deleteNode(node_t **head, unsigned long index, unsigned long tag) 
{ 
    // Store head node 
    node_t *prev = NULL;
    node_t *current = *head; 
    
    if(*head == NULL) return -1;
    if(current->index==index && current->tag == tag)
    {
        *head = current->next;
        free(current);
        return 1;
    }

    while (current!=NULL && (current->index != index || current->tag != tag)) 
    { 
        prev = current; 
        current = current->next; 
    }
    if(current == NULL) return -1;
    // Unlink the node from linked list 
    prev->next = current->next; 
    
    free(current);  // Free memory 

    return 1;
} 

int search_write_buffer(node_t *head, unsigned long index, unsigned long tag)
{
    node_t *current = head;
    if(head==NULL) return -1;

    while (current!=NULL ) 
    {
        if(current->index == index && current->tag == tag) 
        {
            free(current);
            return 1;
        }
        current = current->next; 
    }
    free(current);
    return -1;
}

void print_list(node_t *head) {
    node_t *current = head;

    while (current != NULL) {
        printf("%d %d\n", current->index,current->tag);
        current = current->next;
    }
}

int get_size(node_t *head)
{
  node_t *current = head;
  int size=0;
  while (current != NULL) {
        size++;
        current = current->next;
    } 
  return size;    
}

int main() {
    node_t *head = NULL;
    int max=1;
    
    int ret;

    int a=enqueue(&head, 11,100, max);
    int b= enqueue(&head, 22,110,max);
    int c=enqueue(&head, 33,120,max);
    int d=enqueue(&head, 44,130,max);
    printf("get_size is %d\n",get_size(head));
    //printf("a is %d b is %d  c is %d  d is %d\n", a, b ,c, d);
    print_list(head);
    
    int delete = deleteNode(&head,11,100);
    printf("delete is %d\n", delete);
    // while ((ret=dequeue(&head)) > 0) {
    //     printf("dequeued %d\n", ret);
    // }
   // printf("done. head=%p\n", head);
     //enqueue(&head, 11,100,max);

     int search = search_write_buffer(head, 11, 100);
     printf("search is %d\n",search);
    print_list(head);
    printf("get_size is %d\n",get_size(head));
    return 0;
}
