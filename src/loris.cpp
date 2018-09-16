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

#include "../include/loris/loris.hpp"

#include <fstream>
#include <sstream>

using namespace loris;

Loris::Loris()
{
	assembly = new Assembly();
}

void Loris::AddSource(string source)
{
	compiler.AddSource("<source>", source);
}

void Loris::AddSource(string filename, string source)
{
	compiler.AddSource(filename, source);
}

void Loris::AddFileSource(string filename)
{
	AddSource(filename, ReadFile(filename.c_str()));
}

bool Loris::HasError()
{
	return error.code != Error::NONE;
}

Error Loris::GetError()
{
	return error;
}

bool Loris::Compile()
{
	if (!compiler.Compile(assembly))
	{
		error = compiler.GetError();
		return false;
	}

	vm.SetAssembly(assembly);

	return true;
}

Value Loris::ExecuteFunction(const string& name)
{
	//bad! find a way to return an error instead
	if (assembly == NULL)
		return Value::CreateNull();

	Function* func = assembly->GetFunction(name);
	if (func == NULL)
		return Value::CreateNull();

	Value ret = vm.ExecuteFunction(func);

	if (vm.HasError())
		error = vm.GetError();

	return ret;
}

Value Loris::ExecuteFunction(Function* func)
{
	Value ret = vm.ExecuteFunction(func);

	if (vm.HasError())
		error = vm.GetError();

	return ret;
}

void Loris::AddFunction(const string& name, NativeFunction func)
{
	assembly->AddFunction(name, func);
}

void Loris::AddFunction(const string& name, std::function<Value(VirtualMachine*, Object*)> func)
{
	assembly->AddFunction(name, func);
}

void Loris::AddClass(Class* cls)
{
	assembly->AddClass(cls);
}

Loris::~Loris()
{
	delete assembly;
}

string Loris::ReadFile(const char *filename)
{
	ifstream file(filename);
	stringstream stream;
	string line;

	while (file.good())
	{
		getline(file, line);
		stream << line << endl;
	}

	cout << stream.str() << endl;

	return stream.str();
}