#pragma once

#include "assembly.hpp"
#include "virtualmachine.hpp"
#include <string>

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