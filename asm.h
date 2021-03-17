#ifndef ASM_H
#define ASM_H

struct tag{
    std::string label;
    int pos;
};

std::vector<std::string> separate(std::string);

std::vector<std::string> datatoken(std::string);

std::string assemble(std::vector<std::string>, std::vector<tag>, int);

int strToInt(std::string);

std::string DtoB(long, int);

unsigned int BtoD(std::string, int);

#endif // ASM_H
