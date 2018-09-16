/*

Copyright (C) 2014-2018 Nicolas Brown

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
#pragma once


#include "virtualmachine.hpp"
#include "compiler.hpp"
#include "error.hpp"
#include "bind.hpp"


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