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
#include "../include/loris/bind.hpp"

namespace loris
{

template<> Value::operator double() {
	return AsNumber();
}

template<> Value::operator long() {
	return AsNumber();
}

template<> Value::operator std::string() {
	return AsString();
}

Value box(int value)
{
	return Value::CreateNumber(value);
}

Value box(long value)
{
	return Value::CreateNumber(value);
}

Value box(float value)
{
	return Value::CreateNumber(value);
}

Value box(double value)
{
	return Value::CreateNumber(value);
}

Value box(const std::string& value)
{
	return Value::CreateString(value.c_str());
}

Value box(bool value)
{
	return Value::CreateBool(value);
}


ClassBuilder::ClassBuilder()
{
	def = nullptr;
}

ClassBuilder ClassBuilder::Start(string className)
{
	def = new Class();
	def->name = className;

	return *this;
}

ClassBuilder ClassBuilder::Attrib(string name)
{
	assert(def != NULL);

	ClassAttrib attr;
	attr.name = name;
	attr.isStatic = false;
	attr.init = NULL;
	def->attribs.push_back(attr);

	return *this;
}

ClassBuilder ClassBuilder::StaticAttrib(string name)
{
	assert(def != NULL);

	ClassAttrib attr;
	attr.name = name;
	attr.isStatic = true;
	attr.init = NULL;
	def->attribs.push_back(attr);

	return *this;
}

ClassBuilder ClassBuilder::Constructor(NativeFunction native)
{
	Function* func = new Function;
	func->name = def->name;
	func->isStatic = false;
	func->isNative = true;
	func->nativeFunction = native;

	def->methods[def->name] = func;
	return *this;
}

ClassBuilder ClassBuilder::Destructor(NativeFunction native)
{
	Function* func = new Function;
	func->name = def->name;
	func->isStatic = false;
	func->isNative = true;
	func->nativeFunction = native;

	def->destructor = func;
	return *this;
}

ClassBuilder ClassBuilder::Method(string name, NativeFunction native)
{
	Function* func = new Function;
	func->name = def->name;
	func->isStatic = false;
	func->isNative = true;
	func->nativeFunction = native;

	def->methods[name] = func;
	return *this;
}

ClassBuilder ClassBuilder::StaticMethod(string name, NativeFunction native)
{
	Function* func = new Function;
	func->name = def->name;
	func->isStatic = true;
	func->isNative = true;
	func->nativeFunction = native;

	def->methods[name] = func;
	return *this;
}

Class* ClassBuilder::Build()
{
	Class* c = def;
	def = NULL;

	return c;
}

ClassBuilder CreateClass(std::string name)
{
	ClassBuilder builder;
	return builder.Start(name);
}

}
