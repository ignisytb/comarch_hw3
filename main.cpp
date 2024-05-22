#include "CPU.h"

int main(int argc, char* argv[]) {
    int arg_i = 1;
    Ops ops;

    string filename = "";

    if(argv[arg_i]!=NULL){
        while(argv[arg_i]!=NULL && argv[arg_i][0]=='-'){
            string str(argv[arg_i]);
            str = str.substr(1,str.length());

            if(str == "atp"){
                ops.atp = true;
                arg_i++;
                continue;
            }
            if(str == "antp"){
                ops.atp = false;
                arg_i++;
                continue;
            }
            if(str == "m"){
                ops.mop = true;
                string ads(argv[arg_i+1]);
                for(int i=0;i<ads.length();i++){
                    if(ads[i]==':'){
                        ops.addr1 = stoll(ads.substr(0,i),0,16);
                        ops.addr2 = stoll(ads.substr(i+1,ads.length()),0,16);
                        cout << ads.substr(0,i) << ":" << ads.substr(i+1,ads.length()) << endl;
                        cout << ops.addr1 << ":" << ops.addr2 << endl;
                    }
                }
                arg_i += 2;
                continue;
            }
            if(str == "d"){
                ops.dop = true;
                arg_i++;
                continue;
            }
            if(str == "p"){
                ops.pop = true;
                arg_i++;
                continue;
            }
            if(str == "n"){
                ops.nop = true;
                ops.num_inst = stoi(argv[arg_i+1]);
                arg_i += 2;
                continue;
            }
        }
    }

    if(ops.atp == NULL){
        cout << "[Main] An Essential Option [ATP/ANTP] is missed." << endl;
        return 0;
    }


    if(argv[arg_i]==NULL){
        cout << "[Main] Filename is missed." << endl;
        return 0;
    }
    filename = argv[arg_i];

    CPU* cpu = new CPU(ops);
    cpu->Load_File(filename);
    
    // char opcodes = 0x09;
    // ConSig cs = cpu->ControlUnit(opcodes);
    // cout << cs.Branch << endl;

    // cout << cpu->Read_Mem(0x400000) << endl;

    if(ops.nop){
        for(int i=0;i<ops.num_inst;i++){
            cpu->cycle();
        }
    }



    return 0;
}