#include "ALU.h"

ALUOutput ALU::ALU_Con(char ALUOp, char Funct, ll data1, ll data2){
    ALUOutput ret;
    switch(ALUOp) {
        case 0b000:
        break;

        case 0b001:
        break;
        
        case 0b010:
        ret.result = Addy(data1, data2);
        break;
        
        case 0b011:
        ret.result = Andy(data1,data2);
        break;
        
        case 0b100:
        break;
        
        case 0b101:
        ret.result = SLL(data2,16);
        break;
        
        case 0b110:
        ret.result = Ory(data1,data2);
        break;
        
        case 0b111:
        ret.result = Suby(data1,data2);
        break;
    }
    ret.Zero = (ret.result == 0);
    ret.Neg = (ret.result < 0);

    return ret;
}