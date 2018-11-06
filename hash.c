#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define SIZE 64

struct DataItem {
   int key;
   int data;
   int target_addr;  
   
};

struct DataItem* hashArray[SIZE]; 
struct DataItem* dummyItem;
struct DataItem* item;

int hashCode(int key) {
   int index=five_bit_convert(key);
   
   return index;
}

int five_bit_convert(key)
{
	int n=key;
	int binaryNum[32]; 
	int i = 0;
	int j=0; 
	while(j<32)
	{
		binaryNum[j]=0;
		j++;
	}
    while (n > 0) { 
        binaryNum[i] = n % 2; 
        n = n / 2; 
        i++; 
    } 
    return binaryNum[9]*pow(2,5)+binaryNum[8]*pow(2,4)+binaryNum[7]*pow(2,3)+binaryNum[6]*pow(2,2)+binaryNum[5]*pow(2,1)+binaryNum[4]*pow(2,0);
}

struct DataItem *search(int key) {
   //get the hash 
   int hashIndex = hashCode(key);  
	
   //move in array until an empty 
   if(hashArray[hashIndex] != NULL) {
      return hashArray[hashIndex];  
   }        
   else return NULL;
     
}

void insert(int key,int data, int target_addr ) {

   struct DataItem *item = (struct DataItem*) malloc(sizeof(struct DataItem));
   item->key = key;
   item->data = data;
   item->target_addr = target_addr;  
   

   //get the hash 
   int hashIndex = hashCode(key);

   //move in array until an empty or deleted cell	

   hashArray[hashIndex] = item;
   
}


void display() {
   int i = 0;
	
   for(i = 0; i<SIZE; i++) {
	    
      if(hashArray[i] != NULL)
         printf(" (%d,%d,%d)",hashArray[i]->key,hashArray[i]->data,hashArray[i]->target_addr);
      else
         printf(" ~~ ");
   }
	
   printf("\n");
}


