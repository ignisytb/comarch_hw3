#include <iostream>
#include <fstream>
#include <string.h>
#include "Memory.h"
#include "Register.h"
#include "ALU.h"

using namespace std;

#ifndef CPU_H
#define CPU_H

struct Ops {
    int num_inst=0x0;
    bool atp=NULL, dop=false, pop=false, nop=false, mop=false;
    ll addr1=0x0, addr2=0x0;
};

class CPU {
private:
    int cys;
    Mem* dram;
    Reg* reg;
    ALU* alu;
    ll pc;
    Ops options;
    IF_ID II;
    ID_EX IE;
    EX_MEM EM;
    MEM_WB MW;
public:
    CPU(Ops ops){
        cys = 0;
        pc = 0x400000;
        dram = new Mem;
        reg = new Reg;
        alu = new ALU;
        options = ops;
    }
    ~CPU(){
        free(dram);
        free(reg);
    }
    bool Load_File(string file);
    ll Read_Mem(ll addr){
        return dram->Read(addr);
    };
    void clock();
    void cycle();
    void stall(int stage);
    ConSig ControlUnit(char opcode);
};


#endif