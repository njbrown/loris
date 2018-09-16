/*
Copyright (c) 2014, Nicolas Brown
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of dragonscript nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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