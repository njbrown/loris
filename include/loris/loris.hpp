/*
Copyright (c) 2014, Nicolas Brown
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of dragonscript nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#pragma once


#include "virtualmachine.hpp"
#include "compiler.hpp"
#include "error.hpp"



namespace loris
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