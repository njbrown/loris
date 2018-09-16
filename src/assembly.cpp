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

#include "../include/loris/assembly.hpp"
#include "../include/loris/compiler.hpp"
#include "../include/loris/virtualmachine.hpp"

using namespace loris;

void Assembly::AddClass(Class* cls)
{
	//classes.push_back(cls);
	classes[cls->name] = cls;
}
	
void Assembly::AddFunction(Function* func)
{
	//functions.push_back(func);
	functions[func->name] = func;
}

Class* Assembly::GetClass(string name)
{
	/*
	for(int i=0;i<classes.size();i++)
		if(classes[i]->name == name)
			return classes[i];
	*/

	auto iter = classes.find(name);
	if(iter!=classes.end())
		return iter->second;

	return NULL;
}

Function* Assembly::GetFunction(string name)
{
	/*
	for(int i=0;i<functions.size();i++)
		if(functions[i]->name == name)
			return functions[i];
	*/

	auto iter = functions.find(name);
	if(iter!=functions.end())
		return iter->second;

	return NULL;
}

void Assembly::AddFunction(string name, NativeFunction func)
{
	Function* f = new Function();
	f->name = name;
	f->isNative = true;
	f->nativeFunction = func;

	//functions.push_back(f);
	functions[f->name] = f;
}

void Assembly::AddFunction(string name, std::function<Value(VirtualMachine*, Object*)> func)
{
	Function* f = new Function();
	f->name = name;
	f->isNative = true;
	f->nativeFunction = func;

	//functions.push_back(f);
	functions[f->name] = f;
}