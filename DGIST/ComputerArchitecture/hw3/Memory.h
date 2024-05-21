#define ll long long

#include <iostream>
#include <vector>

using namespace std;

#ifndef Mem_H
#define Mem_H

class Mem{
private:
    std::vector<char> MemorySpace;
    std::vector<ll> AddressSpace;
public:
    Mem(){}
    ~Mem(){};
    char ReadByte(ll address);
    void WriteByte(ll address, char data);
    ll Read(ll address);
    void Write(ll address, ll data);
};

#endif