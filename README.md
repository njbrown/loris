# Loris
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat-square)](https://github.com/njbrown/loris/issues)
[![HitCount](http://hits.dwyl.io/njbrown/loris.svg)](https://hits.dwyl.io/njbrown/loris)
[![License](https://img.shields.io/badge/license-MIT-brightgreen.svg?style=flat-square)](https://github.com/njbrown/loris/blob/master/LICENSE)

Easy scripting in C++.

## Features of Loris

*	Simple and familiar syntax
*	Object Oriented
*	Auto-binding of c++ functions to Loris
*	Mark and Sweep Garbage Collection
*	Easy to embed in c++ applications
*	Easy to extend

## Future Features
* 	Pre-compilation

## Syntax

see https://github.com/njbrown/dragonscript/blob/master/SYNTAX.md

## Example Script

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


## Example Usage

	#include "loris/loris.hpp"


	double multiply(double a, double b)
	{
		return a * b;
	}

	int main()
	{
		loris::Loris loris;
		loris.AddSource(R"(
		def hello()
		{
			return multiply(5, 10);			
		}
		)");

		loris.AddFunction("multiply", loris::Def(multiply));
		loris.Compile();

		double result = loris.ExecuteFunction<double>("hello");
	}

## Known Issues
* `or` and `and` statements are parsed but the bytecode isn't generated and will cause errors if used
* garbage collection sometimes causes random crashes