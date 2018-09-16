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

class Loris
{
	VirtualMachine vm;
	Compiler compiler;
	Error error;
	Assembly *assembly;
public:
	Loris();

	void AddSource(string source);
	void AddSource(string filename, string source);
	void AddFileSource(string filename);

	bool HasError();

	Error GetError();

	bool Compile();

	Value ExecuteFunction(const string& name);

	Value ExecuteFunction(Function* func);

	template<typename T>
	T ExecuteFunction(Function* func)
	{
		return (T)ExecuteFunction(func);
	}

	template<typename T>
	T ExecuteFunction(const string& name)
	{
		return (T)ExecuteFunction(name);
	}

	void AddFunction(const string& name, NativeFunction func);
	void AddFunction(const string& name, std::function<Value(VirtualMachine*, Object*)> func);
	void AddClass(Class* cls);

	~Loris();

private:
	string ReadFile(const char *filename);
};

}