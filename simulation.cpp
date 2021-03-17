#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cmath>
#include <asm.h>

using namespace std;


/* Function: simulate()
 * This function simulates the code processing of CPU.
 *
 * The function maintains a program counter which points to the real address on the computer
 * where the line of binary code is stored. After the code is read into the 32-bit integer,
 * the PC increses 1. The code is sent for analysis. Lots of "if" conditional branches were
 * used here, to determine the right operations for this code. For each branch, the meaning is
 * in the comment on the right hand side.
 *
 * This function contains a endless while loop, since the exit should be handled by the MIPS programmer.
 */

void simulate(int* PC, int* reg, int* hepptr, int* txtptr, long offset){
    int filenum;
    bool loopflag = true;
    ifstream fin;
    ofstream fout;
    fstream fio;
    while (loopflag){
        unsigned int code;
        code = *PC;                 // Read a line of code from the current program counter.
        PC += 1;
        int hi = 32;
        int lo = 33;
        int op = code / pow(2,26);              // Extract the 6 most significant bit as the op code.

        if (op == 0){                                   // The R instructions.
            int rs = (code % (int)pow(2,26)) / pow(2,21);
            int rt = (code % (int)pow(2,21)) / pow(2,16);
            int rd = (code % (int)pow(2,16)) / pow(2,11);
            int shamt = (code % (int)pow(2,11)) / pow(2,6);
            int funct = (code % (int)pow(2,6));

            if (funct == 0x20){                     // add
                if (*(reg+rs) + *(reg+rt) > pow(2,31)-1 || *(reg+rs) + *(reg+rt) < -pow(2,31)){
                    cout << "overflow at " << (int*)((long)(PC-1)-offset) << endl;
                    loopflag = false;
                }
                *(reg+rd) = *(reg+rs) + *(reg+rt);
            }
            if (funct == 0x21){                     // addu
                *(reg+rd) = (unsigned int)*(reg+rs) + (unsigned int)*(reg+rt);
            }
            if (funct == 0x24){                     // and
                *(reg+rd) = *(reg+rs) & *(reg+rt);
            }
            if (funct == 0x1a){                     // div
                *(reg+lo) = *(reg+rs) / *(reg+rt);
                *(reg+hi) = *(reg+rs) % *(reg+rt);
            }
            if (funct == 0x1b){                     // divu
                *(reg+lo) = (unsigned int)*(reg+rs) / (unsigned int)*(reg+rt);
                *(reg+hi) = (unsigned int)*(reg+rs) % (unsigned int)*(reg+rt);
            }
            if (funct == 0x18){                     // mult
                *(reg+lo) = (*(reg+rs) * *(reg+rt)) & (int)pow(2,32);
                *(reg+hi) = (*(reg+rs) * *(reg+rt)) / (int)pow(2,32);
            }
            if (funct == 0x19){                     // multu
                *(reg+lo) = ((unsigned int)*(reg+rs) * (unsigned int)*(reg+rt)) & (int)pow(2,32);
                *(reg+hi) = ((unsigned int)*(reg+rs) * (unsigned int)*(reg+rt)) / (int)pow(2,32);
            }
            if (funct == 0x27){                     // nor
                *(reg+rd) = ~(*(reg+rs) | *(reg+rt));
            }
            if (funct == 0x25){                     // or
                *(reg+rd) = *(reg+rs) | *(reg+rt);
            }
            if (funct == 0x0){                      // sll
                *(reg+rd) = *(reg+rt) << shamt;
            }
            if (funct == 0x4){                      // sllv
                *(reg+rd) = *(reg+rt) << *(reg+rt);
            }
            if (funct == 0x3){                      // sra
                *(reg+rd) = *(reg+rt) >> shamt;
            }
            if (funct == 0x7){                      // srav
                *(reg+rd) = *(reg+rt) >> *(reg+rs);
            }
            if (funct == 0x2){                      // srl
                *(reg+rd) = (unsigned int)*(reg+rt) >> shamt;
            }
            if (funct == 0x6){                      // srlv
                *(reg+rd) = (unsigned int)*(reg+rt) >> *(reg+rs);
            }
            if (funct == 0x22){                     // sub
                if (*(reg+rs) - *(reg+rt) > pow(2,31)-1 || *(reg+rs) - *(reg+rt) < -pow(2,31)){
                    cout << "overflow at " << (int*)((long)(PC-1)-offset) << endl;
                    loopflag = false;
                }
                *(reg+rd) = *(reg+rs) - *(reg+rt);
            }
            if (funct == 0x23){                     // subu
                *(reg+rd) = (unsigned int)*(reg+rs) - (unsigned int)*(reg+rt);
            }
            if (funct == 0x26){                     // xor
                *(reg+rd) = *(reg+rs) ^ *(reg+rt);
            }
            if (funct == 0x2a){                     // slt
                if (*(reg+rs) < *(reg+rt)) *(reg+rd) = 1;
                else *(reg+rd) = 0;
            }
            if (funct == 0x2b){                     // sltu
                if ((unsigned int)*(reg+rs) < (unsigned int)*(reg+rt)) *(reg+rd) = 1;
                else *(reg+rd) = 0;
            }
            if (funct == 0x9){                      // jalr
                *(reg+rd) = ((long)(PC))-offset;
                PC = (int*)(long)((*(reg+rs))+offset);
            }
            if (funct == 0x8){                      // jr
                PC = (int*)(long)((*(reg+rs))+offset);
            }
            if (funct == 0x34){                     // teq
                if (*(reg+rs) == *(reg+rt)){
                    cout << "Trap exception at " << (int*)((long)(PC-1)-offset) << endl;
                    loopflag = false;
                }
            }
            if (funct == 0x36){                     // tne
                if (*(reg+rs) != *(reg+rt)){
                    cout << "Trap exception at " << (int*)((long)(PC-1)-offset) << endl;
                    loopflag = false;
                }
            }
            if (funct == 0x30){                     // tge
                if (*(reg+rs) >= *(reg+rt)){
                    cout << "Trap exception at " << (int*)((long)(PC-1)-offset) << endl;
                    loopflag = false;
                }
            }
            if (funct == 0x31){                     // tgeu
                if ((unsigned int)*(reg+rs) >= (unsigned int)*(reg+rt)){
                    cout << "Trap exception at " << (int*)((long)(PC-1)-offset) << endl;
                    loopflag = false;
                }
            }
            if (funct == 0x32){                     // tlt
                if (*(reg+rs) < *(reg+rt)){
                    cout << "Trap exception at " << (int*)((long)(PC-1)-offset) << endl;
                    loopflag = false;
                }
            }
            if (funct == 0x33){                     // tltu
                if ((unsigned int)*(reg+rs) < (unsigned int)*(reg+rt)){
                    cout << "Trap exception at " << (int*)((long)(PC-1)-offset) << endl;
                    loopflag = false;
                }
            }
            if (funct == 0x10){                     // mfhi
                *(reg+rd) = *(reg+hi);
            }
            if (funct == 0x12){                     // mflo
                *(reg+rd) = *(reg+lo);
            }
            if (funct == 0x11){                     // mthi
                *(reg+hi) = *(reg+rs);
            }
            if (funct == 0x13){                     // mtlo
                *(reg+lo) = *(reg+rs);
            }
            if (funct == 0xc){                      // syscall

                if (*(reg+2) == 1){             // print_int
                    cout << *(reg+4);
                }
                else if (*(reg+2) == 4){             // print_string
                    char* addr = (char*)(long)((*(reg+4))+offset);
                    while (*addr != '\0'){
                        cout << *addr;
                        addr += 1;
                    }
                }
                else if (*(reg+2) == 5){             // read_int
                    int a;
                    cin >> a;

                    *(reg+2) = a;
                }
                else if (*(reg+2) == 8){             // read_string
                    string str;
                    cin >> str;
                    char* addr = (char*)(long)((*(reg+4))+offset);
                    if ((int)str.length() < *(reg+5)-1){
                        for (char ch:str){
                            *addr = ch;
                            addr += 1;
                        }
                    }
                    else{
                        for (int i=0; i< *(reg+5); i++){
                            *addr = str[i];
                            addr += 1;
                        }
                    }
                }
                else if (*(reg+2) == 9){             // sbrk
                    *(reg+2) = ((long)hepptr)-offset;
                    if ((*(reg+4))%4 == 0){
                        hepptr += (*(reg+4))/4;
                    }
                    else{
                        hepptr += (*(reg+4))/4 + 1;
                    }
                }
                else if (*(reg+2) == 10){            // exit
                    loopflag = false;
                }
                else if (*(reg+2) == 11){            // print_char
                    cout << (char)*(reg+4);
                }
                else if (*(reg+2) == 12){            // read_char
                    char ch;
                    cin >> ch;
                    *(reg+2) = (int)ch;
                }
                else if (*(reg+2) == 13){            // open
                    string filename;
                    char* addr = (char*)(long)((*(reg+4))+offset);
                    while (*addr != '\0'){
                        filename += *addr;
                        addr += 1;
                    }
                    if (*(reg+5) == 0){         // open_read
                        if (fin.is_open()){
                            *(reg+4) = -1;
                        }
                        else{
                            fin.open(filename, ios::in);
                            filenum = 3;
                            *(reg+4) = filenum;
                            if (!fin.is_open()){
                                cout << "Unable to open." << endl;
                                *(reg+4) = -1;
                                loopflag = false;
                            }
                        }
                    }
                    else if (*(reg+5) == 1){     // open_write
                        if (fout.is_open()){
                            *(reg+4) = -1;
                        }
                        else{
                            fout.open(filename, ios::out);
                            filenum = 4;
                            *(reg+4) = filenum;
                        }
                    }
                    else if (*(reg+5) == 2){     // open_rw
                        if (fio.is_open()){
                            *(reg+4) = -1;
                        }
                        else{
                            fio.open(filename, ios::app);
                            fio.close();
                            fio.open(filename, ios::in | ios::out);
                            filenum = 5;
                            *(reg+4) = filenum;
                            if (!fio.is_open()){
                                cout << "Unable to open." << endl;
                                *(reg+4) = -1;
                                loopflag = false;
                            }
                        }
                    }
                }
                else if (*(reg+2) == 14){             // read
                    if (*(reg+4) == 3){
                        char ch;
                        int count = 0;
                        char* chptr = (char*)(long)((*(reg+5))+offset);
                        while ((ch=fin.get()) != EOF){
                            if (count == *(reg+6)) break;
                            *chptr = ch;
                            chptr += 1;
                            count += 1;
                        }
                        *(reg+4) = count;
                    }
                    else if (*(reg+4) == 5){
                        char ch;
                        int count = 0;
                        char* chptr = (char*)(long)((*(reg+5))+offset);
                        while ((ch=fio.get()) != EOF){
                            if (count == *(reg+6)) break;
                            *chptr = ch;
                            chptr += 1;
                            count += 1;
                        }
                        *(reg+4) = count;
                    }
                }
                else if (*(reg+2) == 15){            // write
                    if (*(reg+4) == 4){
                        char ch;
                        int count = 0;
                        char* chptr = (char*)(long)((*(reg+5))+offset);
                        while (count < *(reg+6)){
                            ch = *chptr;
                            fout << ch;
                            chptr += 1;
                            count += 1;
                        }
                        *(reg+4) = count;
                    }
                    else if (*(reg+4) == 5){
                        char ch;
                        int count = 0;
                        char* chptr = (char*)(long)((*(reg+5))+offset);
                        while (count < *(reg+6)){
                            ch = *chptr;
                            fio << ch;
                            chptr += 1;
                            count += 1;
                        }
                        *(reg+4) = count;
                    }
                }
                else if (*(reg+2) == 16){            // close
                    if (*(reg+4) == 3){
                        fin.close();
                    }
                    if (*(reg+4) == 4){
                        fout.close();
                    }
                    if (*(reg+4) == 5){
                        fio.close();
                    }
                }
                else if (*(reg+2) == 17){            // exit2
                    cout << "Exit code: " << *(reg+4) << endl;
                    loopflag = false;
                }
            }
        }
        else if (op == 0x2 || op == 0x3){                   // The J instructions.
            int target = code % (int)pow(2, 26);

            if (op == 0x2){                           // j
                PC = (int*)(target*4+(long)txtptr);
            }
            if (op == 0x3){                           // jal
                *(reg+31) = ((long)PC)-offset;
                PC = (int*)(target*4+(long)txtptr);
            }
        }
        else{                                           // The I instructions.
            int rs = (code % (int)pow(2,26)) / pow(2,21);
            int rt = (code % (int)pow(2,21)) / pow(2,16);
            int imm = (code % (int)pow(2, 16));
            if (imm >= 32768){
                imm -= 65536;
            }

            if (op == 0x8){                         // addi
                if (*(reg+rs) + imm > pow(2,31)-1 || *(reg+rs) + imm < -pow(2, 31)){
                    cout << "overflow at " << (int*)((long)(PC-1)-offset) << endl;
                    loopflag = false;
                }
                *(reg+rt) = *(reg+rs) + imm;
            }
            if (op == 0x9){                         // addiu
                *(reg+rt) = (unsigned int)*(reg+rs) + (unsigned int)imm;
            }
            if (op == 0xc){                         // andi
                *(reg+rt) = *(reg+rs) & imm;
            }
            if (op == 0xd){                         // ori
                *(reg+rt) = *(reg+rs) | imm;
            }
            if (op == 0xe){                         // xori
                *(reg+rt) = *(reg+rs) ^ imm;
            }
            if (op == 0xf){                         // lui
                *(reg+rt) = imm << 16;
            }
            if (op == 0xa){                         // slti
                if (*(reg+rs) < imm) *(reg+rt) = 1;
                else *(reg+rt) = 0;
            }
            if (op == 0xb){                         // sltiu
                if ((unsigned int)*(reg+rs) < (unsigned int)imm) *(reg+rt) = 1;
                else *(reg+rt) = 0;
            }
            if (op == 0x4){                         // beq
                if (*(reg+rs) == *(reg+rt)){
                    PC = PC + imm;
                }
            }
            if (op == 0x5){                         // bne
                if (*(reg+rs) != *(reg+rt)){
                    PC = PC + imm;
                }
            }
            if (op == 0x7){                         // bgtz
                if (*(reg+rs) > 0){
                    PC = PC + imm;
                }
            }
            if (op == 0x6){                         // blez
                if (*(reg+rs) <= 0){
                    PC = PC + imm;
                }
            }
            if (op == 0x1){                         // op = 1

                if (*(reg+rt) == 0x1){          //bgez
                    if (*(reg+rs) >= 0){
                        PC = PC + imm;
                    }
                }
                if (*(reg+rt) == 0x11){         //bgezal
                    if (*(reg+rs) >= 0){
                        *(reg+31) = ((long)PC)-offset;
                        PC = PC + imm;
                    }
                }
                if (*(reg+rt) == 0x0){          //bltz
                    if (*(reg+rs) < 0){
                        PC = PC + imm;
                    }
                }
                if (*(reg+rt) == 0x10){         //bltzal
                    if (*(reg+rs) < 0){
                        *(reg+31) = ((long)PC)-offset;
                        PC = PC + imm;
                    }
                }
                if (*(reg+rt) == 0xc){          // teqi
                    if (*(reg+rs) == imm){
                        cout << "Trap exception at " << (int*)((long)(PC-1)-offset) << endl;
                        loopflag = false;
                    }
                }
                if (*(reg+rt) == 0xe){          // tnei
                    if (*(reg+rs) != imm){
                        cout << "Trap exception at " << (int*)((long)(PC-1)-offset) << endl;
                        loopflag = false;
                    }
                }
                if (*(reg+rt) == 0x8){          // tgei
                    if (*(reg+rs) >= imm){
                        cout << "Trap exception at " << (int*)((long)(PC-1)-offset) << endl;
                        loopflag = false;
                    }
                }
                if (*(reg+rt) == 0x9){          // tgeiu
                    if ((unsigned int)*(reg+rs) >= (unsigned int)imm){
                        cout << "Trap exception at " << (int*)((long)(PC-1)-offset) << endl;
                        loopflag = false;
                    }
                }
                if (*(reg+rt) == 0xa){          // tlti
                    if (*(reg+rs) < imm){
                        cout << "Trap exception at " << (int*)((long)(PC-1)-offset) << endl;
                        loopflag = false;
                    }
                }
                if (*(reg+rt) == 0xb){          // tltiu
                    if ((unsigned int)*(reg+rs) < (unsigned int)imm){
                        cout << "Trap exception at " << (int*)((long)(PC-1)-offset) << endl;
                        loopflag = false;
                    }
                }
            }
            if (op == 0x20){                        // lb
                long addr = *(reg+rs) + imm + offset;
                char* ptr = (char*)(long)addr;
                *(reg+rt) = *ptr;
            }
            if (op == 0x24){                        // lbu
                long addr = (unsigned long)(*(reg+rs) + imm + offset);
                char* ptr = (char*)(long)addr;
                *(reg+rt) = *ptr;
            }
            if (op == 0x21){                        // lh
                long addr = *(reg+rs) + imm + offset;
                short* ptr = (short*)(long)addr;
                *(reg+rt) = *ptr;
            }
            if (op == 0x25){                        // lhu
                long addr = (unsigned long)(*(reg+rs) + imm + offset);
                short* ptr = (short*)(long)addr;
                *(reg+rt) = *ptr;
            }
            if (op == 0x23){                        // lw
                long addr = *(reg+rs) + imm + offset;
                int* ptr = (int*)(long)addr;
                *(reg+rt) = *ptr;
            }
            if (op == 0x22){                        // lwl
                long addr = *(reg+rs) + imm + offset;
                int num = addr % 4;
                int length = 4 - num;
                int* addrword = (int*)(long)(addr - num);
                int word = *addrword;
                int slice = word % (int)pow(2, 8*length);
                slice = slice * (int)pow(2, 8*num);
                int mult = (int)pow(2, 8*num) - 1;
                *(reg+rt) = slice | (*(reg+rt) & mult);
            }
            if (op == 0x26){                        // lwr
                long addr = *(reg+rs) + imm + offset;
                int num = addr % 4;
                int length = num + 1;
                int* addrword = (int*)(long)(addr - num);
                int word = *addrword;
                int slice = word / (int)pow(2, 8*(4-length));
                int mult = (unsigned int)((int)pow(2, 32)-1-((int)pow(2,8*length)-1));
                *(reg+rt) = slice | (*(reg+rt) & mult);
            }
            if (op == 0x30){                        // ll
                long addr = *(reg+rs) + imm + offset;
                int* ptr = (int*)(long)addr;
                *(reg+rt) = (long)*ptr;
            }
            if (op == 0x28){                        // sb
                char* chptr = (char*)(long)(reg+rt);
                char byte = *chptr;
                long addr = *(reg+rs) + imm + offset;
                *((char*)(long)addr) = byte;
            }
            if (op == 0x29){                        // sh
                short* shptr = (short*)(long)(reg+rt);
                short num = *shptr;
                long addr = *(reg+rs) + imm + offset;
                *((short*)(long)addr) = num;
            }
            if (op == 0x2b){                        // sw
                long addr = *(reg+rs) + imm + offset;
                int* ptr = (int*)(long)addr;
                *ptr = *(reg+rt);
            }
            if (op == 0x2a){                        // swl
                long addr = *(reg+rs) + imm + offset;
                int num = addr % 4;
                int length = 4 - num;
                int data = *(reg+rt);
                data = data / pow(2, 8*num);
                char* chptr = (char*)(long)addr;
                for (int i=0; i<length; i++){
                    *chptr = data % (int)pow(2, 8);
                    data = data / (int)pow(2, 8);
                    *chptr += 1;
                }
            }
            if (op == 0x2e){                        // swr
                long addr = *(reg+rs) + imm + offset;
                int num = addr % 4;
                addr -= num;
                int length = num +1;
                int data = *(reg+rt);
                data = data % (int)pow(2, 8*length);
                char* chptr = (char*)(long)addr;
                for (int i=0; i<length; i++){
                    *chptr = data % (int)pow(2, 8);
                    data = data / (int)pow(2, 8);
                    *chptr += 1;
                }
            }
            if (op == 0x38){                        // sc
                long addr = *(reg+rs) + imm + offset;
                int* ptr = (int*)(long)addr;
                *ptr = *(reg+rt);
            }
        }
    }
}

