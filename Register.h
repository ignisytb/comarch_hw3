#define ll long long

#include <stdlib.h>
#include "ALU.h"

#ifndef Reg_H
#define Reg_H

struct ConSig {
    char ALUOp;
    bool Branch, BN, MEMRead, MEMtoReg, MemWrite, ALUSrc, RegWrite, RegDst, RWByte, ALUNeg, Jump;
};


struct IF_ID {
    ll instr=0, NPC=0, pc=0;
};

struct ID_EX {
    ll pc=0, NPC=0, data1=0, data2=0;
    long IMM=0;
    char rs=0, rt=0, rd=0, Funct=0;
    ConSig CS;
};

struct EX_MEM {
    ll pc=0, data2=0;
    long BR_TARGET=0;
    char rd=0;
    ALUOutput ALU_OUT;
    ConSig CS;
};

struct MEM_WB {
    ll pc=0, MEM_OUT=0;
    long ALU_OUT=0;
    char rd=0;
    ConSig CS;
};

class Reg{
private:
    ll* RegData;
public:
    Reg();
    ~Reg(){};
    void Write(char addr, ll data);
    ll Read(char addr);
};

#endif