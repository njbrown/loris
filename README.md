DragonScript
============
DragonScript is an experimental language I work on in my spare time. It is not optimized for speed at the moment and has known memory leaks. The syntax is coincidentally a subset of ChaiScript's. The code is also a mess and lacks documentation. That will all change over the course of its development. If you do use this library and you find any bugs please let me know. It would be really helpful.


Syntax Example
=============

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

Note
====
The API is subject to change. As I add more features and find ways to simplify common tasks I'll modify the API to accomodate these changes.

Why the weird name?
===================
DragonScript was designed originally to be used in a game engine I was working on named Dragon (yes, i like dragons). I wont change the language's name as long as it's still relevant to the game engine. So :p
