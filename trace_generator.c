/**************************************************************/
/* CS/COE 1541				 			
AThis program allows the user to input the instructions for a trace 
and produce a trace file with these instructions readable by CPU.c.
The program takes the name of the file to be generated as an argument.
***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include "CPU.h" 

int main(int argc, char **argv)
{
  struct instruction *tr_entry=malloc(sizeof(struct instruction));
  size_t size;
  char *trace_file_name;
  int trace_view_on = 1;
  
  unsigned char t_type ;
  unsigned int t_sReg_a;
  unsigned int t_sReg_b;
  unsigned int t_dReg;
  unsigned int t_PC ;
  unsigned int t_Addr ;

  unsigned int cycle_number = 0;

  if (argc == 1) {
    fprintf(stdout, "\nMissing argument: the name of the file to be generated\n");
    exit(0);
  }
  trace_file_name = argv[1];

	int check;
int trcount, i, repeat;
char itype ;

printf("\nHow many instructions do you want to generate: ");
scanf("%d", &trcount );
printf("\n") ;

for (i = 0 ; i < trcount ; i++) {
printf("Enter the 6 fields for instruction %d [(PC) (itype R|L|S|B) (sReg_a) (sReg_b) (dReg) (addr)]:\n", i);
scanf("%d %c %d %d %d %d",  &tr_entry->PC, &itype, &t_sReg_a, &t_sReg_b, &t_dReg, &tr_entry->Addr) ;

//set the reegister values after casting
tr_entry->sReg_a = (char) t_sReg_a;
tr_entry->sReg_b = (char) t_sReg_b;
tr_entry->dReg = (char) t_dReg;

repeat = 0 ;
if(itype == 'R') {tr_entry->type = ti_RTYPE ;} 
  else if (itype == 'L') {tr_entry->type = ti_LOAD;} 
  else if (itype == 'S') {tr_entry->type = ti_STORE;} 
  else if (itype == 'B') {tr_entry->type = ti_BRANCH ;} 
  else {printf("unrecognized instruction type\n") ; repeat = 1;  i-- ; }

//write the instruction into the trace file
if (repeat == 0) write_trace(*tr_entry, trace_file_name);
} 

trace_fd = fopen(trace_file_name, "rb");
trace_init();
  while(1) {
    size = trace_get_item(&tr_entry);
   
    if (!size) {       /* no more instructions (trace_items) to simulate */
      printf("+ Simulation terminates at cycle : %u\n", cycle_number);
      break;
    }
    else{              /* simulate cycles */
      cycle_number++;
    }  

    // Display the generated trace 
    if (trace_view_on) {/* print the executed instruction if trace_view_on=1 */
      switch(tr_entry->type) {
        case ti_NOP:
          printf("[cycle %d] NOP:",cycle_number) ;
          break;
        case ti_RTYPE: /* registers are translated for printing by subtracting offset  */
          printf("[cycle %d] RTYPE:",cycle_number) ;
          printf(" (PC: %d)(sReg_a: %d)(sReg_b: %d)(dReg: %d) \n", tr_entry->PC, tr_entry->sReg_a, tr_entry->sReg_b, tr_entry->dReg);
          break;
        case ti_LOAD:
          printf("[cycle %d] LOAD:",cycle_number) ;      
          printf(" (PC: %d)(sReg_a: %d)(dReg: %d)(addr: %d)\n", tr_entry->PC, tr_entry->sReg_a, tr_entry->dReg, tr_entry->Addr);
          break;
        case ti_STORE:
          printf("[cycle %d] STORE:",cycle_number) ;      
          printf(" (PC: %d)(sReg_a: %d)(sReg_b: %d)(addr: %d)\n", tr_entry->PC, tr_entry->sReg_a, tr_entry->sReg_b, tr_entry->Addr);
          break;
        case ti_BRANCH:
          printf("[cycle %d] BRANCH:",cycle_number) ;
          printf(" (PC: %d)(sReg_a: %d)(sReg_b: %d)(addr: %d)\n", tr_entry->PC, tr_entry->sReg_a, tr_entry->sReg_b, tr_entry->Addr);
          break;
      }
    }
  }

  trace_uninit();

  exit(0);
}


