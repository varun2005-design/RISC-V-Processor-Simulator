#include<bits/stdc++.h>
using namespace std;



string tobinary(int num,int bits){
    if(num < 0) num = (int)pow(2,(int)bits)-abs(num);
    if(bits <= 0) return "";
    string binary = "";
    while(num >0){
        binary += (num%2 == 0 ?'0':'1');
        num /= 2;
    }
   
    reverse(binary.begin(),binary.end());
   
    if((int)binary.size() < bits){
        binary = string(bits-binary.size(),'0')+binary;
    }
   
    else if((int)binary.size() > bits){
        binary = binary.substr(binary.size()-bits);
    }
   
    return binary;
}

unordered_map<string,string> REG = {
    {"zero","00000"},{"ra","00001"},{"sp","00010"},{"gp","00011"},{"tp","00100"},
    {"t0","00101"},{"t1","00110"},{"t2","00111"},{"s0","01000"},
    {"s1","01001"},{"a0","01010"},{"a1","01011"},{"a2","01100"},{"a3","01101"},
    {"a4","01110"},{"a5","01111"},{"a6","10000"},{"a7","10001"},{"s2","10010"},
    {"s3","10011"},{"s4","10100"},{"s5","10101"},{"s6","10110"},{"s7","10111"},
    {"s8","11000"},{"s9","11001"},{"s10","11010"},{"s11","11011"},{"t3","11100"},
    {"t4","11101"},{"t5","11110"},{"t6","11111"}
};

string R_type (string rs2,string rs1,string ins,string rd);
string L_type (string imm, string rs1, string ins, string rd);
string S_type (string imm1, string rs2, string rs1, string ins, string imm2);
string B_type (string imm1, string rs2, string rs1, string ins ,string imm2);
string J_type (string imm1, string rd);
string I_type_arithmetic (string imm, string rs1, string ins, string rd);
string I_type_shift (string shift, string rs1, string ins, string rd);
string parse_rtype(string ins,string assembly);
string parse_iatype(string ins,string assembly);
string parse_istype(string ins,string assembly);
string parse_ltype(string ins,string assembly);
string parse_stype(string ins,string assembly);
string parse_btype(string ins, string assembly);
string parse_jtype(string ins,string assembly);



int main (){
   
    ifstream input("ISA.txt");
    ofstream output("binary.txt");
    string line;
   
    while(getline(input, line)){
       
        int i = 0;
        while(i < line.size()){
            if(line[i] == ' '){
                break;
            }
            i++;
        }
       
        string ins = line.substr(0,i);

        while(i < line.size()){
            if(line[i] != ' ') break;
            i++;
        }

        string assembly = line.substr(i);
       
        if(ins =="add" || ins == "slt"|| ins == "sltu"|| ins == "and"|| ins == "or"||
        ins == "xor"|| ins == "sll"|| ins == "srl"|| ins == "sub"|| ins == "sra"|| ins == "mul"|| ins == "rem"){
            output << parse_rtype(ins,assembly) << endl;
        }
        else if(ins =="lw" || ins == "ld"|| ins == "lh"|| ins == "lb"|| ins == "lwu"||
        ins == "lhu"|| ins == "lbu"|| ins == "jalr"){
            output << parse_ltype(ins,assembly) << endl;
        }
        else if(ins =="sd" || ins == "sw"|| ins == "shw"|| ins == "sb"){
            output << parse_stype(ins,assembly) << endl;
        }
        else if(ins =="beq" || ins == "bne"|| ins == "blt"|| ins == "bltu"||
        ins == "bge"|| ins == "bgeu"){
            output << parse_btype(ins,assembly) << endl;
        }
         else if(ins =="jal"){
            output << parse_jtype(ins,assembly) << endl;
        }
        else if (ins == "addi"|| ins == "andi"|| ins == "ori"|| ins == "xori"|| ins == "slti"||
        ins == "sltu"){
            output << parse_iatype(ins,assembly) << endl;
        }
        else {
            output << parse_istype(ins,assembly) << endl;
        }
    }
   
    input.close();
    output.close();
    return 0;
}


string R_type (string rs2,string rs1,string ins,string rd){
   
    string op = "0110011";
   
    unordered_map<string,string> func3 = {
        {"add","000"},{"slt","001"},{"sltu","010"},{"and","011"},{"or","100"},
        {"xor","101"},{"sll","110"},{"srl","111"},{"sub","000"},{"sra","001"},{"mul","000"},{"rem","110"}
    };
   
    if(ins == "add" || ins == "slt" || ins == "sltu" || ins == "and" || ins == "or" ||
        ins == "xor" || ins == "sll" || ins == "srl"){
       
        return (string("0000000") + REG[rs2] + REG[rs1] + func3[ins] + REG[rd] + op );
    }

    else if(ins == "mul" || ins == "rem") return(string("0000001") + REG[rs2] + REG[rs1] + func3[ins] + REG[rd] + op);
   
    else{
       
        return (string("0100000")+ REG[rs2]+ REG[rs1]+ func3[ins]+ REG[rd]+ op);
    }
   
   
   
}

string L_type (string imm, string rs1, string ins, string rd){
   
    string op = "0000011";
   
    unordered_map<string,string> func3 = {
       
        {"lw","000"},{"ld","001"},{"lh","010"},{"lb","011"},{"lwu","100"},
        {"lhu","101"},{"lbu","110"}
    };
   
    return (imm + REG[rs1] + func3[ins] + REG[rd] + op );
}

string S_type (string imm1, string rs2, string rs1, string ins, string imm2){
   
    string op = "0100011";
   
    unordered_map<string,string> func3 = {
       
         {"sd","000"},{"sw","001"},{"shw","010"},{"sb","011"}
       
    };
   
    return (imm1 + REG[rs2] + REG[rs1] + func3[ins] + imm2 + op);
}

string B_type (string imm1, string rs2, string rs1, string ins ,string imm2){
   
    string op = "1100011";
   
    unordered_map<string,string> func3 = {
       
         {"beq","000"},{"bne","001"},{"blt","010"},{"bltu","011"},{"bge","100"},
         {"bgeu","101"}
    };
   
    return (imm1 + REG[rs2] + REG[rs1] + func3[ins] + imm2 + op);
}

string J_type (string imm1, string rd){
   
    string op = "1101111";
   
    return (imm1 + REG[rd] + op);
}

string I_type_arithmetic (string imm, string rs1, string ins, string rd){
   
    string op = "0010011";
   
    unordered_map<string,string> func3 = {
       
        {"addi","000"},{"slti","001"},{"sltiu","010"},{"andi","011"},{"ori","100"},
        {"xori","101"},{"sll","110"}
    };
   
    if(ins == "jalr"){
        return (imm + REG[rs1] + func3["addi"] + REG[rd] + string("1100111"));
    }
   
    else {
        return (imm + REG[rs1] + func3[ins] + REG[rd] + op);
    }
   
}

string I_type_shift (string shift, string rs1, string ins, string rd){
   
    string op = "0010011";
   
    unordered_map<string,string> func3 = {
       
            {"slli","000"},{"srli","001"},{"srai","000"}  
    };
   
    if(ins == "srai"){
        return(string("0100000") + shift + REG[rs1] + func3[ins] + REG[rd] + op);
    }
   
    else{
        return(string("0000000") + shift + REG[rs1] + func3[ins] + REG[rd] + op);
    }
}


string parse_rtype(string ins,string assembly){
   
    int j;
    string a[3];

    for (int i = 0; i < 3; i++)
    {
        if(i == 2) a[i] = assembly.substr(0);
        else{
            j = assembly.find(',');
            a[i] = assembly.substr(0,j);
            assembly.assign(assembly.substr(j+1));
        }
    }

    
    
    
    return (R_type(a[2],a[1],ins,a[0]));
}

string parse_iatype(string ins,string assembly){

    int j;
    string a[3];

    for (int i = 0; i < 3; i++)
    {
        if(i == 2) a[i] = assembly.substr(0);
        else{
            j = assembly.find(',');
            a[i] = assembly.substr(0,j);
            assembly.assign(assembly.substr(j+1));
        }
    }
   
   
   
    int num = atoi(a[2].c_str());
   
    a[2] = tobinary(num,12);
   
    return (I_type_arithmetic(a[2],a[1],ins,a[0]));
}

string parse_istype(string ins,string assembly){
   
    int j;
    string a[3];

    for (int i = 0; i < 3; i++)
    {
        if(i == 2) a[i] = assembly.substr(0);
        else{
            j = assembly.find(',');
            a[i] = assembly.substr(0,j);
            assembly.assign(assembly.substr(j+1));
        }
    }
   
    int num = atoi(a[2].c_str());
   
    a[2] = tobinary(num,5);
   
    return (I_type_shift(a[2],a[1],ins,a[0]));
}

string parse_ltype(string ins,string assembly){
   
    string a[3];
    int i = 0;
    int j;
    assembly.pop_back();
    j = assembly.find(',');
    a[0] = assembly.substr(i,j-i);
    i = j+1;
    j = assembly.find('(');
    a[1] = assembly.substr(i,j-i);
    a[2] = assembly.substr(j+1);

    int num = atoi(a[1].c_str());
    a[1] = tobinary(num,12);

    if(ins == "jalr") return(I_type_arithmetic(a[1],a[2],ins,a[0]));
   
     else return(L_type(a[1],a[2],ins,a[0]));
}

string parse_stype(string ins,string assembly){

    string a[3];
    int i = 0;
    int j;
    assembly.pop_back();
    j = assembly.find(',');
    a[0] = assembly.substr(i,j-i);
    i = j+1;
    j = assembly.find('(');
    a[1] = assembly.substr(i,j-i);
    a[2] = assembly.substr(j+1);
   
    
   
    int num = atoi(a[1].c_str());
    a[1] = tobinary(num,12);
   
    string A[2];
   
    int flag = 7;
   
    A[0] = a[1].substr(0,7);
    A[1] = a[1].substr(flag,5);
   
    return(S_type(A[0], a[0],a[2],ins,A[1]));
   
}

string parse_btype(string ins, string assembly){
   
    int j;
    string a[3];

    for (int i = 0; i < 3; i++)
    {
        if(i == 2) a[i] = assembly.substr(0);
        else{
            j = assembly.find(',');
            a[i] = assembly.substr(0,j);
            assembly.assign(assembly.substr(j+1));
        }
    }

    int num = atoi(a[2].c_str());
    a[2] = tobinary(num,12);
   
    string A[2];
   
    int flag = 7;
   
    A[0] = a[2].substr(0,7);
    A[1] = a[2].substr(flag,5);
    return(B_type(A[0],a[1],a[0],ins ,A[1]));
   
}

string parse_jtype(string ins,string assembly){
   

    int j;
    string a[2];

    for (int i = 0; i < 2; i++)
    {
        if(i == 1) a[i] = assembly.substr(0);
        else{
            j = assembly.find(',');
            a[i] = assembly.substr(0,j);
            assembly.assign(assembly.substr(j+1));
        }
    }


    int num = atoi(a[1].c_str());
    a[1] = tobinary(num,20);
   
    return(J_type(a[1],a[0]));
}