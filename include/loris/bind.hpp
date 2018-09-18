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

#include "loris\loris.hpp"

namespace loris {


template<> Value::operator int();
template<> Value::operator long();
template<> Value::operator float();
template<> Value::operator double();
template<> Value::operator std::string();
template<> Value::operator bool();

Value box(int value);

Value box(long value);

Value box(float value);

Value box(double value);

Value box(const std::string& value);

Value box(bool value);

template<typename T>
Value box(T value)
{
	auto obj = new Object();
	obj->managed = false;
	obj->data = (void*)value;
	return Value::CreateObject(obj);
}


template<typename Ret, typename ... Params, size_t ... I>
Ret call_func(Ret(*sig)(Params...),
	std::index_sequence<I...>, VirtualMachine* vm)
{
	return sig(vm->GetArg(I)...);
}

template<typename Ret, typename ... Params>
std::function<Value(VirtualMachine* vm, Object* self)> Def(Ret(*sig)(Params...))
{
	if (std::is_same<Ret, void>::value) {
		return [=](VirtualMachine* vm, Object* self)
		{
			call_func(sig, std::index_sequence_for<Params...>{}, vm);
			return Value::CreateNull();
		};
	}
	else {
		return [=](VirtualMachine* vm, Object* self)
		{
			return box(call_func(sig, std::index_sequence_for<Params...>{}, vm));
		};
	}
}

class ClassBuilder
{
public:
	Class * def;

	ClassBuilder();

	ClassBuilder Start(string className);

	ClassBuilder Attrib(string name);

	ClassBuilder StaticAttrib(string name);

	ClassBuilder Constructor(std::function<Value(VirtualMachine*, Object*)> native);

	ClassBuilder Destructor(std::function<Value(VirtualMachine*, Object*)> native);

	ClassBuilder Method(string name, std::function<Value(VirtualMachine*, Object*)> native);

	ClassBuilder StaticMethod(string name, std::function<Value(VirtualMachine*, Object*)> native);

	Class* Build();
};

ClassBuilder CreateClass(std::string name);
}