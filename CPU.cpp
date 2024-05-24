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

void CPU::clock(){

    bool Frs=0,Frt=0,stall=0,flush=0,branch=0,pred_miss=0;

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
                    IE.data1 = EM.ALU_OUT.result;
                }
            } else if (EM.CS.RegWrite && (EM.rd != 0) && (EM.rd == IE.rt)){
                Frt = true;
                if(EM.CS.ALUNeg){
                    IE.data2 = EM.ALU_OUT.Neg;
                } else {
                    IE.data2 = EM.ALU_OUT.result;
                }
            }
        }

        if(EM.CS.Branch){
            // Branch Prediction, Jump
            if(EM.CS.BN != EM.ALU_OUT.Zero){
                branch = true;
            }
            if(branch != options.atp){
                pred_miss = true;
                if(options.atp){
                    pc = EM.pc + 4;
                } else {
                    pc = EM.BR_TARGET;
                }
            }
        } else {
            if(!pred_miss){
                if(EM.CS.MemWrite){
                    if(EM.CS.RWByte){
                        dram->WriteByte(EM.ALU_OUT.result, EM.data2&0xFF);
                    } else {
                        dram->Write(EM.ALU_OUT.result, EM.data2);
                    }
                } else if(EM.CS.MEMRead){
                    if(EM.CS.RWByte){
                        MW.MEM_OUT=dram->ReadByte(EM.ALU_OUT.result);
                    } else {
                        MW.MEM_OUT=dram->Read(EM.ALU_OUT.result);
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
            } else {
                MW = *(new MEM_WB);
            }
        }
    }
    MW.CS = EM.CS;
    MW.pc = EM.pc;

    // EX stage
    if(IE.pc!=0){
        if(!pred_miss){
            EM.BR_TARGET = (IE.IMM << 2) + IE.NPC;
            if(IE.CS.ALUSrc){
                EM.ALU_OUT = alu->ALU_Con(IE.CS.ALUOp, IE.Funct, IE.data1, IE.IMM);
            } else {
                EM.ALU_OUT = alu->ALU_Con(IE.CS.ALUOp, IE.Funct, IE.data1, IE.data2);
            }
            EM.data2 = IE.data2;
            if(IE.CS.RegDst){
                EM.rd = IE.rd;
            } else {
                EM.rd = IE.rt;
            }
        } else {
            EM = *(new EX_MEM);
        }
    }
    EM.CS = IE.CS;
    EM.pc = IE.pc;

    // ID stage
    char rs = (II.instr >> 21) & 0x1F;
    char rt = (II.instr >> 16) & 0x1F;
    ll target {II.instr & 0x3FFFFFF};
    target = ((target << 2)&0x0FFFFFFF)+(II.pc&0xF0000000);

    if(IE.CS.MemWrite && (IE.rt == rs || IE.rt == rt)){
        stall = true;
    }

    ConSig CS = ControlUnit((II.instr >> 26) & 0x3F);
    if (!stall) {
        if(II.pc!=0){
            IE.NPC = II.NPC;
            IE.rs = (II.instr >> 21) & 0x1F;
            IE.rt = (II.instr >> 16) & 0x1F;
            IE.rd = (II.instr >> 11) & 0x1F;
            IE.Funct = II.instr & 0x3F;
            IE.IMM = II.instr & 0xFFFF;
            if(!Frs){
                IE.data1 = reg->Read(IE.rs);
            }
            if(!Frt){
                IE.data2 = reg->Read(IE.rt);
            }
        }
        IE.CS = CS;
        IE.pc = II.pc;

        if(CS.Jump && II.pc != 0){
            switch((II.instr >> 26) & 0x3F){
                case 0:
                if(IE.Funct == 8){
                    flush = true;
                    IE.CS.RegWrite = 0;
                    pc = IE.data1;
                }
                break;

                case 2:
                flush = true;
                IE.CS.RegWrite = 0;
                pc = target;
                break;

                case 3:
                flush = true;
                IE.rt = 31;
                IE.data1 = II.NPC;
                IE.data2 = 0;
                IE.CS.ALUOp = 0b010;
                IE.CS.ALUSrc = 0;
                pc = target;
                break;
            }
        }

        if(CS.Branch && options.atp){
            flush = true;
            IE.CS.RegWrite = 0;
            pc = (IE.IMM << 2) + IE.NPC;
        }

        if(pred_miss){
            IE = *(new ID_EX);
        }
    } else {
        IE = *(new ID_EX);
    }

    // IF stage
    if (!stall && !(flush||pred_miss)){
        ll inst = dram->Read(pc);
        if (inst!=0){
            ll npc = pc + 4;
            II.instr = inst;
            II.NPC = npc;
            II.pc = pc;
        } else {
            if(pc != 0){
                fin_pc = pc;
            }
            II = *(new IF_ID);
        }
    } else if (flush||pred_miss) {
        II = *(new IF_ID);
    }

    // Branch, Jump
    if (!(flush||pred_miss)){
        pc = II.NPC;
    }

    if(dram->Read(pc)==0x0 && pc != 0x0){
        fin_pc = pc;
        pc = 0x0;
    }
}

ConSig CPU::ControlUnit(char opcode){
    ConSig ret;
    bool ops[6]={0,};

    for(int i=0;i<6;i++){
        ops[i] = (opcode >> (i)) & 0x1;
    }
    
    ret.Branch = ops[2] && !ops[3];
    ret.MEMRead = ops[5] && !ops[3];
    ret.MEMtoReg = ops[5] && !ops[3];
    ret.MemWrite = ops[5] && ops[3];
    ret.ALUSrc = !(opcode==0 || (!ops[1] && ops[2] && !ops[3] && !ops[4] && !ops[5]));
    ret.RegWrite = !((ops[5] && ops[3]) || (ops[2] && !ops[3]));
    ret.RegDst = !(ops[0]||ops[1]||ops[2]||ops[3]||ops[4]||ops[5]);
    ret.ALUNeg = (opcode==0b001011);
    ret.Jump = !ops[2]&&!ops[3]&&!ops[4]&&!ops[5];

    if(ret.Branch){
        ret.BN = ops[0];
    }
    if(ret.MEMRead || ret.MemWrite){
        ret.RWByte = !ops[1];
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

bool CPU::cycle(bool fin) {

    cys ++;
    
    if (options.dop || options.pop || options.mop){
        if (!fin){
            cout << "===== Cycle " << dec << cys << " =====\n";
        }
    }
    if (fin){
        cout << "===== Completion cycle: " << dec << cys << " =====\n";
    }
    
    if (options.pop || fin){
        cout << "Current pipeline PC state : \n";
        cout << "{";
        if(pc != 0x0){
            if(pc == 0){
                cout << "0x" << hex << fin_pc;
            } else {
                cout << "0x" << hex << pc;
            }
        }
        cout << "|";
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

    if (options.dop || fin){
        cout << "Current register values :\n-------------------------------------------\n";
        if(pc == 0){
            cout << "PC: 0x" << hex << fin_pc << endl;
        } else {
            cout << "PC: 0x" << hex << pc << endl;
        }
    }

    clock();
    
    if (options.dop || fin){
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

    if (pc == 0 && II.pc == 0 && IE.pc == 0 && EM.pc == 0 && MW.pc == 0){
        return false;
    }
    return true;

}