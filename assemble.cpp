#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#include <asm.h>

using namespace std;


const vector<string> regis = {                              // The index of each register name is the register number.
    "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",
    "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
    "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
    "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"
};

const vector<vector<string>> R_operation = {
    {"and", "24"}, {"div", "1a"}, {"divu", "1b"}, {"mult", "18"},
    {"multu", "19"}, {"nor", "27"}, {"or", "25"}, {"sll", "0"},
    {"sllv", "4"}, {"sra", "3"}, {"srav", "7"}, {"srl", "2"},
    {"srlv", "6"}, {"sub", "22"}, {"subu", "23"}, {"xor", "26"},
    {"slt", "2a"}, {"sltu", "2b"}, {"jalr", "9"}, {"jr", "8"},
    {"teq", "34"}, {"tne", "36"}, {"tge", "30"}, {"tgeu", "31"},
    {"tlt", "32"}, {"tltu", "33"}, {"mfhi", "10"}, {"mflo", "12"},
    {"mthi", "11"}, {"mtlo", "13"}, {"add", "20"}, {"addu", "21"},
    {"syscall", "c"}
};

const vector<vector<string>> I_operation = {
    {"andi", "c"}, {"ori", "d"}, {"xori", "e"}, {"lui", "f"},
    {"slti", "a"}, {"sltiu", "b"}, {"beq", "4"}, {"bgez", "1"},
    {"bgezal", "11"}, {"bgtz", "7"}, {"blez", "6"}, {"bltzal", "10"},
    {"bltz", "0"}, {"bne", "5"}, {"teqi", "c"}, {"tnei", "e"},
    {"tgei", "8"}, {"tgeiu", "9"}, {"tlti", "a"}, {"tltiu", "b"},
    {"lb", "20"}, {"lbu", "24"}, {"lh", "21"}, {"lhu", "25"},
    {"lw", "23"}, {"lwl", "22"}, {"lwr", "26"}, {"ll", "30"},
    {"sb", "28"}, {"sh", "29"}, {"sw", "2b"}, {"swl", "2a"},
    {"swr", "2e"}, {"sc", "38"}, {"addi", "8"}, {"addiu", "9"}
};

const vector<vector<string>> J_operation = {
    {"j", "2"}, {"jal", "3"}
};


vector<string> separate(string line){                   // Separate the useful information from a line if MIPS code
    string temp = "";                                   // and store them into a vector by sequence.
    vector<string> v;

    for (int i = 0; i < static_cast<int>(line.size()); i++){
        if (line[i] != ' ' && line[i] != ',' && line[i] != '\t') {
            temp += line[i];
            if (line[i] == ':') temp = "";
            if (line[i] == '#') return v;
        }
        else if (temp != "") {
            v.push_back(temp);
            temp = "";
        }
    }
    if (temp != "") v.push_back(temp);

    return v;
}


vector<string> datatoken(string line){                  // Separate a line of static data into tokens.
    string temp = "";
    vector<string> v;
    bool flag = false;

    for (int i = 0; i < static_cast<int>(line.size()); i++){
        if (line[i] == '"'){
            flag = not flag;
            if (flag && temp != ""){
                v.push_back(temp);
                temp = "";
            }
        }
        else if (((line[i] != ' ' && line[i] != ',') || flag) && line[i] != '\t'){
            temp += line[i];
            if (line[i] == '#') return v;
        }
        else if (temp != ""){
            v.push_back(temp);
            temp = "";
        }
    }
    if (temp != "") v.push_back(temp);

    return v;
}


string DtoB(long decnum, int digit){                    // Decimal to binary number with specified length.
    if (decnum < 0){
        decnum += pow(2, digit);
        return DtoB(decnum, digit);
    }
    else {
        string bin = "";
        while (decnum){
            if (decnum%2 == 1) bin = "1" + bin;
            else bin = "0" + bin;
            decnum /= 2;
        }
        for (int i = 0; digit > static_cast<int>(bin.size()); i++){
            bin = "0" + bin;
        }
        return bin;
    }
}


string HtoB(string hexnum, int digit){                  // Hexadecimal to binary number with specified length.
    int a;
    stringstream ss;

    ss << hex << hexnum;
    ss >> a;

    return DtoB(a, digit);
}


unsigned int BtoD(string binnum, int digit){            // Binary string into decimal integer.
    unsigned int total = 0;
    for (int i=0; i < digit; i++){
        total += (unsigned int)((((int)binnum[i]-48))*((int)pow(2, digit-1-i)));
    }
    return total;
}


int regFind(string reg){                                // Find the register number of the given register name.
    for (int i=0; i<32; i++){
        if (regis[i] == reg) return i;
    }

    return 0;
}


int strToInt(string s){                                 // Transform a string to an integer.
    int a;
    stringstream st;

    st << s;
    st >> a;

    return a;
}


int findPos(vector<tag> allLabels, string label, int PC){   // find the offset from the PC to the given label.
    for (int i = 0; i < static_cast<int>(allLabels.size()); i++){
        if (allLabels[i].label == label){
            return allLabels[i].pos - PC;
        }
    }

    return strToInt(label);
}


int addrSplit(string code, vector<string> &addr){       // Split the address num(reg) into a number and a register name.
    string::size_type pos1, pos2;

    pos1 = code.find("(");
    pos2 = code.find(")");
    addr.push_back(code.substr(0, pos1));
    addr.push_back(code.substr(pos1+1, pos2-pos1-1));

    return 0;
}


string opR(vector<string> code, string fval){
    string op, rs, rt, rd, shamt, funct;
    string mCode;

    op = "000000";

    if (code[0] == "add" || code[0] == "addu" || code[0] == "and" ||
        code[0] == "nor" || code[0] == "or" || code[0] == "sub" ||
        code[0] == "subu" || code[0] == "xor" || code[0] == "slt" ||
        code[0] == "sltu"){

        rs = DtoB(regFind(code[2]), 5);
        rt = DtoB(regFind(code[3]), 5);
        rd = DtoB(regFind(code[1]), 5);
        shamt = "00000";
        funct = HtoB(fval, 6);
    }

    if (code[0] == "div" || code[0] == "divu" || code[0] == "mult" ||
        code[0] == "multu" || code[0] == "teq" || code[0] == "tne" ||
        code[0] == "tge" || code[0] == "tgeu" || code[0] == "tlt" ||
        code[0] == "tltu"){

        rs = DtoB(regFind(code[1]), 5);
        rt = DtoB(regFind(code[2]), 5);
        rd = "00000";
        shamt = "00000";
        funct = HtoB(fval, 6);
    }

    if (code[0] == "sll" || code[0] == "sra" || code[0] == "srl"){
        rs = "00000";
        rt = DtoB(regFind(code[2]), 5);
        rd = DtoB(regFind(code[1]), 5);
        shamt = DtoB(strToInt(code[3]), 5);
        funct = HtoB(fval, 6);
    }

    if (code[0] == "sllv" || code[0] == "srav" || code[0] == "srlv"){
        rs = DtoB(regFind(code[3]), 5);
        rt = DtoB(regFind(code[2]), 5);
        rd = DtoB(regFind(code[1]), 5);
        shamt = "00000";
        funct = HtoB(fval, 6);
    }

    if (code[0] == "jalr"){
        rs = DtoB(regFind(code[1]), 5);
        rt = "00000";
        rd = DtoB(regFind(code[2]), 5);
        shamt = "00000";
        funct = HtoB(fval, 6);
    }

    if (code[0] == "jr" || code[0] == "mthi" || code[0] == "mtlo"){
        rs = DtoB(regFind(code[1]), 5);
        rt = "00000";
        rd = "00000";
        shamt = "00000";
        funct = HtoB(fval, 6);
    }

    if (code[0] == "mfhi" || code[0] == "mflo"){
        rs = "00000";
        rt = "00000";
        rd = DtoB(regFind(code[1]), 5);
        shamt = "00000";
        funct = HtoB(fval, 6);
    }

    if (code[0] == "syscall"){
        rs = "00000";
        rt = "00000";
        rd = "00000";
        shamt = "00000";
        funct = HtoB(fval, 6);
    }

    mCode = op + rs + rt + rd + shamt + funct;
    return mCode;
}


string opI(vector<string> code, string fval, vector<tag> allLabels, int PC){
    string op, rs, rt, imm;
    string mCode;

    if (code[0] == "addi" || code[0] == "addiu" || code[0] == "andi" ||
        code[0] == "ori" || code[0] == "xori" || code[0] == "slti" ||
        code[0] == "sltiu"){

        op = HtoB(fval, 6);
        rs = DtoB(regFind(code[2]), 5);
        rt = DtoB(regFind(code[1]), 5);
        imm = DtoB(strToInt(code[3]), 16);
    }

    if (code[0] == "lui"){
        op = HtoB(fval, 6);
        rs = "00000";
        rt = DtoB(regFind(code[1]), 5);
        imm = DtoB(strToInt(code[2]), 16);
    }

    if (code[0] == "beq" || code[0] == "bne"){
        op = HtoB(fval, 6);
        rs = DtoB(regFind(code[1]), 5);
        rt = DtoB(regFind(code[2]), 5);
        imm = DtoB(findPos(allLabels, code[3], PC), 16);
    }

    if (code[0] == "bgez" || code[0] == "bgezal" ||code[0] == "bltzal" ||
        code[0] == "bltz"){

        op = "000001";
        rs = DtoB(regFind(code[1]), 5);
        rt = HtoB(fval, 5);
        imm = DtoB((findPos(allLabels, code[2], PC)), 16);
    }

    if (code[0] == "bgtz" || code[0] == "blez"){
        op = HtoB(fval, 6);
        rs = DtoB(regFind(code[1]), 5);
        rt = "00000";
        imm = DtoB(findPos(allLabels, code[2], PC), 16);
    }

    if (code[0] == "teqi" || code[0] == "tnei" || code[0] == "tgei" ||
        code[0] == "tgeiu" || code[0] == "tlti" || code[0] == "tltiu"){

        op = "000001";
        rs = DtoB(regFind(code[1]), 5);
        rt = HtoB(fval, 5);
        imm = DtoB(strToInt(code[2]), 16);
    }

    if (code[0] == "lb" || code[0] == "lbu" || code[0] == "lh" ||
        code[0] == "lhu" || code[0] == "lw" || code[0] == "lwl" ||
        code[0] == "lwr" || code[0] == "ll" || code[0] == "sb" ||
        code[0] == "sh" || code[0] == "sw" || code[0] == "swl" ||
        code[0] == "swr" || code[0] == "sc"){

        vector<string> addr;

        addrSplit(code[2], addr);
        op = HtoB(fval, 6);
        rs = DtoB(regFind(addr[1]), 5);
        rt = DtoB(regFind(code[1]), 5);
        imm = DtoB(strToInt(addr[0]), 16);
    }

    mCode = op + rs + rt + imm;
    return mCode;
}


string opJ(vector<string> code, string fval, vector<tag> allLabels, int PC){
    string op, target;
    string mCode;

    op = HtoB(fval, 6);
    target = DtoB(findPos(allLabels, code[1], PC)+PC, 26);

    mCode = op + target;
    return mCode;
}


string assemble(vector<string> v, vector<tag> allLabels, int PC){
    for (auto i:R_operation){
        if (v[0] == i[0]) {
            return opR(v, i[1]);
        }
    }

    for (auto i:I_operation){
        if (v[0] == i[0]) {
            return opI(v, i[1], allLabels, PC);
        }
    }

    for (auto i:J_operation){
        if (v[0] == i[0]) {
            return opJ(v, i[1], allLabels, PC);
        }
    }

    return 0;
}
