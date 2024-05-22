#include "CPU.h"

bool CPU::Load_File(string filename){
    ifstream fin(filename);

    string fin_data;
    ll size_text, size_data;
    
    getline(fin,fin_data);
    size_text=stoll(fin_data,0,16) >> 2;
    getline(fin,fin_data);
    size_data=stoll(fin_data,0,16) >> 2;

    for(int i=0;i<size_text;i++){
        getline(fin, fin_data);
        if(fin_data.substr(0,2)=="0x"){
            dram->Write(0x400000+(4*i),stoll(fin_data,0,16));
        }
    }
    for(int i=0;i<size_data;i++){
        getline(fin, fin_data);
        if(fin_data.substr(0,2)=="0x"){
            dram->Write(0x10000000+(4*i),stoll(fin_data,0,16));
        }
    }

    return true;
}

void CPU::stall(int stage){

}

void CPU::clock(){

    bool Frs=0,Frt=0;

    // WB stage
    if (MW.pc != 0){
        if(MW.CS.RegWrite){
            if(MW.CS.RegWrite && (MW.rd != 0) && (EM.rd != IE.rs) && (MW.rd == IE.rs)){
                Frs = true;
                if(MW.CS.MEMtoReg){
                    IE.data1 = MW.MEM_OUT;
                    reg->Write(MW.rd,MW.MEM_OUT);
                } else {
                    IE.data1 = MW.ALU_OUT;
                    reg->Write(MW.rd,MW.ALU_OUT);
                }
            } else if(MW.CS.RegWrite && (MW.rd != 0) && (EM.rd != IE.rt) && (MW.rd == IE.rt)){
                Frt = true;
                if(MW.CS.MEMtoReg){     
                    IE.data2 = MW.MEM_OUT;
                    reg->Write(MW.rd,MW.MEM_OUT);
                } else {
                    IE.data2 = MW.ALU_OUT;
                    reg->Write(MW.rd,MW.ALU_OUT);
                }
            } else {
                if(MW.CS.MEMtoReg){
                    reg->Write(MW.rd,MW.MEM_OUT);
                } else {
                    reg->Write(MW.rd,MW.ALU_OUT);
                }
            }
        }
    }

    // MEM stage
    if(EM.pc!=0){
        if(EM.CS.RegWrite){
            if(EM.CS.RegWrite && (EM.rd != 0) && (EM.rd==IE.rs)){
                Frs = true;
                if(EM.CS.ALUNeg){
                    IE.data1 = EM.ALU_OUT.Neg;
                } else {
                    cout << "Frs" << endl;
                    IE.data1 = EM.ALU_OUT.result;
                }
            } else if (EM.CS.RegWrite && (EM.rd != 0) && (EM.rd == IE.rt)){
                Frt = true;
                if(EM.CS.ALUNeg){
                    IE.data2 = EM.ALU_OUT.Neg;
                } else {
                    cout << "Frt" << endl;
                    IE.data2 = EM.ALU_OUT.result;
                }
            }
        }

        if(EM.CS.Branch){
            // Branch Prediction, Jump
        } else {
            if(EM.CS.MemWrite){
                if(EM.CS.RWByte){
                    dram->WriteByte(EM.data2, EM.ALU_OUT.result&0xFF);
                } else {
                    dram->Write(EM.data2, EM.ALU_OUT.result);
                }
            } else if(EM.CS.MEMRead){
                if(EM.CS.RWByte){
                    MW.MEM_OUT=dram->ReadByte(EM.data2);
                } else {
                    MW.MEM_OUT=dram->Read(EM.data2);
                }
            } else if(Frt || Frs) {
                if(EM.CS.ALUNeg){
                    MW.ALU_OUT = EM.ALU_OUT.Neg;
                } else {
                    MW.ALU_OUT = EM.ALU_OUT.result;
                }
            } else {
                if(EM.CS.RegWrite){
                    if(EM.CS.ALUNeg){
                        MW.ALU_OUT = EM.ALU_OUT.Neg;
                    } else {
                        MW.ALU_OUT = EM.ALU_OUT.result;
                    }
                } else {
                    MW.pc = 0;
                }
            }

            MW.rd = EM.rd;
        }
        MW.CS = EM.CS;
        MW.pc = EM.pc;
    }

    // EX stage
    if(IE.pc!=0){
        EM.BR_TARGET = (IE.IMM << 2) + IE.NPC;
        if(IE.CS.ALUSrc){
            EM.ALU_OUT = alu->ALU_Con(IE.CS.ALUOp, IE.Funct, IE.data1, IE.IMM);
        } else {
            EM.ALU_OUT = alu->ALU_Con(IE.CS.ALUOp, IE.Funct, IE.data1, IE.data2);
            cout << dec << EM.ALU_OUT.result << endl;
        }
        EM.data2 = IE.data2;
        if(IE.CS.RegDst){
            EM.rd = IE.rd;
        } else {
            EM.rd = IE.rt;
        }
        EM.CS = IE.CS;
        EM.pc = IE.pc;
    }

    // ID stage
    if(II.pc!=0){
        ConSig CS = ControlUnit((II.instr >> 26) & 0x3F);
        IE.NPC = II.NPC;
        IE.rs = (II.instr >> 21) & 0x1F;
        IE.rt = (II.instr >> 16) & 0x1F;
        IE.Funct = II.instr & 0x3F;
        IE.IMM = II.instr & 0xFFFF;
        if(!Frs){
            IE.data1 = reg->Read(IE.rs);
        }
        if(!Frt){
            IE.data2 = reg->Read(IE.rt);
        }
        IE.CS = CS;
        IE.pc = II.pc;
    }

    // IF stage
    if(pc != 0){
        ll inst = dram->Read(pc);
        ll npc = pc + 4;
        II.instr = inst;
        II.NPC = npc;
        II.pc = pc;
    }

    pc = II.NPC;


    // Branch, Jump
}

ConSig CPU::ControlUnit(char opcode){
    ConSig ret;
    bool ops[6]={0,};

    for(int i=0;i<6;i++){
        ops[i] = (opcode >> (i)) & 0x1;
    }
    cout << ops[5] << ops[4] << ops[3] << ops[2] << ops[1] << ops[0] << endl;
    
    ret.Branch = ops[2] && !ops[3];
    ret.MEMRead = ops[5] && !ops[3];
    ret.MEMtoReg = ops[5] && !ops[3];
    ret.MemWrite = ops[5] && ops[3];
    ret.ALUSrc = ops[5] || (ops[0] || ops[2]);
    ret.RegWrite = !((ops[5] && ops[3]) || (ops[2] && !ops[3]));
    ret.RegDst = !(ops[0]||ops[1]||ops[2]||ops[3]||ops[4]||ops[5]);
    ret.ALUNeg = (opcode==0b001011);

    if(ret.Branch){
        ret.BN = ops[0];
    }
    if(ret.MEMRead || ret.MemWrite){
        ret.RWByte = ~ops[1];
    }

    if(opcode == 0x00){
        ret.ALUOp = 0b000;
    } else if (ops[5]){
        ret.ALUOp = 0b001;
    } else {
        char lop = ops[0]+(ops[1] << 1)+(ops[2] << 2)+(ops[3] << 3);
        switch(lop){
            case 0b1001:
            ret.ALUOp = 0b010;
            break;
            case 0b1100:
            ret.ALUOp = 0b011;
            break;
            case 0b0100:
            ret.ALUOp = 0b100;
            break;
            case 0b0101:
            ret.ALUOp = 0b100;
            break;
            case 0b1111:
            ret.ALUOp = 0b101;
            break;
            case 0b1101:
            ret.ALUOp = 0b110;
            break;
            case 0b1011:
            ret.ALUOp = 0b111;
            break;
        }
    }

    return ret;
}

void CPU::cycle() {

    cys ++;
    
    if (options.dop || options.pop || options.mop){
        cout << "===== Cycle " << cys << " =====\n";
    }
    
    if (options.pop){
        cout << "Current pipeline PC state : \n";
        cout << "{";
        cout << "0x" << hex << pc << "|";
        if(II.pc != 0x0){
            cout << "0x" << hex << II.pc;
        }
        cout << "|";
        if(IE.pc != 0x0){
            cout << "0x" << hex << IE.pc;
        }
        cout << "|";
        if(EM.pc != 0x0){
            cout << "0x" << hex << EM.pc;
        }
        cout << "|";
        if(MW.pc != 0x0){
            cout << "0x" << hex << MW.pc;
        }
        cout << "}\n\n";
    }

    if (options.dop){
        cout << "Current register values :\n-------------------------------------------\n";
        cout << "PC: 0x" << hex << pc << endl;
        cout << "Registers:" << endl;
        for(int i=0;i<32;i++){
            cout << "R" << dec << i << ": 0x" << hex << reg->Read(i) << endl;
        }
        cout << "\n";
    }

    if (options.mop){
        cout << "Memory content [0x" << hex << options.addr1 << ".." << options.addr2 << "] : " << endl;
        cout << "-------------------------------------------\n";
        for(int i=options.addr1;i<=options.addr2;i+=4){
            cout << "0x" << hex << i << ": 0x" << dram->Read(i) << endl;
        }
        cout << "\n";
    }

    clock();
}