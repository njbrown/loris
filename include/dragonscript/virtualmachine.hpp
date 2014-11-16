/*
Copyright (c) 2013 Nicolas Brown, nclsbrwn@gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#pragma once


#include <string>
#include <vector>
#include <stack>
#include <deque>
#include <map>
#include <assert.h>
#include <iostream>
#include "assembly.hpp"
#include "error.hpp"

namespace dragonscript
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
struct Value;
struct Class;

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
struct Value
{
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
};

class GC;

class Object
{
protected:
	friend class GC;

	map<string,Value> vars;
	map<string,Function*> methods;

	bool isArray;
	
	bool marked;//for gc

public:
	//name of the type
	string typeName;

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
	static Value ArrayObject::GetSize(VirtualMachine* vm,Object* self);

	//def add(val)
	static Value AddEl(VirtualMachine* vm,Object* self);

	//def get(index)
	static Value GetEl(VirtualMachine* vm,Object* self);
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
	static void AddObject(VirtualMachine* vm,Object* obj);

	static void Collect(VirtualMachine* vm);

	static void MarkObject(Object* obj);
	static void MarkArray(ArrayObject* obj);

	static void Sweep();
};


struct ClassAttrib
{
	string name;
	bool isStatic;
	Function* init;
};

struct Class
{
	string name;
	vector<ClassAttrib> attribs;
	//vector<string> functions;
	map<string,Function*> methods;

	string parentName;
	Class* parent;

	int sourceIndex;

	Class()
	{
		parent = 0;
		parentName = "";
		sourceIndex = -1;
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
	NativeFunction nativeFunction;

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
	map<string,Value> locals;
	
	deque<Value> stack;
};

class VirtualMachine
{
	deque<StackFrame*> frames;
	Value ret;

	int lineNo;//line for debugging
	Error error;
	vector<Value> args;

	//contains classes and function definitions
	Assembly* assembly;

	//is this even being used?
	map<string,Value> globals;

	friend class GC;
public:
	VirtualMachine();

	bool HasError();

	Error GetError();

	string GetErrorString();

	int GetErrorLine();

	void SetAssembly(Assembly* assem);

	Object* CreateObject(Class* cls);

	void AddArg(Value val);

	int NumArgs();

	Value GetArg(int index);

	void ClearArgs();
	
	Value ExecuteMemberFunction(Object* obj,string name);

	Value ExecuteFunction(Function* func);

	Value ExecuteNativeFunction(Object* self,Function* func);

	Value ExecuteScriptFunction(Object* self,Function* func);

	void Negate(StackFrame* frame);

	void OpArithmetic(StackFrame* frame,OpCode opcode);

	//todo: compare other values
	void Comparison(StackFrame* frame,OpCode opcode);

	void CreateInstance(StackFrame* frame,string className);

	void CallMethod(StackFrame* frame,string methodName);
	
	void CallFunction(StackFrame* frame,string funcName);

	void RaiseError(string msg);
	void RaiseError(StackFrame* frame,string msg);
	
};

}