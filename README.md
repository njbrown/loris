# What's Loris

Loris, previously DragonScript, is a test language I built in order to learn the inner workings of a scripting language.

## Features of Loris

*	Simple and familiar syntax
*	Object Oriented
*	Mark and Sweep Garbage Collection
*	Easy to embed in c++ applications
*	Easy to extend

## Future Features
* 	Pre-compilation
*	Auto-binding of c++ functions to Loris

## Syntax

see https://github.com/njbrown/dragonscript/blob/master/SYNTAX.md

## Example

hello.ls

	//class named Hello
	class Hello
	{
		//constructor
		Hello()
		{
			print("this is a constructor");
		}
		
		//member function
		def Greet()
		{
			//call print function
			print("hello world!");
		}
	}

	def main()
	{
		var hello = new Hello();
		hello.Greet();
	}


## Usage

	#include "loris/loris.hpp"


	Value print(VirtualMachine* vm,Object* self)
	{
		for(int i=0;i<vm->NumArgs();i++)
			vm->GetArg(i).Print();

		return Value::CreateNull();
	}

	int main()
	{
		loris::Loris ls;
		ls.AddSource("hello.ls",ReadFile("scripts/hello.ls"));

		if(!ls.Compile())
		{
			loris::Error e = ls.GetError();
			cout<<"error in file"<<e.filename <<" on line "<<e.line<<endl;
			cout<<e.message<<endl;
		}
		
		ls.AddNativeFunction("print",print);
		ls.ExecuteFunction("main");

		system("pause");
	}

## Note

Loris is an experimental language I work on in my spare time. It is not optimized for speed at the moment and has known memory leaks. The syntax is coincidentally a subset of ChaiScript's. The code is also a mess and lacks documentation. That will all change over the course of its development. If you do use this library and you find any bugs please let me know. It would be really helpful.

The API is subject to change. As I add more features and find ways to simplify common tasks I'll modify the API to accomodate these changes.

## Why the weird name?

The vm is pretty slow. Since giving scripting languages animal names is a trend, I go with the Slow Loris. :D
I might make an effort to speed it up in the future but it's not a priority at the moment.

## Known Issues
* `or` and `and` statements are parsed but the bytecode isn't generated and will cause errors if used
* garbage collection sometimes causes random crashes