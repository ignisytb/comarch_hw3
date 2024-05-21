#include <fstream>
#include <iostream>
#include <string.h>
#include <algorithm>
#include <bitset>
#include <string>

using namespace std;

string R_inst[9]={"addu", "and", "jr", "nor", "or", "sltu", "sll", "srl", "subu"};
string I_inst[11]={"addiu", "andi", "beq", "bne", "lui", "lw", "ori", "sltiu", "sw", "lb", "sb"};
string J_inst[2]={"j","jal"};

string* label_li;
int* label_li_num;


int linenum_data(std::string data){
    int num = 1;
    for(int i=0;i<data.length();i++){
        if(data[i]=='\n'){
            num += 1;
        }
    }
    return num;
}

unsigned long long imm_stoi(string imm){
    unsigned long long res;
    if(imm.length()>2){
        if(imm.substr(0,2)=="0x"){
            res = stoll(imm.substr(2,imm.length()-2),0,16);
        } else {
            res = stoll(imm);
        }
    } else {
        res = stoll(imm);
    }
    return res;
}

class Rinstruction {
    private:
    int op;
    int rs;
    int rt;
    int rd;
    int shamt;
    int funct;
    public:
    Rinstruction(string operands, string params);
    ~Rinstruction();
    void print(string fout);
};

Rinstruction::Rinstruction(string operands, string params){
    string rs_s="";
    string rt_s="";
    string rd_s="";

    if(operands=="jr"){
        for(int i=0;i<params.length();i++){
            if(params[i]!='$'){
                rs_s+=params[i];
            }
        }
        op=0x0;
        rs=stoi(rs_s);
        rt=0x0;
        rd=0x0;
        shamt=0x0;
        funct=0x8;
    } else {
        int enu = 0;
        int i;
        for(int i=0;i<params.length();i++){
            if(params[i]==','){
                enu++;
            } else {
                switch (enu)
                {
                case 0:
                if(params[i]!='$'){
                rd_s += params[i];
                }
                break;
                case 1:
                if(params[i]!='$'){
                rs_s += params[i];
                }
                break;
                case 2:
                if(params[i]!='$'){
                rt_s += params[i];
                }
                break;
                }
            }
        }
        if(operands=="sll" || operands=="srl"){
            rd=stoi(rd_s);
            rt=stoi(rs_s);
            shamt=stoi(rt_s);
        } else {
            rd=stoi(rd_s);
            rs=stoi(rs_s);
            rt=stoi(rt_s);
        }
        for(i=0;i<9;i++){
            if(R_inst[i]==operands){break;}
        }
        switch (i)
        {
        case 0:
        op = 0x0;
        shamt = 0x0;
        funct = 0x21;
        break;

        case 1:
        op = 0x0;
        shamt = 0x0;
        funct = 0x24;
        break;

        case 3:
        op = 0x0;
        shamt = 0x0;
        funct = 0x27;
        break;

        case 4:
        op = 0x0;
        shamt = 0x0;
        funct = 0x25;
        break;

        case 5:
        op = 0x0;
        shamt = 0x0;
        funct = 0x2b;
        break;

        case 6:
        op = 0x0;
        rs = 0x0;
        funct = 0x0;
        break;

        case 7:
        op = 0x0;
        rs = 0x0;
        funct = 0x2;
        break;

        case 8:
        op = 0x0;
        shamt = 0x0;
        funct = 0x23;
        break;
        }
    }
}

void Rinstruction::print(string fout) {
    ofstream fileout(fout,ios::app);
    int res = 0;
    res += (op&0x3f) << 26;
    res += (rs&0x1f) << 21;
    res += (rt&0x1f) << 16;
    res += (rd&0x1f) << 11;
    res += (shamt&0x1f) << 6;
    res += (funct&0x3f);
    fileout << "0x" << hex << res << endl;
}

class Iinstruction {
    private:
    int op;
    int rs;
    int rt;
    int immediate;
    public:
    Iinstruction(string operands, string params, int pc);
    ~Iinstruction();
    void print(string fout);
};

Iinstruction::Iinstruction(string operands, string params, int pc){
    string rs_s="";
    string rt_s="";
    string imm="";

    if(operands=="addiu"||operands=="andi"||operands=="ori"||operands=="sltiu"||operands=="beq"||operands=="bne"){
        int i=0;
        int enu = 0;
        for(int i=0;i<params.length();i++){
            if(params[i]==','){
                enu++;
            } else {
                switch (enu)
                {
                case 0:
                if(params[i]!='$'){
                rt_s += params[i];
                }
                break;
                case 1:
                if(params[i]!='$'){
                rs_s += params[i];
                }
                break;
                case 2:
                imm += params[i];
                break;
                }
            }
        }
        if(operands=="addiu"||operands=="andi"||operands=="ori"||operands=="sltiu"){
            rs=stoi(rs_s);
            rt=stoi(rt_s);
            immediate=imm_stoi(imm);
        } else {
            int idx=0;
            while(label_li[idx]!=imm){
                idx++;
            }
            rs=stoi(rt_s);
            rt=stoi(rs_s);
            immediate=(label_li_num[idx]-4-pc)>>2;
        }
        for(i=0;i<9;i++){
            if(I_inst[i]==operands){break;}
        }
        switch (i)
        {
            case 0:
            op=0x9; break;
            case 1:
            op=0xc; break;
            case 2:
            op=0x4; break;
            case 3:
            op=0x5; break;
            case 6:
            op=0xd; break;
            case 7:
            op=0xb; break;
        }
    }
    if(operands=="lui"){
        int enu = 0;
        for(int i=0;i<params.length();i++){
            if(params[i]==','){
                enu++;
            } else {
                switch (enu)
                {
                case 0:
                if(params[i]!='$'){
                rt_s += params[i];
                }
                break;
                case 1:
                imm += params[i];
                break;
                }
            }
        }
        op=0xf;rs=0x0;rt=stoi(rt_s);immediate=imm_stoi(imm);
    }
    if(operands=="lw"||operands=="lb"||operands=="sw"||operands=="sb"){
        int i;
        int enu = 0;
        for(int i=0;i<params.length();i++){
            if(params[i]==','){
                enu++;
            } else if (params[i]=='('||params[i]==')') {
                enu++;
            } else {
                switch (enu)
                {
                case 0:
                if(params[i]!='$'){
                    rt_s += params[i];
                }
                break;
                case 1:
                imm += params[i];
                break;
                case 2:
                if(params[i]!='$'){
                    rs_s += params[i];
                }
                break;
                }
            }
        }
        rs=stoi(rs_s);rt=stoi(rt_s);immediate=imm_stoi(imm);
        for(i=0;i<10;i++){
            if(I_inst[i]==operands){break;}
        }
        switch (i)
        {
            case 5:
            op=0x23; break;
            case 8:
            op=0x2b; break;
            case 9:
            op=0x20; break;
            case 10:
            op=0x28; break;
        }
    }
}

void Iinstruction::print(string fout) {
    ofstream fileout(fout,ios::app);
    int res = 0;
    res += (op&0x3f) << 26;
    res += (rs&0x1f) << 21;
    res += (rt&0x1f) << 16;
    res += immediate&0xffff;
    fileout << "0x" << hex << res << endl;
}

class Jinstruction {
    private:
    int op;
    int jump;
    public:
    Jinstruction(string operands, string params);
    ~Jinstruction();
    void print(string fout);
};

Jinstruction::Jinstruction(string operands, string params){
    int idx=0;
    while(label_li[idx]!=params){idx++;}
    jump=label_li_num[idx] >> 2;
    if(operands=="j"){op=0x2;}
    else if(operands=="jal"){op=0x3;}
}

void Jinstruction::print(string fout) {
    ofstream fileout(fout,ios::app);
    int res = 0;
    res += (op&0x3f) << 26;
    res += jump&0x3ffffff;
    fileout << "0x" << hex << res << endl;
}





void inst_type(string data, int line, string fout){
    for(int i=0;i<9;i++){
        if(R_inst[i]==data.substr(0,R_inst[i].length())){
            if(data.substr(0,3)=="ori"){break;} else if (data.substr(0,4)=="andi"){break;}
            Rinstruction* inst = new Rinstruction(data.substr(0,R_inst[i].length()), data.substr(R_inst[i].length(),data.length()-R_inst[i].length()));
            inst->print(fout);
            break;
        }
    }
    for(int i=0;i<11;i++){
        if(I_inst[i]==data.substr(0,I_inst[i].length())){
            Iinstruction* inst = new Iinstruction(data.substr(0,I_inst[i].length()), data.substr(I_inst[i].length(),data.length()-I_inst[i].length()),0x400000+line*4);
            inst->print(fout);
            break;
        }
    }
    for(int i=0;i<2;i++){
        if(J_inst[i]==data.substr(0,J_inst[i].length())){
            if(J_inst[i]=="j"){
                if(data.substr(0,2)=="jr"){continue;} else if (data.substr(0,3)=="jal"){continue;}
            }
            Jinstruction* inst = new Jinstruction(data.substr(0,J_inst[i].length()), data.substr(J_inst[i].length(),data.length()-J_inst[i].length()));
            inst->print(fout);
            break;
        }
    }
}








int main(int argc, char* argv[]) {

    // filename arguments parsing
    char* target_file = argv[1];
    string fin_file;
    string fout_file;
    int idx = 0;
    while(*(target_file+sizeof(char)*idx)!='\0'){idx++;}
    if(target_file[idx-1]!='s' || target_file[idx-2]!='.'){
        cout << "[ERROR] assembly file extension incorrect" << endl;
        return 0;
    }
    *(target_file+sizeof(char)*(idx-2))='\0';
    if(argv[2]){
        fout_file = argv[2];
        if(fout_file[fout_file.length()-1]!='o' || fout_file[fout_file.length()-2]!='.'){
            cout << "[ERROR] output file extension incorrect" << endl;
            return 0;
        }
        fout_file = fout_file.substr(0,fout_file.length()-2);
    } else {
        fout_file=target_file;
    }

    // file stream / string input_data variables setting
    fin_file=target_file;
    fin_file += ".s";
    fout_file=fout_file+".o";
    ifstream fin(fin_file);
    ofstream fout(fout_file);
    string fin_data;
    string _npassed_data="";
    idx = 0;
    string cls_data="";

    // data aligning
    while(getline(fin,fin_data)){
        // data reading stop
        if(fin.eof()){
            if(_npassed_data!=""){
                for(int i=0;i<_npassed_data.length();i++){
                    if(!isspace(_npassed_data[i])){
                        cls_data += _npassed_data[i];
                    }
                }
            }
            break;
        }


        if(_npassed_data!=""){
            fin_data = _npassed_data+fin_data;
            _npassed_data = "";
        }
        bool is_labeled=false;
        bool is_pass=true;
        for(int i=0;i<fin_data.length();i++){
            if(!is_pass & !isspace(fin_data[i])){
                is_pass = true;
            }
            if(fin_data[i]==':'){
                is_labeled = true;
                is_pass = false;
            }
        }
        if(!is_pass){
            _npassed_data = fin_data;
            continue;
        }
        
        for(int i=0;i<fin_data.length();i++){
            if(!isspace(fin_data[i])){
                cls_data+=fin_data[i];
            }
        }
        cls_data+='\n';
    }
    cls_data += _npassed_data;

    // partition total code
    string text_section="";
    string data_section="";
    string _data="";
    bool is_word=false;
    string word="";
    int word_int;
    int label_cnt = 0;
    char status=0;
    for(int i=0;i<cls_data.length();i++){
        if(cls_data[i]==':'){label_cnt++;}
        if(cls_data[i]!='\n'){
            _data += cls_data[i];
            if(_data.length()>4){
                if(_data.substr(_data.length()-5,5)==".word"){
                    _data=_data.substr(0,_data.length()-5);
                }
            }
        } else {
            if(_data==".text"){
                status = 0;
            } else if(_data==".data"){
                status = 1;
            } else {
                switch (status)
                {
                    case 0:
                    text_section += _data+"\n"; break;
                    case 1:
                    data_section += _data+"\n"; break;
                }
            }
            _data="";
        }
    }
    switch (status)
    {
        case 0:
        text_section += _data+'\n'; break;
        case 1:
        data_section += _data+'\n'; break;
    }
    text_section = text_section.substr(0,text_section.length()-1);
    data_section = data_section.substr(0,data_section.length()-1);

    // label set
    label_li = new string[label_cnt];
    label_li_num = new int[label_cnt];
    int li_idx=0;
    _data = "";
    int line=0;
    for(int i=0;i<data_section.length();i++){
        if(data_section[i]==':'){
            label_li[li_idx]=_data.substr(0,i);
            label_li_num[li_idx]=0x10000000+line*4;
            li_idx++;
            _data = "";
        }else if(data_section[i]!='\n') {
            _data += data_section[i];
        }else{
            line++;
            if(_data==""){line--;}
            _data="";
        }
    }
    line = 0;
    _data="";
    for(int i=0;i<text_section.length();i++){
        if(text_section[i]==':'){
            label_li[li_idx]=_data.substr(0,i);
            label_li_num[li_idx]=0x00400000+line*4;
            li_idx++;
            _data = "";
        }else if(text_section[i]!='\n') {
            _data += text_section[i];
        }else{
            if(_data.substr(0,2)=="la"){
                int j;
                for(j=0;j<_data.length();j++){
                    if(_data[j]==','){break;}
                }
                string label = _data.substr(j+1,_data.length()-j-1);
                bool iscomplex=true;
                for(int k=0;k<label_cnt;k++){
                    if(label_li[k]==label){
                        if((label_li_num[k]&0xFFFF)==0x0000){
                            char buffer[5]="";
                            sprintf(buffer,"%x",label_li_num[k]>>16);
                            string add_imm = buffer;
                            add_imm = "0x"+add_imm;
                            int section_len = text_section.length();
                            text_section=text_section.substr(0,i-_data.length())+"lui"+_data.substr(2,j-2)+','+add_imm+text_section.substr(i,_data.length()-i);
                            i+=text_section.length()-section_len;
                        } else {
                            char buffer[5]="";
                            sprintf(buffer,"%x",label_li_num[k]>>16);
                            string add_imm1 = buffer;
                            add_imm1 = "0x"+add_imm1;
                            sprintf(buffer,"%x",label_li_num[k]&0x0000FFFF);
                            string add_imm2 = buffer;
                            add_imm2 = "0x"+add_imm2;
                            int section_len = text_section.length();
                            text_section=text_section.substr(0,i-_data.length())+"lui"+_data.substr(2,j-2)+','+add_imm1+'\n'+"ori"+_data.substr(2,j-2)+','+_data.substr(2,j-2)+','+add_imm2+text_section.substr(i,_data.length()-i);
                            i+=text_section.length()-section_len;
                            line++;
                        }
                        iscomplex=false;
                    }
                }
                if(iscomplex){
                    line++;
                }
            }
            line++;
            if(_data==""){line--;}
            _data="";
        }
    }
    _data="";
    for(int i=0;i<text_section.length();i++){
        if(text_section[i]==':'){
            _data = "";
        }else if(text_section[i]!='\n') {
            _data += text_section[i];
        }else{
            if(_data.substr(0,2)=="la"){
                int j;
                for(j=0;j<_data.length();j++){
                    if(_data[j]==','){break;}
                }
                string label = _data.substr(j+1,_data.length()-j-1);
                for(int k=0;k<label_cnt;k++){
                    if(label_li[k]==label){
                        char buffer[5]="";
                        sprintf(buffer,"%x",label_li_num[k]>>16);
                        string add_imm1 = buffer;
                        add_imm1 = "0x"+add_imm1;
                        sprintf(buffer,"%x",label_li_num[k]&0x0000FFFF);
                        string add_imm2 = buffer;
                        add_imm2 = "0x"+add_imm2;
                        text_section=text_section.substr(0,i-_data.length())+"lui"+_data.substr(2,j-2)+','+add_imm1+'\n'+"ori"+_data.substr(2,j-2)+','+_data.substr(2,j-2)+','+add_imm2+text_section.substr(i,_data.length()-i);
                    }
                }
            }
            _data="";
        }
    }

    int text_size = 0;
    for(int i=0;i<text_section.length();i++){
        if(text_section[i]=='\n'){
            text_size++;
        }
    }
    if(text_section[text_section.length()-1]==':'){
        text_size--;
    }
    int data_size = 0;
    for(int i=0;i<data_section.length();i++){
        if(data_section[i]=='\n'){
            data_size++;
        }
    }
    data_size++;
    fout << "0x" << hex << text_size*4 << endl;
    fout << "0x" << hex << data_size*4 << endl;

    // parsing instruction type each text section
    _data="";
    line=0;
    for(int i=0;i<text_section.length();i++){
        if(text_section[i]==':'){
            _data="";
        } else if (text_section[i]!='\n'){
            _data+=text_section[i];
        } else {
            inst_type(_data,line,fout_file);
            line++;
            if(_data==""){line--;}
            _data="";
        }
    }
    
    // data section output
    ofstream fileout(fout_file,ios::app);
    for(int i=0;i<data_section.length();i++){
        if(data_section[i]==':'){
            _data="";
        } else if (data_section[i]!='\n'){
            _data+=data_section[i];
        } else {
            fileout << "0x" << hex << imm_stoi(_data) << endl;
            _data="";
        }
    }
    fileout << "0x" << hex << imm_stoi(_data) << endl;
    return 0;
}