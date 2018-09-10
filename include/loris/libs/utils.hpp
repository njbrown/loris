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

#include "../assembly.hpp"
#include "../virtualmachine.hpp"
#include <string>

using namespace dragonscript;

namespace DSUtilsLib
{

Value NativeStr(VirtualMachine* vm,Object* self)
{
	Value val = vm->GetArg(0);

	switch(val.type)
	{
	case ValueType::Bool:
		if(val.val.b)
			return Value::CreateBool(true);
		return Value::CreateBool(false);
		break;
	case ValueType::Number:
		return Value::CreateString(std::to_string(val.val.num).c_str());
		break;
	case ValueType::String:
		return val;
		break;
	case ValueType::Object:
		return Value::CreateString((std::string("<")+val.val.obj->typeName+">").c_str());
		break;
	case ValueType::Null:
		return Value::CreateString("null");
		break;
	}

	return Value::CreateNull();//shouldnt be here, throw error instead
}

Value NativeArray(VirtualMachine* vm,Object* self)
{
	//ignore args at the moment
	return Value::CreateArray();
}

void Install(Assembly* lib)
{
	lib->AddNativeFunction("str",NativeStr);
	lib->AddNativeFunction("array",NativeArray);
}
}