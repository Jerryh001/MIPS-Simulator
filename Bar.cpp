#include "Bar.h"
#include"Header.h"
void IFIDBar::WriteIns(const Instruction& in)
{
	WriteData("OPCODE", in.OpCode);
	WriteData("RS", in.Rs);
	WriteData("RT", in.Rt);
	WriteData("RD", in.Rd);
	WriteData("IMMEDIATE", in.Immediate);
	WriteData("FUNCT", in.Funct);
	WriteData("TYPE", in.getType());
	//WriteData("RAW", in.Raw);
}
void IDEXBar::WriteControl(const int& opcode, const int& funct)
{
	switch (opcode)
	{
	case OPCODE_R:

		switch (funct)
		{
		case FUNCT_ADD:
			WriteData("EX_ALUOp", 0);
			WriteData("EX_ALUSrc", 0);
			WriteData("EX_RegDst", 1);
			WriteData("MEM_Branch", 0);
			WriteData("MEM_MemWrite", 0);
			WriteData("MEM_MemRead", 0);
			WriteData("WB_RegWrite", 1);
			WriteData("WB_MemtoReg", 0);
			break;
		case FUNCT_SUB:
			WriteData("EX_ALUOp", 1);
			WriteData("EX_ALUSrc", 0);
			WriteData("EX_RegDst", 1);
			WriteData("MEM_Branch", 0);
			WriteData("MEM_MemWrite", 0);
			WriteData("MEM_MemRead", 0);
			WriteData("WB_RegWrite", 1);
			WriteData("WB_MemtoReg", 0);
			break;

		}
		break;
	case OPCODE_ADDI:
		WriteData("EX_ALUOp", 0);
		WriteData("EX_ALUSrc", 1);
		WriteData("EX_RegDst", 0);
		WriteData("MEM_Branch", 0);
		WriteData("MEM_MemWrite", 0);
		WriteData("MEM_MemRead", 0);
		WriteData("WB_RegWrite", 1);
		WriteData("WB_MemtoReg", 0);
		break;
	case OPCODE_LW:
		WriteData("EX_ALUOp", 0);
		WriteData("EX_ALUSrc", 1);
		WriteData("EX_RegDst", 0);
		WriteData("MEM_Branch", 0);
		WriteData("MEM_MemWrite", 0);
		WriteData("MEM_MemRead", 1);
		WriteData("WB_RegWrite", 1);
		WriteData("WB_MemtoReg", 1);
		break;
	case OPCODE_BEQ:
		WriteData("EX_ALUOp", 0);
		WriteData("EX_ALUSrc", 1);
		WriteData("MEM_Branch", 1);
		WriteData("MEM_MemWrite", 0);
		WriteData("MEM_MemRead", 0);
		WriteData("WB_RegWrite", 0);
		break;
	case OPCODE_SW:
		WriteData("EX_ALUOp", 0);
		WriteData("EX_ALUSrc", 1);
		WriteData("MEM_Branch", 0);
		WriteData("MEM_MemWrite", 1);
		WriteData("MEM_MemRead", 0);
		WriteData("WB_RegWrite", 0);
		break;
	}
}
Bar::Bar()
{
}
void Bar::SetWrite()
{
	Writeable = true;
}
void Bar::UnsetWrite()
{
	Writeable = false;
}
int Bar::ReadData(const string& s)
{
	return Data[s];
}

void Bar::WriteData(const string& s, const int& i)
{
	if (Writeable)
	{
		Buffer[s] = i;
	}
}

void Bar::WriteDataDirect(const string& s, const int& i)
{
	Data[s] = i;
}

void Bar::DataProtect()
{
	Buffer = Data;
}

string Bar::ReadRaw()
{
	return raw;
}

void Bar::WriteRaw(const string& s)
{
	if (Writeable)
	{
		rawbuffer = s;
	}
}

void Bar::WriteRawDirect(const string& s)
{
	raw = s;
}

void Bar::PassControlTo(Bar &b)
{
	b.WriteData("MEM_Branch", Data["MEM_Branch"]);
	b.WriteData("MEM_MemWrite", Data["MEM_MemWrite"]);
	b.WriteData("MEM_MemRead", Data["MEM_MemRead"]);
	b.WriteData("WB_RegWrite", Data["WB_RegWrite"]);
	b.WriteData("WB_MemtoReg", Data["WB_MemtoReg"]);
}

void Bar::Bubble()
{
	bool s = Writeable;
	Writeable = true;
	Buffer.clear();
	//SetWrite();
	WriteRaw("Bubble(nop)");
	WriteData("EX_ALUOp", 0);
	WriteData("EX_ALUSrc", 0);
	WriteData("EX_RegDst", 0);
	WriteData("MEM_Branch", 0);
	WriteData("MEM_MemWrite", 0);
	WriteData("MEM_MemRead", 0);
	WriteData("WB_RegWrite", 0);
	WriteData("WB_MemtoReg", 0);
	Writeable = s;

}

bool Bar::IsNop()
{
	return raw.find("nop") != string::npos;
}

void Bar::Update()
{

	Data.clear();
	Data = Buffer;
	Buffer.clear();
	raw = rawbuffer;
	SetWrite();
}
