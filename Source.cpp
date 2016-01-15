#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<cctype>
#include<cstdlib>
#include"Header.h"
#include"Instruction.h"
#include"Bar.h"
using namespace std;
int reg[32];
int PC = 0;
string take;
string forwardmsg;
vector<Instruction> IM;
IFBar BEFOREIF;
IFIDBar IFID;
IDEXBar IDEX;
EXMEMBar EXMEM;
MEMWBBar MEMWB;
ifstream fin("input.txt");
ofstream fout("output.txt");
int mux(const int& a, const int& b, const int& choose)
{
	return choose ? b : a;
}
int mux(const int& a, const int& b, const int& c, const int& choose)
{
	switch (choose)
	{
	case 0:
		return a;
	case 1:
		return b;
	case 2:
		return c;
	}
	return 0;
}
int ALU(const int& a, const int& b, const int& action)
{
	return action ? a - b : a + b;
}
Instruction NOP = { "nop","" };
void IF()
{
	const int src = BEFOREIF.ReadData("PCSrc") && IDEX.ReadData("MEM_Branch");
	if (BEFOREIF.ReadData("PCWrite"))
	{
		PC = mux(BEFOREIF.ReadData("PCADD4"), BEFOREIF.ReadData("AddResult") + 1, src);
	}
	BEFOREIF.WriteData("PCWrite", 1);
	BEFOREIF.WriteData("PCADD4", PC + 1);
	if (static_cast<size_t>(PC) < IM.size())
	{
		IFID.WriteIns(IM[PC]);
		IFID.WriteData("PCADD4", PC + 1);
		IFID.WriteRaw(IM[PC].Raw);
	}
	else
	{
		IFID.WriteIns(NOP);
		IFID.WriteRaw("nop");
	}
}
void ID()
{
	IDEX.WriteData("PCADD4", IFID.ReadData("PCADD4"));
	const int rs = IFID.ReadData("RS");
	const int rt = IFID.ReadData("RT");
	const int rd = IFID.ReadData("RD");
	const int Immediate = IFID.ReadData("IMMEDIATE");
	IDEX.WriteData("RS", rs);
	IDEX.WriteData("RT", rt);
	IDEX.WriteData("RD", rd);
	IDEX.WriteData("DATA1", reg[rs]);
	IDEX.WriteData("DATA2", reg[rt]);
	IDEX.WriteData("IMMEDIATE", Immediate);
	IDEX.WriteData("TYPE", IFID.ReadData("TYPE"));
	IDEX.WriteControl(IFID.ReadData("OPCODE"), IFID.ReadData("FUNCT"));
	BEFOREIF.WriteData("AddResult", IDEX.ReadData("PCADD4") + Immediate);
	BEFOREIF.WriteData("PCSrc", reg[rs] == reg[rt]);
	IDEX.WriteRaw(IFID.ReadRaw());
}
void EX()
{
	const int ALUOp = IDEX.ReadData("EX_ALUOp");
	const int ALUSrc = IDEX.ReadData("EX_ALUSRC");
	const int RegDst = IDEX.ReadData("EX_RegDst");
	const int Data1 = IDEX.ReadData("DATA1");
	const int Data2 = IDEX.ReadData("DATA2");
	const int Immediate = IDEX.ReadData("IMMEDIATE");
	const int MEMRD = IDEX.ReadData("MEMRD");
	const int WBRD = IDEX.ReadData("WBRD");
	const int a = mux(Data1, WBRD, MEMRD, IDEX.ReadData("ForwardA"));
	const int b = mux(Data2, WBRD, MEMRD, IDEX.ReadData("ForwardB"));
	if (IDEX.ReadData("ForwardA") == 1 || IDEX.ReadData("ForwardB") == 1)
	{
		forwardmsg = "Forwarding from stage 5 to stage 3.";
	}
	if (IDEX.ReadData("ForwardA") == 2 || IDEX.ReadData("ForwardB") == 2)
	{
		forwardmsg = "Forwarding from stage 4 to stage 3.";
	}
	EXMEM.WriteData("ALUResult", ALU(a, mux(b, Immediate, ALUSrc), ALUOp));
	EXMEM.WriteData("WriteData", b);
	EXMEM.WriteData("WriteReg", mux(IDEX.ReadData("RT"), IDEX.ReadData("RD"), IDEX.ReadData("EX_RegDst")));
	IDEX.PassControlTo(EXMEM);
	EXMEM.WriteRaw(IDEX.ReadRaw());
}
void MEM()
{
	MEMWB.WriteData("ALUResult", EXMEM.ReadData("ALUResult"));
	MEMWB.WriteData("WriteReg", EXMEM.ReadData("WriteReg"));
	if (EXMEM.ReadData("MEM_MemRead"))
	{
		MEMWB.WriteData("ReadData", 1);
	}
	EXMEM.PassControlTo(MEMWB);
	MEMWB.WriteRaw(EXMEM.ReadRaw());
}
void WB()
{

	if (MEMWB.ReadData("WB_RegWrite"))
	{
		reg[MEMWB.ReadData("WriteReg")] = mux(MEMWB.ReadData("ALUResult"), MEMWB.ReadData("ReadData"), MEMWB.ReadData("WB_MemtoReg"));
	}
	BEFOREIF.WriteRaw(MEMWB.ReadRaw());
}
void ReadIns()
{
	string head, in, holdlabel;
	while (1)
	{
		fin >> head;
		if (fin.eof()) break;
		getline(fin, in);
		in = head + in;
		if (in.find(':') != string::npos)
		{
			holdlabel.clear();
			for (size_t i = 0; isalnum(in[i]) && i < in.length(); i++)
			{
				holdlabel += in[i];
			}
		}
		else
		{
			IM.push_back({ in,holdlabel });
			holdlabel.clear();
		}
	}
}
void SetLabel()
{
	for (size_t i = 0; i < IM.size(); i++)
	{
		if (IM[i].HoldLabel != "")
		{
			for (size_t j = 0; j < IM.size(); j++)
			{
				if (i == j) continue;
				if (IM[i].HoldLabel == IM[j].Label)
				{
					IM[i].Immediate = static_cast<int>(j - i - 1);
				}
			}
		}
	}
}
bool Empty()
{
	return BEFOREIF.ReadRaw() == IFID.ReadRaw() && IFID.ReadRaw() == IDEX.ReadRaw() && IDEX.ReadRaw() == EXMEM.ReadRaw() && EXMEM.ReadRaw() == MEMWB.ReadRaw() && MEMWB.ReadRaw() == "nop";
}
void Forward()
{
	IDEX.WriteDataDirect("ForwardA", 0);
	IDEX.WriteDataDirect("ForwardB", 0);
	if (MEMWB.ReadData("WB_RegWrite") && MEMWB.ReadData("WriteReg") != 0)
	{
		if (MEMWB.ReadData("WriteReg") == IDEX.ReadData("RS"))
		{
			IDEX.WriteDataDirect("ForwardA", 1);
		}
		if (MEMWB.ReadData("WriteReg") == IDEX.ReadData("RT") && (IDEX.ReadData("EX_ALUSrc") == 0 || IDEX.ReadData("MEM_MemWrite")))
		{
			IDEX.WriteDataDirect("ForwardB", 1);
		}
		IDEX.WriteDataDirect("WBRD", mux(MEMWB.ReadData("ALUResult"), MEMWB.ReadData("ReadData"), MEMWB.ReadData("WB_MemtoReg")));
	}
	if (EXMEM.ReadData("WB_RegWrite") && EXMEM.ReadData("WriteReg") != 0)
	{
		if (EXMEM.ReadData("WriteReg") == IDEX.ReadData("RS"))
		{
			IDEX.WriteDataDirect("ForwardA", 2);
		}
		if (EXMEM.ReadData("WriteReg") == IDEX.ReadData("RT") && (IDEX.ReadData("EX_ALUSrc") == 0 || IDEX.ReadData("MEM_MemWrite")))
		{
			IDEX.WriteDataDirect("ForwardB", 2);
		}
		IDEX.WriteDataDirect("MEMRD", EXMEM.ReadData("AddResult"));
	}
}
void BarWriteClock()
{
	BEFOREIF.Update();
	IFID.Update();
	IDEX.Update();
	EXMEM.Update();
	MEMWB.Update();
}
void Stall()
{
	if (IDEX.ReadData("StallNextTurn"))
	{
		BEFOREIF.WriteDataDirect("PCWrite", 0);
		EXMEM.Bubble();
		IFID.UnsetWrite();
		IFID.DataProtect();
		IDEX.WriteData("StallNextTurn", 0);
		IDEX.WriteDataDirect("StallNextTurn", 0);
		IDEX.UnsetWrite();
		IDEX.DataProtect();
		EXMEM.UnsetWrite();

	}
	if (IDEX.ReadData("MEM_MemRead") && IDEX.ReadData("RT") != 0 && (IDEX.ReadData("RT") == IFID.ReadData("RS") || IDEX.ReadData("RT") == IFID.ReadData("RT")))
	{
		IDEX.WriteData("StallNextTurn", 1);
	}

}
void Predict()
{
	if (take == "T")
	{
		if (IFID.ReadData("OPCODE") == OPCODE_BEQ)
		{
			IFID.Bubble();
			IFID.UnsetWrite();
			BEFOREIF.WriteDataDirect("PCWrite", 0);
		}
	}
	else
	{
		if (BEFOREIF.ReadData("PCSrc") && IDEX.ReadData("MEM_Branch"))
		{
			IDEX.Bubble();
			IDEX.UnsetWrite();
		}
	}
}
int main()
{
	fin >> take;
	for (int i = 16; i <= 23; i++)
	{
		fin >> reg[i];
	}
	fin.ignore(256, '\n');
	ReadIns();
	SetLabel();
	for (int cycle = 1;; cycle++)
	{
		Predict();
		Forward();
		Stall();
		IF();
		ID();
		EX();
		MEM();
		WB();
		BarWriteClock();

		if (PC&&Empty())
		{
			break;
		}
		fout << "===clock cycle " << cycle << "===" << endl
			<< "IF :" << IFID.ReadRaw() << endl
			<< "ID :" << IDEX.ReadRaw() << endl
			<< "EX :" << EXMEM.ReadRaw() << endl
			<< "MEM:" << MEMWB.ReadRaw() << endl
			<< "WB :" << BEFOREIF.ReadRaw() << endl
			<< endl;
		if (forwardmsg != "" && !EXMEM.IsNop())
		{
			fout << forwardmsg << endl;
		}
		forwardmsg = "";

	}
	fout << "$16~$23" << endl;
	fout << reg[16];
	for (int i = 17; i <= 23; i++)
	{
		fout << " " << reg[i];
	}
	fout << endl;
}