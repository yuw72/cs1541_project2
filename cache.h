/* This file contains a functional simulator of an associative cache with LRU replacement*/
#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

struct cache_blk_t { // note that no actual data will be stored in the cache 
  unsigned long tag;
  char valid;
  char dirty;
  unsigned LRU;	//to be used to build the LRU stack for the blocks in a cache set
};

struct cache_t {
	// The cache is represented by a 2-D array of blocks. 
	// The first dimension of the 2D array is "nsets" which is the number of sets (entries)
	// The second dimension is "assoc", which is the number of blocks in each set.
  int nsets;					// number of sets
  int blocksize;				// block size
  int assoc;					// associativity
  int miss_penalty;				// the miss penalty
  struct cache_blk_t **blocks;	// a pointer to the array of cache blocks
};

struct cache_t * cache_create(int size, int blocksize, int assoc)
{
  int i, nblocks , nsets ;
  struct cache_t *C = (struct cache_t *)calloc(1, sizeof(struct cache_t));
  nblocks = size *1024 / blocksize ;// number of blocks in the cache
  nsets = nblocks / assoc ;			// number of sets (entries) in the cache
  C->blocksize = blocksize ;
  C->nsets = nsets  ; 
  C->assoc = assoc;

  C->blocks= (struct cache_blk_t **)calloc(nsets, sizeof(struct cache_blk_t *));
  
  for(i = 0; i < nsets; i++) {
		C->blocks[i] = (struct cache_blk_t *)calloc(assoc, sizeof(struct cache_blk_t));
	}
  return C;
}

//------------------------------

int updateLRU(struct cache_t *cp ,int index, int way)
{
int k ;
for (k=0 ; k< cp->assoc ; k++) 
{
  if(cp->blocks[index][k].LRU < cp->blocks[index][way].LRU) 
     cp->blocks[index][k].LRU = cp->blocks[index][k].LRU + 1 ;
  // printf("index is %d   way is %d     value is %d\n",index, way, cp->blocks[index][k].LRU);
}
cp->blocks[index][way].LRU = 0 ;
}

int data_updateLRU(struct cache_t *d_cache ,int index, int way)
{
int k ;
for (k=0 ; k< d_cache->assoc ; k++) 
{
  if(d_cache->blocks[index][k].LRU < d_cache->blocks[index][way].LRU) 
     d_cache->blocks[index][k].LRU = d_cache->blocks[index][k].LRU + 1 ;
   //printf("index is %d  k is %d  k.value is %d  way is %d     value is %d\n",index, k,  d_cache->blocks[index][k].LRU, way, d_cache->blocks[index][k].LRU);
}
d_cache->blocks[index][way].LRU = 0 ;
}


int data_cache_access(struct cache_t *d_cache, struct Queue *write_buffer, unsigned long address, int access_type, int busy_writeBack, int *N1, int *N2, int *N3, int *L2_miss/*0 for read, 1 for write*/)
//returns 0 (if a hit), 1 (if its found in L2) or
//2 (if it is found in write buffer)
{
int i ;
int block_address ;
int index ;
int tag ;
int way ;
int max ;


block_address = (address / d_cache->blocksize);
tag = block_address / d_cache->nsets;
index = block_address - (tag * d_cache->nsets) ;

//printf("index is %d ,  tag is %d-------------\n", index, tag);

 // printf("size isssssssssssssssssssssssssssssssssss %d\n",get_size(*write_buffer));
 // printf("address is ssssssssssssssssssssssssssss   %d\n",address);
for (i = 0; i < d_cache->assoc; i++) {	/* look for the requested block */
  if (d_cache->blocks[index][i].tag == tag && d_cache->blocks[index][i].valid == 1) 
  {
  	data_updateLRU(d_cache, index, i) ;
  	if (access_type == 1) d_cache->blocks[index][i].dirty = 1 ;
  	return(0);						/* a cache hit */
  }
}
//////////////search buffer///////////////////////
if (search_write_buffer(write_buffer, index, tag) != -1) //hit
{
  *N1 = *N1 + 1;
  //printf("here---in writebuffer---------------\n");
      // look for an invalid entry

      // if valid==0  -----empty block
          // valid=1; tag=tag; updateLRU; dirty=1; 

          // remove_searched_buffer();
          // if (access_type ==1) valid = 1; tag = tag; updateLRU; dirty = 1; 
          // return(2);

              //probably a dead code

                                                                  for (way=0 ; way< d_cache->assoc ; way++)    /* look for an invalid entry */
                                                                   if (d_cache->blocks[index][way].valid == 0)  /* found an invalid entry */
                                                                   {
                                                                          d_cache->blocks[index][way].valid = 1 ;
                                                                                      d_cache->blocks[index][way].tag = tag ;
                                                                                                 if(busy_writeBack==0) deleteNode(write_buffer, index, tag);
                                                                                                     data_updateLRU(d_cache, index, way);
                                                                                                          d_cache->blocks[index][way].dirty = 1;
                                                                                                                 return(2);        
                                                                                   }

      // else if valid bit == 1; --full block
          //  find LRU block;
          //  if dirtybit==0
            //    evict the old block; dirty = 1; tag = tag
            //    if (access_type == 1) valid = 1; tag = tag; updateLRU; dirty = 1;
            //    remove_searched_buffer();
            //    return(2);
          //  else if (dirtybit==1)
            //    evict_old_block;
            //    remove_searched_buffer();
            //    write_buffer(old_block);
            //    if (access_type == 1) valid = 1; tag = tag; updateLRU; dirty = 1;
              //       return(2);

           ////////////pass//////////////
            
         max = d_cache->blocks[index][0].LRU ;     /* find the LRU block */
        
         way = 0 ;
         for (i=1 ; i< d_cache->assoc ; i++)  
          if (d_cache->blocks[index][i].LRU > max) {
            max = d_cache->blocks[index][i].LRU ;
            way = i ;
          }
         //printf("way is ===============%d\n", way); 
        // d_cache->blocks[index][way] is the LRU block
       //printf("way --------------is %d\n", way);
        data_updateLRU(d_cache, index, way);
        if (d_cache->blocks[index][way].dirty == 0)   
          {
            //////////pass////////////
            /* the evicted block is clean*/
            d_cache->blocks[index][way].dirty = 1;
            if(busy_writeBack == 0) deleteNode(write_buffer, index, tag);
            
            d_cache->blocks[index][way].tag = tag;
            //printf("--------------------------tag now is+++++++++++++%d\n",d_cache->blocks[index][way].tag);
              
            return(2);
          }
        else
          {
            /////! pass/////////

             /* the evicted block is dirty*/
    
            if(busy_writeBack == 0) 
            {
                deleteNode(write_buffer, index, tag);
                enqueue(write_buffer, index, d_cache->blocks[index][way].tag);
            }
            d_cache->blocks[index][way].dirty = 1;
            d_cache->blocks[index][way].tag = tag;
            return(2);
          }

}

/* a cache miss */
*L2_miss = *L2_miss + 1;

for (way=0 ; way< d_cache->assoc ; way++)		/* look for an invalid entry */
    if (d_cache->blocks[index][way].valid == 0)	/* found an invalid entry */
	 {
      // ##############pass########

      d_cache->blocks[index][way].valid = 1 ;
      d_cache->blocks[index][way].tag = tag ;
      d_cache->blocks[index][way].LRU = way;
	    data_updateLRU(d_cache, index, way);
	    d_cache->blocks[index][way].dirty = 0;
      if(access_type == 1) d_cache->blocks[index][way].dirty = 1;
	  return(1);				
    }

/////################pass#################
 max = d_cache->blocks[index][0].LRU ;			/* find the LRU block */
 way = 0 ;
 for (i=1 ; i< d_cache->assoc ; i++)  
  if (d_cache->blocks[index][i].LRU > max) {
    max = d_cache->blocks[index][i].LRU ;
    way = i ;
  }
  //printf("---------------way is %d\n",way);
// d_cache->blocks[index][way] is the LRU block




data_updateLRU(d_cache, index, way);
if (d_cache->blocks[index][way].dirty == 0)		
  {											/* the evicted block is clean*/
    //######passs###########
  	d_cache->blocks[index][way].dirty = access_type;
  	d_cache->blocks[index][way].tag = tag;

    return(1);
  }
else
  {	
    //######pass######
   										/* the evicted block is dirty*/
     //printf("maxxxxxxxxxxxxx is %d\n",wb_max);
    // printf("tag is %d\n",d_cache->blocks[index][way].tag);
       int wb_isFull=enqueue(write_buffer,index, d_cache->blocks[index][way].tag);
       *N3 = *N3 + 1;
       if(wb_isFull == -1)
       {
          *N2 = *N2 + 1;
       }
     //else
      // write_in_L2() 
	d_cache->blocks[index][way].dirty = access_type;
	d_cache->blocks[index][way].tag = tag;
  
  return(1);
  }

}







int inst_cache_access(struct cache_t *cp, unsigned long address, int access_type, int *L2_miss /*0 for read, 1 for write*/)
//returns 0 (if a hit), 1 (if a miss but no dirty block is writen back) or
//2 (if a miss and a dirty block is writen back)
{
int i ;
int block_address ;
int index ;
int tag ;
int way ;
int max ;

block_address = (address / cp->blocksize);
tag = block_address / cp->nsets;
index = block_address - (tag * cp->nsets) ;


//printf("index is %d ,  tag is %d-------------\n", index, tag);

 // printf("address is    %d\n",address);

for (i = 0; i < cp->assoc; i++) { /* look for the requested block */
  if (cp->blocks[index][i].tag == tag && cp->blocks[index][i].valid == 1) 
  {
  updateLRU(cp, index, i) ;
  if (access_type == 1) cp->blocks[index][i].dirty = 1 ;
  return(0);            /* a cache hit */
  }
}
/* a cache miss */

 *L2_miss = *L2_miss + 1;
for (way=0 ; way< cp->assoc ; way++)    /* look for an invalid entry */
    if (cp->blocks[index][way].valid == 0)  /* found an invalid entry */
   {
      cp->blocks[index][way].valid = 1 ;
      cp->blocks[index][way].tag = tag ;
      cp->blocks[index][way].LRU = way;
    updateLRU(cp, index, way);
    cp->blocks[index][way].dirty = 0;
      if(access_type == 1) cp->blocks[index][way].dirty = 1 ;
    return(1);        
    }

 max = cp->blocks[index][0].LRU ;     /* find the LRU block */
 way = 0 ;
 for (i=1 ; i< cp->assoc ; i++)  
  if (cp->blocks[index][i].LRU > max) {
    max = cp->blocks[index][i].LRU ;
    way = i ;
  }
// cp->blocks[index][way] is the LRU block
cp->blocks[index][way].tag = tag;
updateLRU(cp, index, way);
if (cp->blocks[index][way].dirty == 0)    
  {                     /* the evicted block is clean*/
  cp->blocks[index][way].dirty = access_type;
  return(1);
  }
else
  {                     /* the evicted block is dirty*/
  cp->blocks[index][way].dirty = access_type;
  return(2);
  }

}
