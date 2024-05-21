#include "Register.h"

Reg::Reg(){
    RegData = (ll*) malloc(sizeof(ll)*32);
    for(int i=0;i<32;i++){
        RegData[i] = 0x00;
    }
}

void Reg::Write(char addr, ll data){
    RegData[addr] = data;
}

ll Reg::Read(char addr) {
    return RegData[addr];
}