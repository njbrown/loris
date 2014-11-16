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
	Assembly *assembly;
public:
	DragonScript()
	{
		assembly = 0;
	}

	void AddSource(string source)
	{
		compiler.AddSource("<source>",source);
	}

	void AddSource(string filename,string source)
	{
		compiler.AddSource(filename,source);
	}

	bool HasError()
	{
		return error.code!=Error::NONE;
	}

	Error GetError()
	{
		return error;
	}

	bool Compile()
	{
		if(!compiler.Compile())
		{
			error = compiler.GetError();
			return false;
		}

		assembly = compiler.GetAssembly();
		vm.SetAssembly(assembly);

		return true;
	}

	Value ExecuteFunction(string name)
	{
		//bad! find a way to return an error instead
		if(assembly==NULL)
			return Value::CreateNull();

		Function* func = assembly->GetFunction(name);
		if(func==NULL)
			return Value::CreateNull();

		Value ret = vm.ExecuteFunction(func);

		if(vm.HasError())
			error = vm.GetError();
		
		return ret;
	}

	Value ExecuteFunction(Function* func)
	{
		Value ret = vm.ExecuteFunction(func);

		if(vm.HasError())
			error = vm.GetError();
		
		return ret;
	}

	void AddNativeFunction(string name,NativeFunction func)
	{
		assembly->AddNativeFunction(name,func);
	}
};

}