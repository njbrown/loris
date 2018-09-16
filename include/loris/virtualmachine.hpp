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


#include <string>
#include <vector>
#include <stack>
#include <deque>
#include <unordered_map>
#include <assert.h>
#include <iostream>
#include "string.h"
#include "assembly.hpp"
#include "error.hpp"

namespace loris
{
#define VM_ASSERT(check,msg) if(!(check)){this->RaiseError(frame,msg);return;}
#define VM_ERROR(msg){this->RaiseError(frame,msg);return;}

using namespace std;

typedef char byte;

struct DSInstr;
class Object;
struct ArrayObject;
struct Function;
class VirtualMachine;
class Value;
class Class;

struct ValueType
{
	enum Enum
	{
		Number,
		Bool,
		String,
		Object,
		Array,
		Null,
	};
};

//strings are immutable now!
class Value
{
public:
	ValueType::Enum type;

	union
	{
		double num;
		bool b;
		char* str;
		void* data;
		Object* obj;
		ArrayObject* arr;
	}val;

	bool marked;//for mark and sweep

	
	Value(const Value& other);
	Value& operator=(const Value& other);
	Value();

	//conversions
	double AsNumber();

	bool AsBool();

	const char* AsString();

	Object* AsObject();

	ArrayObject* AsArray();

	//display
	void Print(bool newLine=true);
	~Value();

	static Value CreateNull();

	static Value CreateBool(bool val);

	static Value CreateNumber(double val);
	static Value CreateString(const char* val);

	static Value CreateObject(Object* obj);

	static Value CreateArray();

	static Value CreateClass(VirtualMachine* vm,Class* cls);

	template<typename T>
	operator T()
	{
		if (this->type == ValueType::Object)
			return (T)this->val.obj;
		return T();
	}
};

class GC;

class Object
{
protected:
	friend class GC;

	unordered_map<string,Value> vars;
	unordered_map<string,Function*> methods;

	bool isArray;
	
	bool marked;//for gc, mark and sweep

public:
	//name of the type
	string typeName;

	//if set to true, data will be deleted upon 
	bool manageData;
	//for storing native data
	void* data;

	//should be true by default
	//true means it's managed by the engine and can be GC'd
	//false means user has to do the cleaning up
	bool managed;

	//destrutcor
	Function* destructor;

	Object();
	bool HasAttrib(string name);
	Value GetAttrib(string name);
	void SetAttrib(string name,Value value);
	void SetMethod(string name,Function* func);
	Function* GetMethod(string name);
	bool HasMethod(string name);
};

struct ArrayObject:public Object
{
	vector<Value> elements;

	ArrayObject();

	//def size()
	static Value GetSize(VirtualMachine* vm,Object* self);

	//def add(val)
	static Value AddEl(VirtualMachine* vm,Object* self);

	//def get(index)
	static Value GetEl(VirtualMachine* vm,Object* self);

	//def remove(item)
	static Value RemoveEl(VirtualMachine* vm,Object* self);

	//def remove_at(index)
	static Value RemoveAt(VirtualMachine* vm,Object* self);
};

/*
Garbage Collector
*/
class GC
{
	//not ideal for this kinda thing
	//but it's quick, dirty and gets the job done
	static vector<Object*> objects;
public:
	static void AddObject(VirtualMachine* vm,Object* obj,bool doGC=true);

	static void Collect(VirtualMachine* vm);

	static void MarkObject(Object* obj);
	static void MarkArray(ArrayObject* obj);

	static void Sweep(VirtualMachine* vm);
};


struct ClassAttrib
{
	string name;
	bool isStatic;
	Function* init;//for attribs specified with an expression
};

class Class
{
public:
	string name;
	vector<ClassAttrib> attribs;
	//vector<string> functions;
	unordered_map<string,Function*> methods;

	//only invoked by gc
	Function* destructor;

	string parentName;
	Class* parent;

	int sourceIndex;

	Class()
	{
		parent = nullptr;
		parentName = "";
		sourceIndex = -1;

		destructor = nullptr;
	}

	Function* GetMethod(string name)
	{
		//todo: check if method exists
		return methods[name];
	}
};

struct Function
{
	string name;
	vector<string> strings;//for calling classes and other stuff
	vector<Value> constants;
	//vector<DValue> locals;
	int numLocals;

	vector<string> args;
	vector<DSInstr> instr;//instructions

	bool isNative;
	std::function<Value(VirtualMachine*, Object*)> nativeFunction;

	bool isStatic;

	int sourceIndex;

	Function()
	{
		isNative = false;
		nativeFunction = nullptr;

		sourceIndex = 1;
		isStatic = false;
	}
};


enum class OpCode:byte
{
	//math
	Add,
	Sub,
	Mul,
	Div,
	Neg,

	//loading of variables
	LoadConstant,
	LoadLocal,
	StoreLocal,
	LoadProp,//value = prop name string index, stack top = object, stack top -1 = value
	StoreProp,
	LoadBool,
	LoadNull,
	//unconditionally remove top var, used for lhs expressions those returned values dont get used
	Pop,

	//objects
	CreateInstance,
	CallMethod,
	CallStaticMethod,
	CallFunction,
	AddArg,

	//comparison
	IsEqual,
	IsLessThan,
	IsLessThanOrEqual,
	IsGreaterThan,
	IsGreaterThanOrEqual,
	IsNotEqual,

	//jumps
	JumpIfTrue,
	JumpIfFalse,
	Jump,

	//return
	Return,

	Line,//for debugging
	Nop,//(no operation) does nothing, helps with generating if,while and for statements

};

struct DSInstr
{
	OpCode op;
	short val;
};

struct StackFrame
{
	Function* function;
	unordered_map<string,Value> locals;
	
	deque<Value> stack;

public:
	StackFrame()
	{
		function=nullptr;
	}
};

class VirtualMachine
{
	deque<StackFrame*> frames;
	vector<StackFrame*> allocatedFrames;

	Value ret;

	int lineNo;//line for debugging
	Error error;
	vector<Value> args;

	//contains classes and function definitions
	Assembly* assembly;

	//is this even being used?
	unordered_map<string,Value> globals;

	Value nullVal;
	Value selfVal;

	friend class GC;
public:
	VirtualMachine();

	StackFrame* GetStackFrame();
	void ReturnStackFrame(StackFrame* frame);

	bool HasError();

	Error GetError();

	string GetErrorString();

	int GetErrorLine();

	void SetAssembly(Assembly* assem);

	//wth this, objects being created from c++ dont risk the chance of being GC'ed while being instantiated
	Object* CreateNativeObject(Class* cls,bool addToGC = true);
	
	Object* CreateObject(Class* cls,bool addToGC = true,bool doGC = true);
	void DestroyObject(Object* obj);

	void AddArg(Value val);

	int NumArgs();

	Value GetArg(unsigned int index);

	void ClearArgs();
	
	Value ExecuteMemberFunction(Object* obj,string name);

	Value ExecuteFunction(Function* func);

	Value ExecuteNativeFunction(Object* self,Function* func);

	Value ExecuteScriptFunction(Object* self,Function* func);

	inline void Negate(StackFrame* frame);

	inline void OpArithmetic(StackFrame* frame,OpCode opcode);

	//todo: compare other values
	inline void Comparison(StackFrame* frame,OpCode opcode);

	inline void CreateInstance(StackFrame* frame,string className);

	inline void CallMethod(StackFrame* frame,string methodName);
	
	inline void CallFunction(StackFrame* frame,string funcName);

	void RaiseError(string msg);
	void RaiseError(StackFrame* frame,string msg);
	void ClearError();
	
};

}