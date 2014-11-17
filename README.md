DragonRcript
============

Example
======

hello.ds

	class Hello
	{
		Hello()
		{
			print("this is a constructor");
		}

		def Greet()
		{
			print("hello world!");
		}
	}

	def main()
	{
		var hello = new Hello();
		hello.Greet();
	}


Usage
=====

	#include "dragonscript/dragonscript.hpp"

	using namespace dragonscript;

	Value print(VirtualMachine* vm,Object* self)
	{
		for(int i=0;i<vm->NumArgs();i++)
			vm->GetArg(i).Print();

		return Value::CreateNull();
	}

	int main()
	{
		DragonScript ds;
		ds.AddSource("hello.ds",ReadFile("scripts/hello.ds"));

		if(!ds.Compile())
		{
			Error e = ds.GetError();
			cout<<"error in file"<<e.filename <<" on line "<<e.line<<endl;
			cout<<e.message<<endl;
		}
		
		ds.AddNativeFunction("print",print);
		ds.ExecuteFunction("main");

		system("pause");
	}