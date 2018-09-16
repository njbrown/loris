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

#include <vector>
#include <unordered_map>
#include <string>
#include <functional>

using namespace std;

namespace loris
{

class Class;
struct Function;
class Value;
class VirtualMachine;
class Object;

typedef Value (*NativeFunction)(VirtualMachine* vm,Object* self);

//assembly contains list of classes and functions
class Assembly
{
public:
	//list of source code names
	vector<string> sourceNames;

	unordered_map<string,Class*> classes;
	unordered_map<string,Function*> functions;

	void AddClass(Class* cls);
	
	void AddFunction(Function* func);
	void AddFunction(string name,NativeFunction func);
	void AddFunction(string name, std::function<Value(VirtualMachine*, Object*)> func);

	Class* GetClass(string name);
	Function* GetFunction(string name);
};

}