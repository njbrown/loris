Each scripts main scope can contain either classes or functions. Variables can only be declared within functions.

Functions
=========

Functions are declared with the def  keyword.
e.g.

	def SimpleFunction()
	{
	}

	def FuncWithArgs(a,b,c)
	{
	}

	def Add(a,b)
	{
	    return a+b;
	}

The number of arguments passed to a function doesnt have to be the same as the number of parameters in its definition.

	def ArgsTest(a,b)
	{
	    print(a);
	    print(b);
	}

The following should print two nulls:

	ArgsTest();

The following should print 1 and 2:

	ArgsTest(1,2);

the following should also print 1 and 2:

	ArgsTest(1,2,3,4,5);

The extra arguments are unused

Function overloading is not supported. If you declare a function twice, the first one will be discarded:
	def SayHello()
	{
	    print("hello');
	}

	def SayHello()
	{
	    print("hijacked!");
	}

	def main()
	{
	    SayHello();
	}

In the example above, "hijacked" will be printed.

Classes
=======

class attribs and methods are all public. The static keyword is valid but will have no effect (I'll implement static vars and functions later)

	class MyClass
	{
		var myAttrib;
	
		MyClass()
		{
	
		}
	
		def SayHello()
		{
			print("hello");
		}
	}

The class's constructor is just a function declared without the def keyword and its name. The constructor acts like any other function and can even be called outside of the class's definition.

Attributes
==========

class attribs can only be declared. Initialization of attribs must be done in it's functions. Attributes are set to null unless a value is assigned it.
attributes can be assigned outside of the class's declaration

Class Methods
===============

class methods are declared like any other functions. You can access the calling instance using the self keyword.
	class Greet
	{
		var greeting;

		Greet(msg)
		{
			self.greeting = msg;
		}

		def Send()
		{
			print(self.greeting);
		}
	}


Inheritance
===========

inheritance is done by using the extend keyword.
	class Base
	{
		var BaseVal;

		Base()
		{
			print("base constructed");
		}
	}

	class Derived extends Base
	{
		Derived()
		{
			self.Base();

			print("derived constructed");
		}
	}

During instantiation, all of the base class's attribs and methods are copied into the instance before the derived class'. The base class's attribs and methods will be replaced by the derived's if they( the attribs and methods) share the same name.

Statements
==========

This version only supports if, while and return statements. They function the same way as they would in Javascript.

	if(5<10)
	{
		print("logic is intact")
	}else{
		print("something is terribly wrong :( ");
	}

	int counter = 0;
	while(couter<10)
	{
		print(i);
	}

	return false;

Variables
=========

DragonScript's types are: number, string, bool and Object.
number, string and bool are passed by reference. Objects are created from classes and are passed by value.

Math operations can only be done on numbers. You can, however, concatenate strings( AND ONLY STRINGS) using the + operator.

Variable Comparisons
====================

only bools and numbers can be compared
bools can only be compared using the == and != operators
numbers can be compared using the < > <= >= != and == operators

Arithmetic Operations
=====================
the only assignment operation is the = operator
addition, subtraction, division and multiplication

Built-ins
=========
Dragonscript currently has no built-in functions

Keywords
========
class def var for else extends if and or static


there's no mechanism to include other stripts. DragonScript was made with the intention of having all scripts being compiled once.

