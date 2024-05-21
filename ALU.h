#define ll long long

#ifndef ALU_H
#define ALU_H

struct ALUOutput {
    bool Zero, Neg;
    ll result;
};

class ALU{
private:
public:
    ll Addy(ll a, ll b){return a+b;}
    ll Suby(ll a, ll b){return a-b;}
    ll Andy(ll a, ll b){return a&b;}
    ll Ory(ll a, ll b){return a|b;}
    ll NOry(ll a, ll b){return ~(a|b);}
    ll SLL(ll a, ll b){return a<<b;}
    ll SRL(ll a, ll b){return a>>b;}
    ALU(){};
    ~ALU(){};
    ALUOutput ALU_Con(char ALUOp, char Funct, ll data1, ll data2);
};

#endif