/**************************************************************/
/* CS/COE 1541    
 * a simple simulator of a 5-stage pipeline with Instruction cache and perfect data cache
 * compile with gcc -o cpu CPU+cache.c                    
 * and execute using                                                    
 * ./cpu /afs/cs.pitt.edu/courses/1541/short_traces/sample.tr 0 0
 ****************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include "CPU.h" 
#include "cache.h"

//cache statistics
unsigned int I_accesses = 0;
unsigned int I_misses = 0;
unsigned int D_misses = 0;
unsigned int D_accesses = 0;



int main(int argc, char **argv)
{
  struct instruction *tr_entry;
  struct instruction IF, ID, EX, MEM, WB;
  size_t size;
  char *trace_file_name;
  int trace_view_on = 0;
  int flush_counter = 4; //5 stage pipeline, so we have to move 4 instructions once trace is done

  unsigned int cycle_number = 0;

  if (argc == 1) {
    fprintf(stdout, "\nUSAGE: tv <trace_file> <switch - any character>\n");
    fprintf(stdout, "\n(switch) to turn on or off individual item view.\n\n");
    exit(0);
  }

  trace_file_name = argv[1];
  if (argc == 3) trace_view_on = atoi(argv[2]) ;

  // here you should extract the cache parameters from the configuration file 
  unsigned int I_size = 2 ; 
  unsigned int I_assoc = 4;
  unsigned int I_bsize = 16; 
  unsigned int miss_penalty = 10;
  unsigned int wb_penalty = 1;
  int wb_max = 1;  //write_buffer size
  unsigned int D_size = 2 ; 
  unsigned int D_assoc = 2;
  unsigned int D_bsize = 16; 
  unsigned int D_miss_penalty = 10;

  int stall=0;
  int count=miss_penalty;
  int busy_writeBack=0;
  int availability=0;





  fprintf(stdout, "\n ** opening file %s\n", trace_file_name);

  trace_fd = fopen(trace_file_name, "rb");

  if (!trace_fd) {
    fprintf(stdout, "\ntrace file %s not opened.\n\n", trace_file_name);
    exit(0);
  }

  trace_init();
  struct cache_t *I_cache;
  struct cache_t *D_cache;
  //I_cache = cache_create(I_size, I_bsize, I_assoc); 
  D_cache = cache_create(D_size, D_bsize, D_assoc);
  node_t *write_buffer = NULL;
  while(1) {
    size = trace_get_item(&tr_entry);
    if (!size && flush_counter==0) {  /* no more instructions (instructions) to simulate */
      //modify the statistics output format according to the project description
      printf("+ Simulation terminates at cycle : %u\n", cycle_number);
      printf("I-cache accesses %u and misses %u\n", I_accesses, I_misses);
      break ;
    }
     else{              /* move the pipeline forward */
     if(stall==0)
      cycle_number++;

      /* move instructions one stage ahead */
      if(stall == 0)
      {
	      WB = MEM;
	      MEM = EX;
	      EX = ID;
	      ID = IF;
	  }
      if(!size&& stall==0){    /* if no more instructions in trace, reduce flush_counter */
        flush_counter--;
      }
	  else {	
	  	 //instruction cache access
	  }
	  if(flush_counter > 0 )
	  {
	  	if(stall==0) memcpy(&IF, tr_entry, sizeof(IF));
	    
	    if(busy_writeBack == 0)
     	{
     	// if()INST_cache_access
			  if(MEM.type != ti_LOAD && MEM.type != ti_STORE)
			  	 availability = 1;
			  else
			  {
			  	  int access_type;
			  	  if(MEM.type == ti_LOAD) access_type = 0;
			  	  if(MEM.type == ti_STORE) access_type = 1;
				  int level = data_cache_access(D_cache, &write_buffer, MEM.Addr, access_type, wb_max);
				  if (level == 0)	//data cache L1 hit;
					  availability = 1;
				  else if(level == 2)
				  {
				   	  availability = 1;
				   	  cycle_number += wb_penalty;
				  }
				  else
				  {
				  	availability = 0;
				  	cycle_number += wb_penalty + miss_penalty;
				  	D_misses++;
				  }
				  D_accesses++;
			  }
		}
		//############busy_writeBack#################
		else if(busy_writeBack==1 && stall==0)
		{
		 // if() inst_cache_access
			//printf("----------inside L2-----------\n");
			   if(MEM.type != ti_LOAD && MEM.type != ti_STORE)
			  	 availability = 1;
			  else
			  {
			  	  int access_type;
			  	  if(MEM.type == ti_LOAD) access_type = 0;
			  	  if(MEM.type == ti_STORE) access_type = 1;
				  int level = data_cache_access(D_cache, &write_buffer, MEM.Addr, access_type, wb_max);
				  if (level == 0)	//data cache L1 hit;
					  availability = 1;
				  else if(level == 2)
				  {
				   	  availability = 1;
				   	  count = count-1;
				   	  printf("yoyo level 2 count is %d--------",count);
				   	  cycle_number += wb_penalty;
				  }
				  else
				  {
				  	//printf("-------I am here--------\n");
				  	availability = 0;
				  	count = count -1 ;
				  	if(count-1>=0)
				  	{ 
				  		count = count -1 ;
				  		printf("----before cycle number is %d\n", cycle_number);
				  		cycle_number = cycle_number + miss_penalty + wb_penalty;
				  		printf("after cycle number is %d----\n", cycle_number);
				  	}
				  	//else if(count==1) cycle_number = cycle_number + miss_penalty;
				  	else if (count == 0 )
				  	{
				  	 cycle_number = cycle_number + miss_penalty+wb_penalty;
				  	}
				  	D_misses++;
				  }
				  D_accesses++;
			  }	
		}
			//printf("buffer size is %d--------------------\n", get_size(write_buffer));
        if(availability == 1 && get_size(write_buffer)!=0)
        {
        	//printf("----------inside L2-----------\n");
        	busy_writeBack =1;
        	if(stall==1)
        	{
        		dequeue(&write_buffer);
        		//printf("before cycle number is %d\n", cycle_number);
        		printf("count is %d----------\n",count);
        		cycle_number = cycle_number + count;
        		//printf("after cycle number is %d\n", cycle_number);
        		busy_writeBack = 0;
        		stall=0;
        		count = miss_penalty;
        	}
        	else
        	{
        		count = count - 1;
			//printf("yoyo decrement--------- count is %d\n", count);
        	}
        	if(count == 0)
        	{
        		busy_writeBack = 0;
        		stall = 0;
        		dequeue(&write_buffer);
        		availability = 0;
        		count = miss_penalty;
        	}
        	
        }

        if(availability == 0 && busy_writeBack == 1)
        {
        	stall = 1;
        	availability = 1;
        }


        //printf("availability = %d\n",availability);
       // printf("count is %d\n",count );





	  }

      //printf("==============================================================================\n");
      //
    }  

	 if (trace_view_on && cycle_number >= 5 ) {/* print the executed instruction if trace_view_on=1 */
		 switch (WB.type) {
		 case ti_NOP:
			 printf("[cycle %d] NOP:\n", cycle_number);
			 break;
		 case ti_RTYPE: /* registers are translated for printing by subtracting offset  */
			 printf("[cycle %d] RTYPE:", cycle_number);
			 printf(" (PC: %d)(sReg_a: %d)(sReg_b: %d)(dReg: %d) \n", WB.PC, WB.sReg_a, WB.sReg_b, WB.dReg);
			 break;
		 case ti_ITYPE:
			 printf("[cycle %d] ITYPE:", cycle_number);
			 printf(" (PC: %d)(sReg_a: %d)(dReg: %d)(addr: %d)\n", WB.PC, WB.sReg_a, WB.dReg, WB.Addr);
			 break;
		 case ti_LOAD:
			 printf("[cycle %d] LOAD:", cycle_number);
			 printf(" (PC: %d)(sReg_a: %d)(dReg: %d)(addr: %d)\n", WB.PC, WB.sReg_a, WB.dReg, WB.Addr);
			 break;
		 case ti_STORE:
			 printf("[cycle %d] STORE:", cycle_number);
			 printf(" (PC: %d)(sReg_a: %d)(sReg_b: %d)(addr: %d)\n", WB.PC, WB.sReg_a, WB.sReg_b, WB.Addr);
			 break;
		 case ti_BRANCH:
			 printf("[cycle %d] BRANCH:", cycle_number);
			 printf(" (PC: %d)(sReg_a: %d)(sReg_b: %d)(addr: %d)\n", WB.PC, WB.sReg_a, WB.sReg_b, WB.Addr);
			 break;
		 case ti_JTYPE:
			 printf("[cycle %d] JTYPE:", cycle_number);
			 printf(" (PC: %d)(addr: %d)\n", WB.PC, WB.Addr);
			 break;
		 case ti_SPECIAL:
			 printf("[cycle %d] SPECIAL:\n", cycle_number);
			 break;
		 case ti_JRTYPE:
			 printf("[cycle %d] JRTYPE:", cycle_number);
			 printf(" (PC: %d) (sReg_a: %d)(addr: %d)\n", WB.PC, WB.dReg, WB.Addr);
			 break;
		 }
	 } 
  }
  printf("DATA access time is %d,  miss time is %d\n", D_accesses,D_misses);
  trace_uninit();

  exit(0);
}


