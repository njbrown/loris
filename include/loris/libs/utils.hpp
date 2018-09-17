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

#include "../assembly.hpp"
#include "../virtualmachine.hpp"
#include <string>

using namespace loris;

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
	lib->AddFunction("str",NativeStr);
	lib->AddFunction("array",NativeArray);
}
}