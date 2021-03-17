#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <asm.h>

using namespace std;


int * readtext(int* txtptr, int* stcptr)
{
    string str;             // Used to store a line of MIPS code.
    vector<tag> allLabels;
    int PC = 0;
    string inputname, outputname;

    ifstream fin;
    cout << "Please enter the input file name >";
    cin >> inputname;
    fin.open(inputname, ios::in);
    if (!fin.is_open()){
        cout << "Unable to open." << endl;
        exit(0);
    }

    while (getline(fin, str)){                  // Processing the static data
        if (str.substr(0, 5) == ".text") break;
        if (str.substr(0, 5) == ".data") continue;

        vector<string> info = datatoken(str);   // Split the data into tokens. The function is in assemble.cpp
        if (info.empty()) continue;

        if (info[1] == ".ascii"){               // ascii
            char* chptr = (char*)stcptr;
            for (char ch:info[2]){
                *chptr = ch;
                chptr += 1;
            }
            while ((long)stcptr < (long)chptr){
                stcptr += 1;
            }
        }

        if (info[1] == ".asciiz"){              // asciiz
            char* chptr = (char*)stcptr;
            for (char ch:info[2]){
                *chptr = ch;
                chptr += 1;
            }
            *chptr = '\0';
            while ((long)stcptr <= (long)chptr){
                stcptr += 1;
            }
        }

        if (info[1] == ".word"){                // word
            for (int i = 2; i < static_cast<int>(info.size()); i++){
                *stcptr = strToInt(info[i]);
                stcptr += 1;
            }
        }

        if (info[1] == ".half"){                // half
            short* shptr = (short*)stcptr;
            for (int i = 2; i < static_cast<int>(info.size()); i++){
                *shptr = strToInt(info[i]);
                shptr += 1;
            }
            while ((long)stcptr < (long)shptr){
                stcptr += 1;
            }
        }

        if (info[1] == ".byte"){                // byte
            char* chptr = (char*)stcptr;
            for (int i = 2; i < static_cast<int>(info.size()); i++){
                *chptr = (char)(strToInt(info[i]));
                chptr += 1;
            }
            while ((long)stcptr < (long)chptr){
                stcptr += 1;
            }
        }
    }



    string separator = ":";

    while (getline(fin, str)){      // Scan the file for the first time.
        bool haveDigit = false;
        bool haveColon = false;

        for (int i = 0; i < static_cast<int>(str.size()); i++){
            if (str[i] == '#') break;
            if (isalnum(str[i])) haveDigit = true;
            if (str[i] == ':') {
                haveColon = true;
                haveDigit = false;
            }
        }
        if (haveDigit) PC += 1;     // Refresh the program counter when there is a valid line of code.
        if (haveColon) {            // When there is a label, update the label and position.
            string cutStr;
            string::size_type posSeparator;

            posSeparator = str.find(separator);
            if (string::npos != posSeparator){
                int codePos;

                cutStr = str.substr(0, posSeparator);
                if (haveDigit) codePos = PC - 1;
                else codePos = PC;
                allLabels.push_back({cutStr, codePos});
            }
        }
    }

    fin.clear();                    // Reset the pointer of the input file.
    fin.seekg(0, ios::beg);
    PC = 0;                         // Reset the PC.
    bool txtflag = false;

    while (getline(fin, str)){      // Scan the file for the second time.
        if (str.substr(0, 5) == ".text") {
            txtflag = true;
            continue;
        }
        if (not txtflag) continue;

        bool haveDigit = false;

        for (int i = 0; i < static_cast<int>(str.size()); i++){
            if (isalnum(str[i])) haveDigit = true;
            if (str[i] == ':') haveDigit = false;
            if (str[i] == '#') break;
        }
        if (not haveDigit) continue;
        PC += 1;

        vector<string> info{separate(str)};     //Separate the line of code into operation, registers, offset, etc.

        string binCode;
        unsigned int decnum;

        binCode = assemble(info, allLabels, PC);    // Assemble the line of code.
        decnum = BtoD(binCode, 32);             // Convert the code into a decimal integer. The function is in assemble.cpp

        *txtptr = decnum;
        txtptr += 1;
    }

    fin.close();

    return stcptr;
}
