#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <readtext.h>
#include <sim.h>

using namespace std;


int main()
{

    void* mem = malloc(6291456);
    int* txtptr = (int*)mem;                // The pointer of text segment. Starts from 0x00400000.
    int* PC = txtptr;                       // The program counter.
    int* stcptr = txtptr + 262144;          // The pointer of static data segment. Starts from 0x00500000.
    long offset = (long)txtptr - 0x00400000;  // The offset between the real address and the simulated address.

    int reg[34];
    *reg = 0;
    *(reg+28) = (int)(0x00500000);       // Initialize some registers.
    *(reg+29) = (int)(0x009fffff);
    *(reg+30) = (int)(0x009fffff);

    stcptr = readtext(txtptr, stcptr);      // Read the MIPS code, store the data in the allocated memory and return the static pointer.

    int* hepptr = stcptr + 1;               // The pointer pf the heap. Starts from where the static data ends.

    simulate(PC, reg, hepptr, txtptr, offset);

    free(mem);

    return 0;
}
