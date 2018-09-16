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

#include "../include/loris/bind.hpp"

namespace loris
{

template<> Value::operator int() {
	return AsNumber();
}

template<> Value::operator long() {
	return AsNumber();
}

template<> Value::operator float() {
	return AsNumber();
}

template<> Value::operator double() {
	return AsNumber();
}

template<> Value::operator std::string() {
	return AsString();
}

template<> Value::operator bool() {
	return AsBool();
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
