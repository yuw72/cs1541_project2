/**************************************************************/
/* CS/COE 1541              
   compile with gcc -o pipeline five_stage.c      
   and execute using              
   ./pipeline  /afs/cs.pitt.edu/courses/1541/short_traces/sample.tr 0  
***************************************************************/
// Team Name: Alex Blackson, Yuchao Wang, Nick West
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include "CPU.h" 
#include "hash.c"
#include "cache.h"
unsigned int I_accesses = 0;
unsigned int I_misses = 0;
unsigned int D_misses = 0;
unsigned int D_accesses = 0;
int data_hazard_condition(struct instruction IF)
{
  return (IF.type == ti_RTYPE || IF.type == ti_STORE || IF.type == ti_BRANCH);
}

int data_hazard_condition2(struct  instruction IF)
{
  return (IF.type == ti_ITYPE || IF.type == ti_JRTYPE || IF.type == ti_LOAD); 
}

int branch_not_taken(struct  instruction IF, struct instruction ID, int size)
{
  if(size==0) return 1;
  if((ID.PC+4==IF.PC))
    return 1;
  else 
    return 0; 
}

int nothing_in_hashtable(struct instruction IF)
{
  int key = IF.PC;
  item = search(key);
  if(item != NULL)
    return 0;
  else
    return 1;
}

int search_hashtable(struct instruction IF)
{
  int key=IF.PC;
  item = search(key);
  if(item == NULL)
    return 0;
  //printf("PC %d search data value %d \n",IF.PC,item->data);
  return item->data; 
}

int branch_PC_match(struct instruction IF)
{
  int key = IF.PC;
  item = search(key);

  return key==item->key;
}

int check_prediction(struct instruction IF, struct instruction ID, int taken_or_not)
{
 // printf("taken_or_not = %d\n", taken_or_not);
  int truly_taken_or_not = branch_taken(IF,ID);
 // printf("truly_taken_or_not = %d\n", truly_taken_or_not );
  return taken_or_not == truly_taken_or_not;
}

int branch_taken(struct instruction IF, struct instruction ID)
{
  if((ID.PC+4!=IF.PC))
    return 1;
  else 
    return 0;  
}

void overwrite_hashtable(struct instruction ID, int is_taken)
{
  int key = ID.PC;
  insert(key, is_taken, ID.Addr);
  //printf("the value in key %d is %d\n", key, search(key)->data);
}



int main(int argc, char **argv)
{
 int sin=0;
 int cos=0;

  struct instruction *tr_entry;
  struct instruction IF, ID, EX, MEM, WB;
  size_t size;
  char *trace_file_name;
  int trace_view_on = 0;
  int prediction_method = 0;
  int flush_counter = 4;  //5 stage pipeline, so we have to move 4 instructions once trace is done
  int data_hazard = 0;
  int control_hazard = 0;
  int taken_or_not = 0;   // branch detection in IF stage
  int is_taken = 0;
  int overwrite = 0;      // hash table overwritten.
  int overwrite_D = 0;
  int write_en = 0 ;
  int is_match = 0;
  unsigned int cycle_number = 0;

  if (argc == 1) {
    fprintf(stdout, "\nUSAGE: tv <trace_file> <switch - any character>\n");
    fprintf(stdout, "\n(switch) to turn on or off individual item view.\n\n");
    exit(0);
  }
    
  trace_file_name = argv[1];  
  if (argc == 4) 
  {
    prediction_method = atoi(argv[2]);
    trace_view_on = atoi(argv[3]) ;
  }
  else
  {
    printf("\nThere should be 3 arguments, please run again\n");
    exit(0);
  }
   FILE *fp;
   char buff[255];
   fp = fopen("cache_config.txt", "r");
   if(fp == NULL) 
    {
        printf("the file doesn't exist\n");
        exit(0);
    }
   // fscanf(fp, "%s", buff);
   // printf("1 : %s\n", buff );

   fgets(buff, 255, (FILE*)fp);
   unsigned int I_size = atoi(buff);
   printf("1: %d\n", I_size );
   
   fgets(buff, 255, (FILE*)fp);
   unsigned int I_assoc = atoi(buff);
   printf("2: %d\n", I_assoc );

   fgets(buff, 255, (FILE*)fp);
   unsigned int D_size = atoi(buff);
   printf("3: %d\n", D_size );
   
   fgets(buff, 255, (FILE*)fp);
   unsigned int D_assoc = atoi(buff);
   printf("4: %d\n", D_assoc );

   fgets(buff, 255, (FILE*)fp);
   unsigned int bsize = atoi(buff);
   printf("5: %d\n", bsize );
   
   fgets(buff, 255, (FILE*)fp);
   int wb_max = atoi(buff);
   printf("6: %d\n", wb_max);

   fgets(buff, 255, (FILE*)fp);
   unsigned int miss_penalty= atoi(buff);
  // printf("7: %d\n", miss_penalty);
   fclose(fp);
  
  // here you should extract the cache parameters from the configuration file 
   unsigned int I_bsize = bsize; 
   unsigned int wb_penalty = 1;
   unsigned int D_bsize = bsize; 
   unsigned int D_miss_penalty = miss_penalty;

  int stall=0;
  int count=miss_penalty;
  int busy_writeBack=0;
  int availability=0;
  int D_availability=0;
  int I_availability=0;
  fprintf(stdout, "\n ** opening file %s\n", trace_file_name);

  trace_fd = fopen(trace_file_name, "rb");

  if (!trace_fd) {
    fprintf(stdout, "\ntrace file %s not opened.\n\n", trace_file_name);
    exit(0);
  }

  trace_init();
  struct cache_t *I_cache;
  struct cache_t *D_cache;
  I_cache = cache_create(I_size, I_bsize, I_assoc); 
  D_cache = cache_create(D_size, D_bsize, D_assoc);
  node_t *write_buffer = NULL;

  while(1) {
    if ((!data_hazard)&& (!control_hazard)&& stall==0){
      size = trace_get_item(&tr_entry); /* put the instruction into a buffer */
    }

    if((data_hazard == 1 || control_hazard == 1)  && size==0) flush_counter=4;

    data_hazard = 0;
    control_hazard = 0;
    if (!size && flush_counter==0) 
    {       
      /* no more instructions (instructions) to simulate */
      printf("+ Simulation terminates at cycle : %u\n", cycle_number);
      break;
    }
    else
    {              /* move the pipeline forward */
      if(stall==0)  cycle_number++;
      /* move instructions one stage ahead */

      // Data Hazard Detection
      if(ID.type == ti_LOAD && stall == 0)
      {
        if(data_hazard_condition(IF))
        {
          if(ID.dReg == IF.sReg_a || ID.dReg == IF.sReg_b){
            data_hazard = 1;
            WB = MEM;
            MEM = EX;
            EX = ID;
            ID.type = ti_NOP;
            IF = IF;
          }
        }
        else if (data_hazard_condition2(IF))
        {
          if (ID.dReg == IF.sReg_a)
          {
            data_hazard = 1;
            WB = MEM;
            MEM = EX;
            EX = ID;
            ID.type = ti_NOP;
            IF = IF;
          }
        }
      }
    
      // Handling control hazards 
      if (prediction_method){

 ///////////////////////////////xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx///////////////////////////////////////////////////////////////////////////////////////       

        if(ID.type == ti_BRANCH)
        {
          is_match = check_prediction(IF,ID,taken_or_not);
           //printf("PC %d match = %d\n",ID.PC,is_match);
          if(is_match)
          {
            
            control_hazard = 0;
            overwrite = 0;
          }
          else
          {

            control_hazard =1;
            overwrite = 1;
          //  printf("before = %d\n", taken_or_not );
            taken_or_not =  abs(taken_or_not-1);
           // printf("after = %d\n", taken_or_not );
            WB = MEM;
            MEM = EX;
            EX = ID;
            ID.type = ti_NOP;
            IF = IF;
          }
          //printf("I want to overwrite = %d\n",overwrite);
          if(overwrite == 1){
            overwrite_hashtable(ID, taken_or_not);
            
          }
        }

        if(IF.type == ti_BRANCH)
        {
          if(nothing_in_hashtable(IF))
          {
            taken_or_not = 0;
            overwrite = 1;
          }
          else
          {
             taken_or_not = search_hashtable(IF);
            overwrite = 1 ;
          }
        }
    }
    else{
//########################################################################################################

        if (ID.type == ti_BRANCH && stall == 0)
        {
          if(branch_not_taken(IF,ID,size))
          {
              control_hazard = 0;
          }
          else
          { 
            control_hazard = 1;
            WB = MEM;
            MEM = EX;
            EX = ID;
            ID.type = ti_NOP;
            IF = IF;
          }
        }
    }
      

      if (!data_hazard && !control_hazard && stall == 0)
      {

        WB = MEM;
        MEM = EX;
        EX = ID;
        ID = IF;
      }     

      if(!size)
      {    /* if no more instructions in trace, reduce flush_counter */
        flush_counter--;   
      }
      else
      {   /* copy trace entry into IF stage */
        if (!data_hazard && (!control_hazard) && stall == 0)
        {
          memcpy(&IF, tr_entry , sizeof(IF));
          if(I_size !=0)
          {  
            if(busy_writeBack == 0)
            {
              int I_level = inst_cache_access(I_cache, IF.PC, 0);
              //printf("I_level is %d \n",I_level);
              if(I_level == 0 )  I_availability = 1;
              else
              {
                I_availability =0;
                cycle_number = cycle_number + miss_penalty;
                I_misses++;
              }
              I_accesses ++;
              //printf("I_availability is %d\n", I_availability);
            }
          else if(busy_writeBack == 1 && stall == 0)
          {

  //printf("--------------------------------------");
             int I_level = inst_cache_access(I_cache, IF.PC, 0);
             //printf("I_level is %d \n",I_level);
          
            if(I_level == 0)
            {
              I_availability = 1;
            }
            else
            {
              //printf("i am here");
              I_availability = 0;
              cycle_number = cycle_number+ miss_penalty;
              //printf("cycle number is %d\n",cycle_number);
              I_misses++;
            }
            I_accesses++;
          }
         } 
         else 
          I_accesses++; 

        }
      }     

    if(flush_counter > 0 )
    {
      if(D_size != 0)
      {
      if(busy_writeBack == 0)
      {
      // if()INST_cache_access
         
        if(MEM.type != ti_LOAD && MEM.type != ti_STORE)
           D_availability = 1;
        else
        {

            if(stall==0) D_accesses++; 
            int access_type;
            if(MEM.type == ti_LOAD) access_type = 0;
            if(MEM.type == ti_STORE) access_type = 1;
          int level = data_cache_access(D_cache, &write_buffer, MEM.Addr, access_type, wb_max);
          if (level == 0) //data cache L1 hit;
            D_availability = 1;
          else if(level == 2)
          {
              D_availability = 1;
              cycle_number += wb_penalty;
          }
          else
          {
            D_availability = 0;
            cycle_number += wb_penalty + miss_penalty;
            D_misses++;
          }
        
        }
    }
    //############busy_writeBack#################
    else if(busy_writeBack==1 && stall==0)
    {
     // if() inst_cache_access
      //printf("----------inside L2-----------\n");

         if(MEM.type != ti_LOAD && MEM.type != ti_STORE)
           {
             D_availability = 1;
             if(I_availability==0)
             {
               count = count-1; // 1 instruction -- 1 count decrement
          // cycle_number = cycle_number + 1;
             }
           }
        else
        {

            if(stall==0) D_accesses++; 
            int access_type;
            if(MEM.type == ti_LOAD) access_type = 0;
            if(MEM.type == ti_STORE) access_type = 1;
          int level = data_cache_access(D_cache, &write_buffer, MEM.Addr, access_type, wb_max);
      
          if (level == 0) //data cache L1 hit;
          {
             if(I_availability == 0) 
              {
                count = count -1; //  1 instruction -- 1 count decrement
                //cycle_number = cycle_number + 1;
              }
            D_availability = 1;
          //  printf("-------I am here--------\n");
          }
          else if(level == 2) // write buffer
          {
            D_availability = 1;

            if(I_availability == 1)
            {
              
              int old_count = count;
              count = count - wb_penalty;
              if(count <= 0) 
                cycle_number = cycle_number + wb_penalty-(old_count-1);
            }
            else
            {
              cycle_number = cycle_number + 1;
              count = count -1;//  1 instruction -- 1 count decrement
            }

          }
          else
          {
            //printf("here--------");
            //cycle_number = cycle_number + 1; //make up one cycle for the installing instruction
            D_availability = 0;
            if(I_availability == 1)
            {
              count = count -1 ;
              int temp_val = count - wb_penalty;
              if(temp_val >= 0 )
              { 
                count = temp_val ;
                //printf("----before cycle number is %d\n", cycle_number);
                cycle_number = cycle_number + miss_penalty + wb_penalty;
                //printf("after cycle number is %d----\n", cycle_number);
              }           
              else if (count == 0 )
              {
                
                cycle_number = cycle_number + miss_penalty + wb_penalty;
              }
              else
              {

                cycle_number = cycle_number + wb_penalty - count;
                
              }
            }
            else // I_availability = 0;
            {
            count = count -1;
            cycle_number = cycle_number + miss_penalty;
            }
            D_misses++;
          }
        
        } 
       
    }
      
     
    /////####################busy L2################################


    if(I_availability ==1 && D_availability ==1) 
      availability=1;
    else
        availability=0;

  //  printf("I_avila = %d   availability = %d   get_size = %d   busy_writeBack = %d\n",I_availability ,availability, get_size(write_buffer), busy_writeBack);
      
        if(availability == 1 && get_size(write_buffer)!=0)
        {
          //printf("----------inside L2-----------\n");

          busy_writeBack =1;
          if(stall==1)
          {
            dequeue(&write_buffer);
            //printf("before cycle number is %d\n", cycle_number);
            //printf("count is %d----------\n",count);
            cycle_number = cycle_number + count;
            //printf("after cycle number is %d\n", cycle_number);
            busy_writeBack = 0;
            stall = 0;
            count = miss_penalty;
          }
          else
          {
            count = count - 1;
          //printf("yoyo decrement--------- count is %d\n", count);

          }
          if(count <= 0)
          {
            busy_writeBack = 0;
            stall = 0;
            dequeue(&write_buffer);
            
            count = miss_penalty;
          }
          //printf("count is %d\n",count);
          
        }

        if(availability == 0 && busy_writeBack == 1)
        {
          printf("count----------------- is %d\n", count);
          stall = 1;
          I_availability = 1;
          D_availability = 1;
        }


        
       // printf("count is %d\n",count );
     }
     else // D_size =0;
       {
         if(MEM.type == ti_LOAD || MEM.type == ti_STORE)
           D_accesses++;
       }
        
    }

    }  


    if (trace_view_on && cycle_number>=5) {/* print the executed instruction if trace_view_on=1 */
      switch(WB.type) {
        case ti_NOP:
          printf("[cycle %d] NOP:\n",cycle_number) ;
          break;
        case ti_RTYPE: /* registers are translated for printing by subtracting offset  */
          printf("[cycle %d] RTYPE:",cycle_number) ;
          printf(" (PC: %d)(sReg_a: %d)(sReg_b: %d)(dReg: %d) \n", WB.PC, WB.sReg_a, WB.sReg_b, WB.dReg);
          break;
        case ti_ITYPE:
          printf("[cycle %d] ITYPE:",cycle_number) ;
          printf(" (PC: %d)(sReg_a: %d)(dReg: %d)(addr: %d)\n", WB.PC, WB.sReg_a, WB.dReg, WB.Addr);
          break;
        case ti_LOAD:
          printf("[cycle %d] LOAD:",cycle_number) ;      
          printf(" (PC: %d)(sReg_a: %d)(dReg: %d)(addr: %d)\n", WB.PC, WB.sReg_a, WB.dReg, WB.Addr);
          break;
        case ti_STORE:
          printf("[cycle %d] STORE:",cycle_number) ;      
          printf(" (PC: %d)(sReg_a: %d)(sReg_b: %d)(addr: %d)\n", WB.PC, WB.sReg_a, WB.sReg_b, WB.Addr);
          break;
        case ti_BRANCH:
          printf("[cycle %d] BRANCH:",cycle_number) ;
          printf(" (PC: %d)(sReg_a: %d)(sReg_b: %d)(addr: %d)\n", WB.PC, WB.sReg_a, WB.sReg_b, WB.Addr);
          break;
        case ti_JTYPE:
          printf("[cycle %d] JTYPE:",cycle_number) ;
          printf(" (PC: %d)(addr: %d)\n", WB.PC,WB.Addr);
          break;
        case ti_SPECIAL:
          printf("[cycle %d] SPECIAL:\n",cycle_number) ;        
          break;
        case ti_JRTYPE:
          printf("[cycle %d] JRTYPE:",cycle_number) ;
          printf(" (PC: %d) (sReg_a: %d)(addr: %d)\n", WB.PC, WB.dReg, WB.Addr);
          break;
      }
    }
  } // end of while-loop
printf("DATA access time is %d,  miss time is %d\n", D_accesses,D_misses);
printf("I-cache accesses %u and misses %u\n", I_accesses, I_misses);
printf("count inside is %d\n",sin);
printf("count outside is %d\n",cos);
  trace_uninit();

  exit(0);
}