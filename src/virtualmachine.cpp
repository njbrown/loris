#include <algorithm>
#include "../include/dragonscript/virtualmachine.hpp"

using namespace dragonscript;

Value::Value()
{
	type = ValueType::Null;
}

Value::Value(const Value& other)
{
	type = other.type;

	//if its a string, copy over the value
	if(type == ValueType::String)
	{
		int len = strlen(other.val.str);
		char* str = new char[len+1];
		strcpy(str,other.val.str);
		str[len]='\0';
		val.str = str;
	}
	else
	{
		val = other.val;
	}
}

Value& Value::operator=(const Value& other)
{
	type = other.type;

	//if its a string, copy over the value
	if(type == ValueType::String)
	{
		int len = strlen(other.val.str);
		char* str = new char[len+1];
		strcpy(str,other.val.str);
		str[len]='\0';
		val.str = str;
	}
	else
	{
		val = other.val;
	}

	return *this;
}

double Value::AsNumber()
{
	return val.num;
}

bool Value::AsBool()
{
	return val.b;
}

const char* Value::AsString()
{
	return val.str;
}

Object* Value::AsObject()
{
	return val.obj;
}

ArrayObject* Value::AsArray()
{
	return val.arr;
}

//display
void Value::Print(bool newLine)
{
	const char* lineEnd = newLine?"\n":"";

	switch (type)
	{
	case ValueType::Number:
		cout<<val.num<<lineEnd;
		break;
	case ValueType::Bool:
		if(val.b)
			cout<<"true"<<lineEnd;
		else
			cout<<"false"<<lineEnd;
		break;
	case ValueType::String:
		cout<<val.str<<lineEnd;
		break;
	case ValueType::Object:
		cout<<"<Object>"<<lineEnd;
		break;
	case ValueType::Null:
		cout<<"null"<<lineEnd;
		break;
	default:
		break;
	}
}

Value::~Value()
{
	//free up string data is var is a string
	if(type == ValueType::String)
		delete[] val.str;
}

Value Value::CreateNull()
{
	Value v;
	v.type = ValueType::Null;
	return v;
}

Value Value::CreateBool(bool val)
{
	Value v;
	v.type = ValueType::Bool;
	v.val.b = val;
	return v;
}

Value Value::CreateNumber(double val)
{
	Value v;
	v.type = ValueType::Number;
	v.val.num = val;
	return v;
}

Value Value::CreateObject(Object* obj)
{
	Value v;
	v.type = ValueType::Object;
	v.val.obj = obj;

	return v;
}

Value Value::CreateString(const char* val)
{
	int len = strlen(val);
	char* str = new char[len+1];
	strcpy(str,val);
	str[len]='\0';

	Value v;
	v.type = ValueType::String;
	v.val.str = str;

	return v;
}

/* OBJECT */

Object::Object()
{
	marked = false;
	isArray = false;
}

bool Object::HasAttrib(string name)
{
	map<string,Value>::iterator iter = vars.find(name);
	return iter!=vars.end();
}

Value Object::GetAttrib(string name)
{
	return vars[name];
}

void Object::SetAttrib(string name,Value value)
{
	vars[name] = value;
}

void Object::SetMethod(string name,Function* func)
{
	methods[name] = func;
}

Function* Object::GetMethod(string name)
{
	if(!HasMethod(name))
		return NULL;
	return methods[name];
}

bool Object::HasMethod(string name)
{
	map<string,Function*>::iterator iter = methods.find(name);
	return iter!=methods.end();
}

/* VIRTUAL MACHINE */

VirtualMachine::VirtualMachine()
{
	lineNo = 0;
}

bool VirtualMachine::HasError()
{
	return error.code!=Error::NONE;
}

Error VirtualMachine::GetError()
{
	return error;
}

string VirtualMachine::GetErrorString()
{
	return error.message;
}

int VirtualMachine::GetErrorLine()
{
	return error.line;
}

void VirtualMachine::SetAssembly(Assembly* assem)
{
	assembly = assem;

	//load all the classes as objects
	for(auto iter = assem->classes.begin();iter!=assem->classes.end();iter++)
	{
		Value classObj = Value::CreateClass(this,iter->second);
		globals[iter->first] = classObj;
	}
}

Object* VirtualMachine::CreateObject(Class* cls)
{
	Object* obj=0;
	if(cls->parent)
	{
		obj = CreateObject(cls->parent);
		obj->typeName = cls->name;
	}
	else
	{
		obj = new Object;
		obj->typeName = cls->name;
	}

	//copy functions and attribs

	//attribs
	for(size_t j=0;j<cls->attribs.size();j++)
	{
		//only add non-static vars to instance
		if(!cls->attribs[j].isStatic)
			obj->SetAttrib(cls->attribs[j].name,Value::CreateNull());
	}

	//functions
	map<string,Function*>::iterator iter;
	for(iter = cls->methods.begin();iter!=cls->methods.end();iter++)
	{
		obj->SetMethod(iter->first,iter->second);
	}

	return obj;
}

void VirtualMachine::AddArg(Value val)
{
	args.push_back(val);
}

int VirtualMachine::NumArgs()
{
	return args.size();
}

Value VirtualMachine::GetArg(unsigned int index)
{
	if(index<args.size())
		return args[index];

	return Value::CreateNull();
}

void VirtualMachine::ClearArgs()
{
	args.clear();
}

	
Value VirtualMachine::ExecuteMemberFunction(Object* obj,string name)
{
	Function* func = obj->GetMethod(name);
	if(func==NULL)
		return Value::CreateNull();

	if(func->isNative)
		return ExecuteNativeFunction(obj,func);

	return ExecuteScriptFunction(obj,func);
}

Value VirtualMachine::ExecuteFunction(Function* func)
{
	if(func->isNative)
		return ExecuteNativeFunction(NULL,func);
	else
		return ExecuteScriptFunction(NULL,func);
}

Value VirtualMachine::ExecuteNativeFunction(Object* self,Function* func)
{
	//todo
	assert(func->isNative);
	assert(func->nativeFunction!=NULL);

	//reverse args
	std::reverse(args.begin(),args.end());

	Value val =  func->nativeFunction(this,self);
	args.clear();
		

	return val;
}

Value VirtualMachine::ExecuteScriptFunction(Object* self,Function* func)
{
	Value ret = Value::CreateNull();//value returned from called function
	//Value retured;//value returned from this function

	//init stackframe before execution
	StackFrame* frame = new StackFrame;
	frame->function = func;

	if(self!=NULL)
		frame->locals["self"]=Value::CreateObject(self);

	//args
	//todo: rename func->args to func->params
	//adding args in reverse
	//see function call in Compiler.h
	size_t paramSize = func->args.size();
	for(size_t i=0;i<paramSize;i++)
	{
		if(i<args.size())
			frame->locals[func->args[i]]=args[paramSize-1-i];
		else
			frame->locals[func->args[i]]=Value::CreateNull();
	}

	args.clear();

	//no need for this, language is dynamically typed
	//for(int i=0;i<func->numLocals;i++)
	//	frame->locals.push_back(Value());
	frames.push_back(frame);

	//switch vars
	Value val;

	//start execution
	int cpSize = func->instr.size();
	int cp = 0; //code/instruction pointer
	while(cp<cpSize && error.code==Error::NONE)
	{
		DSInstr instr = func->instr[cp];
		switch(instr.op)
		{
		case OpCode::Pop:
			frame->stack.pop_back();
			break;
		case OpCode::Line:
			lineNo = instr.val;
			break;
			/* COMPARISONS */
		case OpCode::IsGreaterThan:
		case OpCode::IsLessThan:
		case OpCode::IsGreaterThanOrEqual:
		case OpCode::IsLessThanOrEqual:
		case OpCode::IsEqual:
		case OpCode::IsNotEqual:
			Comparison(frame,instr.op);
			break;
		/* ARITHMETIC */
		case OpCode::Add:
		case OpCode::Sub:
		case OpCode::Mul:
		case OpCode::Div:
			OpArithmetic(frame,instr.op);
			break;
		case OpCode::Neg:
			Negate(frame);
			break;
		/* JUMPS */
		case OpCode::Jump:
			cp = instr.val-1;//cp gets incremented at the end of the loop
			break;
		case OpCode::JumpIfTrue:
			val = frame->stack.back();
			frame->stack.pop_back();
			if(val.type == ValueType::Bool && val.val.b)
				cp = instr.val-1;//cp gets incremented at the end of the loop
			break;
		case OpCode::JumpIfFalse:
			val = frame->stack.back();
			frame->stack.pop_back();
			if(val.type == ValueType::Bool && !val.val.b)
				cp = instr.val-1;//cp gets incremented at the end of the loop
			break;
		/* LOADING AND STORING VALUES */
		case OpCode::LoadConstant:
			frame->stack.push_back(frame->function->constants[instr.val]);
			break;
		case OpCode::LoadLocal:
			if(frame->locals.find(func->strings[instr.val])!=frame->locals.end())//if it's a local, add it to stack
				frame->stack.push_back(frame->locals[func->strings[instr.val]]);
			else
			{
				//check if it's a class being referred
					
				if(globals.find(func->strings[instr.val])!=globals.end())
				{
					//add global if available
					frame->stack.push_back(globals[func->strings[instr.val]]);
				}
				else
				{
					//push null if it doesnt exist
					frame->stack.push_back(Value::CreateNull());
				}
			}
			break;
		case OpCode::StoreLocal:
			val = frame->stack.back();
			frame->stack.pop_back();
			frame->locals[func->strings[instr.val]] = val;
			break;
		case OpCode::LoadBool:
			frame->stack.push_back(Value::CreateBool(instr.val==1));
			break;
		case OpCode::LoadNull:
			frame->stack.push_back(Value::CreateNull());
			break;
		case OpCode::LoadProp:
			val = frame->stack.back();
			frame->stack.pop_back();
			/*
			//should never happen, only in the case of a null
			if(val.type!=ValueType::Object)
			{
				errors.push_back(Error::InvalidOperation("cannot get property from non object"));
				break;
			}
			*/
			frame->stack.push_back(val.val.obj->GetAttrib(func->strings[instr.val]));
			break;
		case OpCode::StoreProp:
			//object is at the top of the stack
			val = frame->stack.back();
			frame->stack.pop_back();

			//followed by the value to be stored
			val.val.obj->SetAttrib(func->strings[instr.val],frame->stack.back());
			frame->stack.pop_back();//pop top value

			break;

		case OpCode::CreateInstance:
			CreateInstance(frame,func->strings[instr.val]);
			break;
				
		case OpCode::CallMethod:
			CallMethod(frame,func->strings[instr.val]);
			break;
				
		case OpCode::CallFunction:
			CallFunction(frame,func->strings[instr.val]);
			break;

		case OpCode::AddArg:
			//pop value at top of stack and add to args
			val = frame->stack.back();
			frame->stack.pop_back();

			AddArg(val);
			break;
				
		case OpCode::Return:
			ret = frame->stack.back();
			frame->stack.pop_back();

			frames.pop_back();
			delete frame;
			return ret;
		default:
			//dont execute any op we dont know
			break;
		}

		//increase counter
		cp++;
	}

	frames.pop_back();
	delete frame;

	return Value::CreateNull();
}

void VirtualMachine::Negate(StackFrame* frame)
{
	Value a = frame->stack.back();
	frame->stack.pop_back();

	if(a.type == ValueType::Number)
	{
		a.val.num = -a.val.num;
		frame->stack.push_back(a);
	}
	else
	{
		VM_ERROR("negation can only be done to numbers");
	}
}

void VirtualMachine::OpArithmetic(StackFrame* frame,OpCode opcode)
{
	Value b = Value(frame->stack.back());
	frame->stack.pop_back();
	Value a = Value(frame->stack.back());
	frame->stack.pop_back();

	Value res;

	//arithmetic can only be done on number vars
	if(b.type == ValueType::Number && a.type == ValueType::Number)
	{
		res.type = ValueType::Number;
		switch(opcode)
		{
		case OpCode::Add:
			res.val.num = a.val.num + b.val.num;break;
		case OpCode::Sub:
			res.val.num = a.val.num - b.val.num;break;
		case OpCode::Mul:
			res.val.num = a.val.num * b.val.num;break;
		case OpCode::Div:
			res.val.num = a.val.num / b.val.num;break;
		}
	}
	else if(b.type == ValueType::String && a.type == ValueType::String)
	{
		if(opcode==OpCode::Add)
		{
			char* str = new char[strlen(a.val.str)+strlen(b.val.str)+1];
			strcpy(str,a.val.str);
			strcat(str,b.val.str);

			res.type = ValueType::String;
			res.val.str = str;
		}
		else
		{
			VM_ERROR("invalid operation between strings");
		}
	}
	else
	{
		VM_ERROR("invalid math operation");
	}

	frame->stack.push_back(res);
}

void VirtualMachine::Comparison(StackFrame* frame,OpCode opcode)
{
	Value b = Value(frame->stack.back());
	frame->stack.pop_back();
	Value a = Value(frame->stack.back());
	frame->stack.pop_back();

	Value res;
	res.type = ValueType::Bool;

	//only number comparisons for now
	if(b.type == ValueType::Number && a.type == ValueType::Number)
	{
		switch(opcode)
		{
			case OpCode::IsGreaterThan:
				res.val.b = a.val.num > b.val.num;break;
			case OpCode::IsLessThan:
				res.val.b = a.val.num < b.val.num;break;
			case OpCode::IsGreaterThanOrEqual:
				res.val.b = a.val.num >= b.val.num;break;
			case OpCode::IsLessThanOrEqual:
				res.val.b = a.val.num <= b.val.num;break;
			case OpCode::IsEqual:
				res.val.b = a.val.num == b.val.num;break;
			case OpCode::IsNotEqual:
				res.val.b = a.val.num != b.val.num;break;
		}
	}
	else if(b.type == ValueType::Bool && a.type == ValueType::Bool)
	{
		switch(opcode)
		{
			case OpCode::IsEqual:
				res.val.b = a.val.b == b.val.b;break;
			case OpCode::IsNotEqual:
				res.val.b = a.val.b != b.val.b;break;
			default:
				VM_ERROR("invalid operation between bools");
				break;
		}
	}else if(b.type == ValueType::String && a.type == ValueType::String)
	{
		res.val.b = strcmp(b.val.str,a.val.str)==0;
	}
	else
	{
		VM_ERROR("invalid comparison");
	}

	frame->stack.push_back(res);
}

void VirtualMachine::CreateInstance(StackFrame* frame,string className)
{
	assert(assembly!=NULL);

	Class* cls = this->assembly->GetClass(className);
	//assert(cls!=NULL);
	VM_ASSERT(cls!=NULL,"class "+className+" not found");

	Object* obj = CreateObject(cls);
	//assert(obj!=NULL);
	VM_ASSERT(obj!=NULL,"error creating instance of "+className+". report this immediately!");


	//call constructor if available
	if(obj->HasMethod(className))
	{
		this->ExecuteMemberFunction(obj,className);
	}

		
	frame->stack.push_back(Value::CreateObject(obj));
	GC::AddObject(this,obj);
}

void VirtualMachine::CallMethod(StackFrame* frame,string methodName)
{
	//get self
	Value var = Value(frame->stack.back());
	frame->stack.pop_back();

	//must be a object
	//assert(var.type == ValueType::Object);
	VM_ASSERT(var.type == ValueType::Object || var.type == ValueType::Array ,"attemped to call a method from a non-Object type");

	//must contain method
	//assert(var.val.obj->HasMethod(methodName));
	VM_ASSERT(var.val.obj->HasMethod(methodName),"object doesnt have method "+methodName);

	Value ret = this->ExecuteMemberFunction(var.val.obj,methodName);
	frame->stack.push_back(ret);
}

void VirtualMachine::CallFunction(StackFrame* frame,string funcName)
{
	//VM_ASSERT(assembly!=NULL,"assembly not set");

	Function* func = assembly->GetFunction(funcName);
	//assert(func!=NULL);
	VM_ASSERT(func!=NULL,"function "+funcName+" not found");

	Value ret = this->ExecuteFunction(func);
	frame->stack.push_back(ret);
}

void VirtualMachine::RaiseError(string msg)
{
	RaiseError(frames.back(),msg);
}

void VirtualMachine::RaiseError(StackFrame* frame,string msg)
{
	error = Error();
	error.message = msg;
	error.line = lineNo;

	if(frame->function->sourceIndex>=0)
		error.filename = assembly->sourceNames[frame->function->sourceIndex];
		
}

/* Garbage Collector */
vector<Object*> GC::objects;

void GC::AddObject(VirtualMachine* vm,Object* obj)
{
	objects.push_back(obj);

	if(objects.size()>1000)
	{
		Collect(vm);
	}
}

void GC::Collect(VirtualMachine* vm)
{
	//search through stack and mark objects
	for(size_t s = 0;s<vm->frames.size();s++)
	{
		StackFrame* frame = vm->frames[s];

		for(size_t f = 0;f<frame->stack.size();f++)
		{
			if(frame->stack[f].type == ValueType::Object)
				MarkObject(frame->stack[f].AsObject());
			if(frame->stack[f].type == ValueType::Array)
				MarkArray(frame->stack[f].AsArray());
		}
	}

	//sweep
	Sweep();
}

void GC::MarkObject(Object* obj)
{
	obj->marked = true;


	for(auto var:obj->vars)
	{
		if(var.second.type == ValueType::Object)
		{
			MarkObject(var.second.AsObject());
		}
		else if(var.second.type == ValueType::Array)
		{
			MarkArray(var.second.AsArray());
		}
	}
}

void GC::MarkArray(ArrayObject* arr)
{
	arr->marked = true;

	//vars could be added to the array
	for(auto var:arr->vars)
	{
		if(var.second.type == ValueType::Object)
		{
			MarkObject(var.second.AsObject());
		}
		else if(var.second.type == ValueType::Array)
		{
			MarkArray(var.second.AsArray());
		}
	}

	//elements in the array
	for(auto var:arr->elements)
	{
		if(var.type == ValueType::Object)
		{
			MarkObject(var.AsObject());
		}
		else if(var.type == ValueType::Array)
		{
			MarkArray(var.AsArray());
		}
	}

}

void GC::Sweep()
{
	//remove all unmarked objects
	for(auto iter = objects.begin();iter!=objects.end();)
	{
		if((*iter)->marked)
		{
			//unmark for next sweep
			(*iter)->marked = false;
			iter++;
		}
		else
		{
			//erase returns next valid pointer
			iter = objects.erase(iter);
		}
	}
}

class Object;
Value Value::CreateClass(VirtualMachine* vm,Class* cls)
{
	Value v;
	v.type = ValueType::Object;
	Object *obj = new Object;
	
	//add each static attrib as a var
	for(auto i = cls->attribs.begin();i!=cls->attribs.end();i++)
	{
		if(i->isStatic)
		{
			if(i->init)
			{
				obj->SetAttrib(i->name,vm->ExecuteFunction(i->init));
			}
			else
			{
				obj->SetAttrib(i->name,Value::CreateNull());
			}
		}
	}

	for(auto i=cls->methods.begin();i!=cls->methods.end();i++)
	{
		if(i->second->isStatic)
		{
			obj->SetMethod(i->first,i->second);
		}
	}
	
	v.val.obj = obj;

	return v;

}

Value Value::CreateArray()
{
	Value v;
	v.type = ValueType::Array;
	v.val.obj = new ArrayObject;

	return v;
}

ArrayObject::ArrayObject()
{
	isArray = true;

	typeName = "Array";

	Function* func = new Function;
	func->isNative = true;
	func->nativeFunction = AddEl;
	func->name = "add";
	func->args.push_back("val");
	SetMethod("add",func);

	func = new Function;
	func->isNative = true;
	func->nativeFunction = GetEl;
	func->name = "get";
	func->args.push_back("index");
	SetMethod("get",func);

	func = new Function;
	func->isNative = true;
	func->nativeFunction = GetSize;
	func->name = "size";
	SetMethod("size",func);
}

//def size()
Value ArrayObject::GetSize(VirtualMachine* vm,Object* self)
{
	ArrayObject* arr = (ArrayObject*)self;
	return Value::CreateNumber(arr->elements.size());
}

//def add(val)
Value ArrayObject::AddEl(VirtualMachine* vm,Object* self)
{
	Value value = vm->GetArg(0);

	//should never happen, but just in case
	ArrayObject* arr = (ArrayObject*)self;
	arr->elements.push_back(value);

	return value;
}

//def get(index)
Value ArrayObject::GetEl(VirtualMachine* vm,Object* self)
{
	//ensure the index is a number
	Value index = vm->GetArg(0);
	if(index.type!=ValueType::Number)
	{
		vm->RaiseError("index should only be an integer");
		return Value::CreateNull();
	}

	//ensure it's within the bounds of the array
	int ind = (int)index.val.num;
	ArrayObject* arr = (ArrayObject*)self;

	if(ind<0 || ind>=(int)arr->elements.size())
	{
		vm->RaiseError("index out of bounds");
		return Value::CreateNull();
	}

	return arr->elements[ind];
}