#include "../include/dragonscript/assembly.hpp"
#include "../include/dragonscript/compiler.hpp"
#include "../include/dragonscript/virtualmachine.hpp"

using namespace dragonscript;

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

void Assembly::AddNativeFunction(string name,NativeFunction func)
{
	Function* f = new Function();
	f->name = name;
	f->isNative = true;
	f->nativeFunction = func;

	//functions.push_back(f);
	functions[f->name] = f;
}