
#pragma once
#include <vector>
#include <string>

#include "parser.hpp"
#include "virtualmachine.hpp"
#include "assembly.hpp"

using namespace std;

namespace dragonscript
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

	//helpers

	//break stack
	//stack<int> breakStack;
public:
	Assembly* GetAssembly();

	void AddSource(string filename,string code);

	bool Compile();

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