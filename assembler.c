#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {

  char* rtype[] = {"", "lsl", "lsr", "and", "or", "sub", "seq", "slt"};
  char* mtype[] = {"setdes", "setsrc", "not", "ldw", "stw", "done\n", "",""};
  char* itype[] = {"","","mov", "movl", "beq", "blt", "b",""};

  // open files to read and write
  FILE* fin, *fout;
  fin = fopen(*(argv+1), "r");
  fout = fopen(*(argv+2), "w");

  char* instruction;
  char* input = malloc(100*sizeof(char));
  fgets(input, 100, fin);
  char* outCode = malloc(10*sizeof(char));// pointer to 10 byte array

  // Load instructions in  
  do {

    instruction = strtok(input, "@");// parses comments from instruction

    //string = malloc(count);
    char *i, *op;
    char count = 0;
    i = instruction;
   
    // get length of instruction
    while( *i != 0 ) {
      count++;
      i++;
    }

    // clear any preceeding spaces
    while(*instruction == ' ' && count >= 0) {
      instruction++;
      count--;
    }

    // if string is actual instruction
    if(count > 0) {
      op = strtok(instruction," ");
    }
    // if string is just comments, discard
    else {
      fgets(input, 100, fin); // get new value from file
      continue;
    }

    // parse tag from instruction
    char* tag = strtok(NULL, " ");

    if(tag == NULL) {
      // check if op was "done", else get new instruction
      if(strcmp(op, "done\n") && strcmp(op,"done")) {
        fgets(input, 100, fin);
        continue;
      }
      tag = op;
    }

    // write opcode to outCode
    for(int j = 0; j < 8; j++) {

      // generate output code for RType instruction
      if(!strcmp(rtype[j],op)) {// if op == rtype[j]
        outCode[0] = '0';// set 3bit opcode to 000 in outcode
        outCode[1] = '0';
        outCode[2] = '0';
        outCode[3] = ((*(tag+2) >> 2)&1)+48;
        outCode[4] = ((*(tag+2) >> 1)&1)+48;
        outCode[5] = (*(tag+2) &1)+48;
        outCode[6] = ((j>>2)&1)+48;// set 3 bit funccode to proper value
        outCode[7] = ((j>>1)&1)+48;
        outCode[8] = (j&1)+48;
        outCode[9] = '\n';

        break;
      }

      // generate output code for MType instruction
      else if(!strcmp(mtype[j],op)) {// if op == mtype[j]
        outCode[0] = '0';// set 3 bit opcode to 001 in outcode
        outCode[1] = '0';
        outCode[2] = '1';
        // j>2 ops have #imm as tag
        if(j>2 && j != 6) {
          outCode[3] = ((*(tag+1) >> 2)&1)+48;
          outCode[4] = ((*(tag+1) >> 1)&1)+48;
          outCode[5] = (*(tag+1) &1)+48;
        }
        // j <=2 ops have $rx as tag
        else {
          if(tag != 0) {
          outCode[3] = ((*(tag+2) >> 2)&1)+48;
          outCode[4] = ((*(tag+2) >> 1)&1)+48;
          outCode[5] = (*(tag+2) &1)+48;
          }
        }
        outCode[6] = ((j>>2)&1)+48;
        outCode[7] = ((j>>1)&1)+48;
        outCode[8] = (j&1)+48;
        outCode[9] = '\n';
        break;
      }

      // generate outputcode for IType instruction
      else if(!strcmp(itype[j],op)) {
        outCode[0] = ((j>>2)&1)+48;
        outCode[1] = ((j>>1)&1)+48;
        outCode[2] = (j&1)+48;

        // either mov or movl
        if(j<=3) {
          outCode[3] = '1';
          outCode[4] = '1';
          outCode[5] = '1';        
          outCode[6] = ((*(tag+1) >> 2)&1)+48;
          outCode[7] = ((*(tag+1) >> 1)&1)+48;
          outCode[8] = (*(tag+1)&1)+48;
          outCode[9] = '\n';
        }

        // branching instruction to LUT value
        else {
          outCode[3] = *(tag+1);
          outCode[4] = *(tag+2);
          outCode[5] = *(tag+3);
          outCode[6] = *(tag+4);
          outCode[7] = *(tag+5);
          outCode[8] = *(tag+6);
          outCode[9] = '\n';
        }
        break;
      }      
    }
    fwrite(outCode, 1, 10, fout);
    fgets(input, 100, fin);
  } while (!feof(fin));

return 0;
}
  
  
