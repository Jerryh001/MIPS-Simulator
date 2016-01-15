#pragma once
#include<string>
using namespace std;
class Instruction
{
public:
	string Label;
	int OpCode = 0;
	int Rs = 0;
	int Rt = 0;
	int Rd = 0;
	int Funct = 0;
	int Immediate = 0;
	string HoldLabel;
	string Raw;
	Instruction(const string&, const string&);
	Instruction();
	void Compile();
	char getType() const
	{
		return OpCode ? 'I' : 'R';
	}
};

