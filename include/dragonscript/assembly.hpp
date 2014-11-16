#pragma once

#include <vector>
#include <map>
#include <string>

using namespace std;

namespace dragonscript
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

	map<string,Class*> classes;
	map<string,Function*> functions;

	void AddClass(Class* cls);
	
	void AddFunction(Function* func);

	void AddNativeFunction(string name,NativeFunction func);

	Class* GetClass(string name);
	Function* GetFunction(string name);
};

}