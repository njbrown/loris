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
#include <vector>
#include <string>

#include "parser.hpp"
#include "virtualmachine.hpp"
#include "assembly.hpp"

using namespace std;

namespace loris
{

struct SourceCode
{
	string filename;
	string source;
	Program* program;
};

class Compiler
{
	Parser parser;
	vector<SourceCode> sources;
	Error error;
	Assembly* assembly;
	bool debug;//debug mode

	//helpers

	//break stack
	//stack<int> breakStack;
public:
	Compiler()
	{
		debug = false;
	}

	Assembly* GetAssembly();

	void AddSource(string filename,string code);

	bool Compile(bool debug = false);
	bool Compile(Assembly* assembly, bool debug = false);

	//compiles function node into instructions
	Function* CompileFunction(FunctionDefinition* funcDef);

	void PushLineOp(Function* func,int line);

	void CompileBlock(Function* func,Block* block);

	void CompileStatement(Function* func,Statement* stmt);

	void CompileExpressionStatement(Function* func,ExpressionStatement* stmt);

	void CompileExpression(Function* func,Expression* expr);

	void CompileWhileStatement(Function* func,WhileStatement* stmt);

	void CompileIfStatement(Function* func,IfStatement* stmt);

	Error GetError();
};

}