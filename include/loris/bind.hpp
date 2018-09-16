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

#include "loris\loris.hpp"

namespace loris {


template<> Value::operator double();
template<> Value::operator long();
template<> Value::operator std::string();

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

	ClassBuilder Constructor(NativeFunction native);

	ClassBuilder Destructor(NativeFunction native);

	ClassBuilder Method(string name, NativeFunction native);

	ClassBuilder StaticMethod(string name, NativeFunction native);

	Class* Build();
};

ClassBuilder CreateClass(std::string name);
}