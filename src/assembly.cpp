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
#include "../include/loris/assembly.hpp"
#include "../include/loris/compiler.hpp"
#include "../include/loris/virtualmachine.hpp"

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