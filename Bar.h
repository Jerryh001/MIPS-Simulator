#pragma once
#include"Instruction.h"
#include<map>
#include<string>
class Bar
{
	map<string, int> Data;
	map<string, int> Buffer;
	bool Writeable = true;
	string raw = "nop";
	string rawbuffer;
public:
	Bar();
	void SetWrite();
	void UnsetWrite();

	int ReadData(const string&);
	void WriteData(const string&, const int&);
	void WriteDataDirect(const string&, const int&);
	void DataProtect();
	string ReadRaw();
	void WriteRaw(const string&);
	void WriteRawDirect(const string&);
	void PassControlTo(Bar&);
	void Bubble();
	bool IsNop();
	void Update();
};
class IFBar;
class IFIDBar;
class IDEXBar;
class EXMEMBar;
class IFBar :public Bar
{

};
class IFIDBar :public Bar
{

public:
	void WriteIns(const Instruction&);
};
class IDEXBar :public Bar
{
public:
	void WriteControl(const int&, const int&);
};
class EXMEMBar :public Bar
{

};
class MEMWBBar :public Bar
{

};