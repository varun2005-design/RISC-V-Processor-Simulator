#include <bits/stdc++.h>
using namespace std;

struct ControlWord
{
    bool regRead;
    bool regWrite;
    bool memRead;
    bool memWrite;
    bool mem2reg;
    bool ALUSrc;
    bool branch;
    bool Jump;
    string ALUSelect;
    string ALUOp;
};

struct IFID
{
    string instruction;
    int npc;
    bool valid = 0;
    bool bstall = 0;
};

struct IDEX
{
    ControlWord CW;
    int rs1 = -1, rs2 = -1, imm = -1, npc = -1, jpc = -1, rsl1 = -1, rsl2 = -1;
    string func3 = "000", func7 = "0000000";
    int rd = -1;
    bool valid = 0;
    bool bstall = 0;
};

struct EXME
{
    ControlWord CW;
    int ALUResult = -1, rs2 = -1, rd = -1, rsl1 = -1, rsl2 = -1;
    int npc = -1, bpc = -1, jpc = -1;
    bool valid = 0;
    bool bstall = 0;
};

struct MEWB
{
    ControlWord CW;
    int ALUResult = -1, ldout = -1, rd = -1, rsl1 = -1, rsl2 = -1;
    int npc = -1, bpc = -1, jpc = -1;
    bool valid = 0;
};

class WriteBackUnit
{
public:
    int writeData;
    WriteBackUnit(ControlWord &CW, int ALUResult, int ldout, int npc)
    {
        if (CW.mem2reg)
        {
            writeData = ldout;
        }
        else if (CW.Jump)
        {
            writeData = npc;
        }
        else
        {
            writeData = ALUResult;
        }
    }
};

class Memory
{

   

public:
 vector<int> DM;
    Memory()
    {
        DM.resize(1024, 0);
    }

    int read(int address)
    {
        return DM[address];
    }

    void write(int address, int value)
    {
        DM[address] = value;
    }

    void print()
    {
        for (int i = 0; i < 10; i++)
        {
            cout << DM[i] << " ";
        }
        cout << endl;
    }
};

class ALU
{

public:
    int flag = 0;
    int operate(ControlWord &CW, int alusrc1, int alusrc2)
    {

        if (CW.ALUSelect == "0000")
        {
            return (alusrc1 & alusrc2);
        }

        else if (CW.ALUSelect == "0001")
        {
            return (alusrc1 | alusrc2);
        }

        else if (CW.ALUSelect == "0010")
        {
            return (alusrc1 + alusrc2);
        }

        else if (CW.ALUSelect == "0110")
        {
            if (CW.ALUOp == "01")
            {
                if (alusrc1 == alusrc2)
                {
                    flag = 1;
                    return 0;
                }
                else
                {
                    flag = 0;
                    return 0;
                }
            }

            return (alusrc1 - alusrc2);
        }

        return 0;
    }
};

class ALUControl
{
public:
    string ALUSelect;
    ALUControl(ControlWord &CW, string func7, string func3)
    {

        if (CW.ALUOp == "00")
        {
            CW.ALUSelect = "0010";
            return;
        }

        else if (CW.ALUOp == "01")
        {
            CW.ALUSelect = "0110";
            return;
        }

        else if (CW.ALUOp == "10")
        {
            if (func7[2] == '1')
            {
                if (func3 == "000")
                {
                    CW.ALUSelect = "0110";
                    return;
                }
            }

            else
            {
                if (func3 == "000")
                {
                    CW.ALUSelect = "0010";
                    return;
                }
                else if (func3 == "011")
                {
                    CW.ALUSelect = "0000";
                    return;
                }
                else if (func3 == "100")
                {
                    CW.ALUSelect = "0001";
                    return;
                }
            }
        }
        return;
    }
};

class RegisterFile
{

public:
    vector<int> GPR;
    RegisterFile()
    {
        GPR.resize(32, 0);
    }

    int read(int index)
    {
        return GPR[index];
    }

    void write(int index, int value)
    {
        GPR[index] = value;
    }

    void print()
    {
        for (int i = 0; i < 32; i++)
        {
            cout << GPR[i] << " ";
        }
        cout << endl;
    }
};

class decoder
{
public:
    string ins, func7, rs2, rs1, func3, rd, op;
    int imm, rsl1, rsl2, rdl;

    decoder(string &instruction)
    {
        ins = instruction;
        decodeterms();
        imm = immgen();
    }

    void decodeterms()
    {
        func7 = ins.substr(0, 7);
        rs2 = ins.substr(7, 5);
        rs1 = ins.substr(12, 5);
        func3 = ins.substr(17, 3);
        rd = ins.substr(20, 5);
        op = ins.substr(25, 7);
        rsl1 = binary_to_signed_int(string("0") + rs1);
        rsl2 = binary_to_signed_int(string("0") + rs2);
        rdl = binary_to_signed_int(string("0") + rd);
        if (op == "0010011")
        {
            func7 = "0000000";
        }
    }

    int binary_to_signed_int(string bin)
    {
        int n = bin.size();
        if (bin[0] == '0')
        {
            int value = 0;
            for (int i = 0; i < n; i++)
            {
                value = (value << 1) | (bin[i] - '0');
            }
            return value;
        }
        string inverted = bin;
        for (char &bit : inverted)
        {
            bit = (bit == '0') ? '1' : '0';
        }
        int value = 0;
        for (int i = 0; i < n; i++)
        {
            value = (value << 1) | (inverted[i] - '0');
        }
        value += 1;
        return -value;
    }

    int immgen()
    {

        if (op == "0010011" || op == "1100111" || op == "0000011")
        {
            return (binary_to_signed_int(ins.substr(0, 12)));
        }
        else if (op == "1101111")
        {
            return (binary_to_signed_int(ins.substr(0, 20)));
        }
        else if (op == "1100011" || op == "0100011")
        {
            return (binary_to_signed_int(ins.substr(0, 7) + ins.substr(20, 5)));
        }
        else
        {
            return 0;
        }
    }
};

class controlunit
{
public:
    ControlWord CW;

    controlunit(string op)
    {

        if (op == "0110011")
        {
            CW.regRead = 1;
            CW.regWrite = 1;
            CW.ALUSrc = 0;
            CW.memRead = 0;
            CW.memWrite = 0;
            CW.mem2reg = 0;
            CW.branch = 0;
            CW.Jump = 0;
            CW.ALUOp = "10";
        }

        else if (op == "0010011")
        {
            CW.regRead = 1;
            CW.regWrite = 1;
            CW.ALUSrc = 1;
            CW.memRead = 0;
            CW.memWrite = 0;
            CW.mem2reg = 0;
            CW.branch = 0;
            CW.Jump = 0;
            CW.ALUOp = "10";
        }

        else if (op == "0000011")
        {
            CW.regRead = 1;
            CW.regWrite = 1;
            CW.ALUSrc = 1;
            CW.memRead = 1;
            CW.memWrite = 0;
            CW.mem2reg = 1;
            CW.branch = 0;
            CW.Jump = 0;
            CW.ALUOp = "00";
        }

        else if (op == "0100011")
        {
            CW.regRead = 1;
            CW.regWrite = 0;
            CW.ALUSrc = 1;
            CW.memRead = 0;
            CW.memWrite = 1;
            CW.mem2reg = 0;
            CW.branch = 0;
            CW.Jump = 0;
            CW.ALUOp = "00";
        }

        else if (op == "1100011")
        {
            CW.regRead = 1;
            CW.regWrite = 0;
            CW.ALUSrc = 0;
            CW.memRead = 0;
            CW.memWrite = 0;
            CW.mem2reg = 0;
            CW.branch = 1;
            CW.Jump = 0;
            CW.ALUOp = "01";
        }

        else
        {
            CW.regRead = 0;
            CW.regWrite = 1;
            CW.ALUSrc = 0;
            CW.memRead = 0;
            CW.memWrite = 0;
            CW.mem2reg = 0;
            CW.branch = 0;
            CW.Jump = 1;
            CW.ALUOp = "10";
        }
    }
};

class Fetcher
{

    vector<string> instruction_memory;
    string line;

public:
    Fetcher(const string &filename)
    {
        ifstream fin(filename);
        if (!fin)
        {
            cout << "Error: Cannot open " << endl;
            return;
        }

        string line;
        while (fin >> line)
        {
            if (!line.empty())
                instruction_memory.push_back(line);
        }
        fin.close();
    }

    string Instruction(int pc)
    {
        if (pc - 1 < 0 || pc - 1 >= instruction_memory.size())
        {
            cout << "Error: PC out of bounds" << endl;
            return "";
        }
        return instruction_memory[pc - 1];
    }

    int size()
    {
        return (instruction_memory.size());
    }
};

IFID IF_stage(IFID &ifid, Fetcher &fetch, int pc)
{
    if (ifid.bstall)
    {
        return ifid;
    }

    IFID reg;
    if (pc > fetch.size())
    {
        reg.valid = 0;
        return reg;
    }
    reg.instruction = fetch.Instruction(pc);
    reg.npc = pc + 1;
    reg.valid = 1;
    string s = reg.instruction.substr(25);
    if (s == "1100011")
    {
        reg.bstall = true;
    }

    return reg;
}

IDEX ID_stage(IFID &ifid, IDEX &idex, RegisterFile &rf)
{
    IDEX reg;
    if (!ifid.valid)
    {
        reg.valid = 0;
        return reg;
    }
    decoder dec(ifid.instruction);
    controlunit cu(dec.op);

    reg.CW = cu.CW;
    reg.rs1 = rf.read(dec.rsl1);
    reg.rs2 = rf.read(dec.rsl2);
    reg.rsl1 = dec.rsl1;
    reg.rsl2 = dec.rsl2;
    reg.imm = dec.imm;
    reg.func3 = dec.func3;
    reg.func7 = dec.func7;
    reg.rd = dec.rdl;
    reg.npc = ifid.npc;
    reg.jpc = dec.imm;
    reg.valid = 1;
    reg.bstall = ifid.bstall;
    return reg;
}

EXME EX_stage(IFID &ifid, IDEX &idex, EXME &exme, MEWB &mewb,MEWB &temp4, ALU &alu, int &pc)
{

    EXME reg;
    if (!idex.valid)
    {
        reg.valid = 0;
        return reg;
    }

    int alusrc1;
    ALUControl alc(idex.CW, idex.func7, idex.func3);
    int alusrc2; 
    alusrc1 = idex.rs1;
    alusrc2 = idex.rs2;

    if (mewb.CW.memRead)
    {
        if (idex.rsl1 == mewb.rd)
        {
            alusrc1 = mewb.ldout;
        }

        if (idex.rsl2 == mewb.rd)
        {
            
            alusrc2 = mewb.ldout;
        }
    }

    else
    {
        if (idex.rsl1 == mewb.rd)
        {
            alusrc1 = mewb.ALUResult;
        }

        if (idex.rsl2 == mewb.rd)
        {
            
            alusrc2 = mewb.ALUResult;
        }
    }

    if (exme.CW.memRead)
    {
        if (idex.rsl1 == temp4.rd)
        {
            alusrc1 = temp4.ldout;
        }

        if (idex.rsl2 == temp4.rd)
        {
            
            alusrc2 = temp4.ldout;
        }

    }

    if(!exme.CW.memRead){

            if (idex.rsl1 == exme.rd)
            {
                alusrc1 = exme.ALUResult;
            }

            if (idex.rsl2 == exme.rd)
            {
                alusrc2 = exme.ALUResult;
            }
        }

    if (idex.CW.memWrite && idex.rsl2 == exme.rd)
    {
        idex.rs2 = exme.ALUResult;
    }
    

    alusrc2 = idex.CW.ALUSrc ? idex.imm : alusrc2;
    int result = alu.operate(idex.CW, alusrc1, alusrc2);
    reg.CW = idex.CW;
    reg.ALUResult = result;
    reg.rs2 = idex.rs2;
    reg.rsl1 = idex.rsl1;
    reg.rsl2 = idex.rsl2;
    reg.rd = idex.rd;
    reg.CW.branch = alu.flag & idex.CW.branch;
    reg.npc = idex.npc;
    reg.bpc = idex.imm;
    reg.jpc = idex.jpc;
    reg.valid = 1;
    reg.bstall = idex.bstall;
    if (reg.bstall)
    {
        ifid.bstall = 0;
        idex.bstall = 0;
        reg.bstall = 0;
        if (reg.CW.branch)
        {
            pc = reg.bpc;
        }
        else
        {
            pc = pc + 1;
        }
    }

    return reg;
}

MEWB MEM_stage(EXME &exmem, MEWB &mewb, Memory &mem)
{
    MEWB reg;
    if (!exmem.valid)
    {
        reg.valid = 0;
        return reg;
    }

    reg.CW = exmem.CW;
    reg.rd = exmem.rd;
    reg.rsl1 = exmem.rsl1;
    reg.rsl2 = exmem.rsl2;
    reg.npc = exmem.npc;
    reg.bpc = exmem.bpc;
    reg.jpc = exmem.jpc;
    reg.ALUResult = exmem.ALUResult;
    if (exmem.CW.memRead)
        reg.ldout = mem.read(exmem.ALUResult);
    else if (exmem.CW.memWrite){
        mem.write(exmem.ALUResult, exmem.rs2);
    }
    else
        reg.ldout = 0;
    reg.valid = 1;
    return reg;
}

void WB_stage(MEWB &memwb, RegisterFile &rf)
{
    if (!memwb.valid)
    {
        return;
    }

    WriteBackUnit wbu(memwb.CW, memwb.ALUResult, memwb.ldout, memwb.npc);
    if (memwb.CW.regWrite)
    {
        rf.write(memwb.rd, wbu.writeData);
    }
}

int main()
{

    Fetcher fetch("binary.txt");
    RegisterFile rf;
    ALU alu;
    Memory mem;

    int pc = 0;
    IFID ifid,temp1;
    IDEX idex,temp2;
    EXME exme,temp3;
    MEWB mewb,temp4;

    do
    {

        int nextpc = pc;

        if (ifid.bstall)
        {
            nextpc = pc;
        }

        else
        {
            nextpc = pc + 1;
        }
        pc = nextpc;

        WB_stage(mewb, rf);
        temp4 = MEM_stage(exme, mewb, mem);
        temp3 = EX_stage(ifid, idex, exme, mewb, temp4, alu, pc);
        temp2 = ID_stage(ifid, idex, rf);
        temp1 = IF_stage(ifid, fetch, pc);

    

        ifid = temp1;
        idex = temp2;
        exme = temp3;
        mewb = temp4;

    

    } while (mewb.valid || exme.valid || idex.valid || ifid.valid);

    cout << "Sum of first 8 natural numbers : " << rf.GPR[10];
    return 0;
}