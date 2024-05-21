#include "Memory.h"

void Mem::WriteByte(ll addr, char data) {
    int idx;
    for(idx = 0; idx < AddressSpace.size(); idx++) {
        if(AddressSpace[idx] == addr) {
            if (data == 0) {
                MemorySpace.erase(MemorySpace.begin()+idx);
                AddressSpace.erase(AddressSpace.begin()+idx);
            } else {
                MemorySpace[idx] = data;
            }
            return;
        }
    }
    if(data != 0){
        MemorySpace.push_back(data);
        AddressSpace.push_back(addr);
    }
}

char Mem::ReadByte(ll addr) {
    for(int i=0;i<AddressSpace.size();i++){
        if(AddressSpace[i]==addr){
            return MemorySpace[i];
        }
    }
    return 0x00;
}

void Mem::Write(ll addr, ll data){
    for(int i=0;i<4;i++){
        WriteByte(addr+i,(data >> (8*(3-i)))&0xFF);
    }
}

ll Mem::Read(ll addr){
    ll res = 0;
    for(int i=0;i<4;i++){
        res = res << 8;
        res += ReadByte(addr+i) & 0xFF;
    }
    return res;
}