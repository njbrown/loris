#pragma once


#include "virtualmachine.hpp"
#include "compiler.hpp"
#include "error.hpp"



namespace dragonscript
{

class DragonScript
{
	VirtualMachine vm;
	Compiler compiler;
	Error error;
public:
	void AddSource(string source)
	{
		compiler.AddSource("<source>",source);
	}
	void AddSource(string filename,string source)
	{
		compiler.AddSource(filename,source);
	}

	void Compile()
	{
		if(compiler.Compile())
		{

		}
	}

	Value ExecuteFunction(string name)
	{
		return Value::CreateNull();
	}
};

}