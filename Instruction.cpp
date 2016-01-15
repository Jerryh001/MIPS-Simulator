#include<sstream>
#include"Instruction.h"
#include"Header.h"
Instruction::Instruction()
{
}
Instruction::Instruction(const string& s, const string& l = "")
{
	Raw = s;
	Label = l;
	Compile();
}
inline void Instruction::Compile()
{
	stringstream ss;
	string temp = Raw;
	for (size_t i = 0; i < temp.length(); i++)
	{
		if (temp[i] == ',' || temp[i] == '(' || temp[i] == ')' || temp[i] == '$')
		{
			temp[i] = ' ';
		}
	}
	ss << temp;
	ss >> temp;
	if (temp == "add" || temp == "sub")
	{
		OpCode = OPCODE_R;
		if (temp == "add")
		{
			Funct = FUNCT_ADD;
		}
		else
		{
			Funct = FUNCT_SUB;
		}
	}
	else
	{
		if (temp == "addi")
		{
			OpCode = OPCODE_ADDI;
		}
		else if (temp == "beq")
		{
			OpCode = OPCODE_BEQ;
		}
		else if (temp == "lw")
		{
			OpCode = OPCODE_LW;
		}
		else//sw
		{
			OpCode = OPCODE_SW;
		}
	}
	if (getType() == 'R')
	{
		ss >> temp;
		Rd = atoi(temp.c_str());
		ss >> temp;
		Rs = atoi(temp.c_str());
		ss >> temp;
		Rt = atoi(temp.c_str());
	}
	else
	{
		switch (OpCode)
		{
		case 0x23:
		case 0x2b:
			ss >> temp;
			Rt = atoi(temp.c_str());
			ss >> temp;
			Immediate = atoi(temp.c_str());
			ss >> temp;
			Rs = atoi(temp.c_str());
			break;
		case 0x8:
			ss >> temp;
			Rt = atoi(temp.c_str());
			ss >> temp;
			Rs = atoi(temp.c_str());
			ss >> temp;
			Immediate = atoi(temp.c_str());
			break;
		case 0x4:
			ss >> temp;
			Rs = atoi(temp.c_str());
			ss >> temp;
			Rt = atoi(temp.c_str());
			ss >> HoldLabel;
			break;
		}
	}
}