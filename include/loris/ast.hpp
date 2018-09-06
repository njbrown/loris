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
#include <string>
#include <vector>

using namespace std;
#include "lexer.hpp"

namespace dragonscript
{

class ASTNode
{
public:
	virtual ~ASTNode()
	{
	}

	enum Type
	{
		//expressions
		Expr,
		ExprStmt,
		BinaryExpr,
		//literals
		NumberLiteral,
		StringLiteral,
		BoolLiteral,
		NullLiteral,
		//itentifiers and expressions
		Iden,
		PropAccess,
		FunctionCall,
		New,
		Var,
		//statements
		IfStmt,
		ReturnStmt,
		WhileStmt,
		///definitions
		Enum,
		FunctionDef,
		ClassDef,

		//other
		BlockStmt,
		Neg
	};

	Type type;
	int line;
};

class Statement:public ASTNode{
};

class Block:public Statement
{
public:
	vector<Statement*> statements;

	Block()
	{
		type = Type::BlockStmt;
	}

	void AddStatement(Statement *stmt)
	{
		statements.push_back(stmt);
	}
};

class ClassDefinition;
class FunctionDefinition;

class Program:public ASTNode
{
public:
	vector<ClassDefinition*> classes;
	vector<FunctionDefinition*> functions;

	void AddClass(ClassDefinition* def)
	{
		classes.push_back(def);
	}

	void AddFunction(FunctionDefinition* func)
	{
		functions.push_back(func);
	}

};

class ImportStatement:public Statement
{
public:
	vector<string> path;

	void AddToPath(string part)
	{
		path.push_back(part);
	}
};

class ClassAttribDefinition:public ASTNode
{
public:
	string name;
	string type;
	bool isStatic;
	FunctionDefinition* init;//initialization expression enclosed in a function

	ClassAttribDefinition()
	{
		init = nullptr;
	}

	void SetStatic(bool stat)
	{
		isStatic = stat;
	}

	void SetName(string name)
	{
		this->name = name;
	}

	void SetType(string type)
	{
		this->type = type;
	}
};

class FunctionDefinition;

class ClassDefinition:public ASTNode
{
public:
	string name;
	string superClass;
	vector<ClassAttribDefinition*> attribs;
	vector<FunctionDefinition*> functions;

	void SetName(string name)
	{
		this->name = name;
	}

	void SetSuperClass(string name)
	{
		this->superClass = name;
	}
 
	void AddAttrib(ClassAttribDefinition* attrib)
	{
		attribs.push_back(attrib);
	}

	void AddFunction(FunctionDefinition* func)
	{
		functions.push_back(func);
	}
};

class FunctionParameter:public ASTNode
{
public:
	string name;
	string type;

	void SetName(string name)
	{
		this->name = name;
	}

	void SetType(string type)
	{
		this->type = type;
	}
};

class FunctionDefinition:public Statement
{
public:
	vector<FunctionParameter*> params;
	vector<Statement*> statements;
	//Block *block;
	string name;
	string returnType;
	bool isStatic;
	bool isConstructor;

	FunctionDefinition()
	{
		type = ASTNode::FunctionDef;
	}

	void SetName(string name)
	{
		this->name = name;
	}

	void SetStatic(bool stat)
	{
		isStatic = stat;
	}

	void SetReturnType(string type)
	{
		this->returnType = type;
	}

	void AddParam(FunctionParameter* param)
	{
		params.push_back(param);
	}

	void AddStatement(Statement *stmt)
	{
		//block->AddStatement(stmt);
		statements.push_back(stmt);
	}
};

class Expression;

class ExpressionStatement:public Statement
{
public:
	Expression *expr;

	ExpressionStatement(Expression *e)
	{
		expr = e;

		type = ASTNode::ExprStmt;
	}
};

class Expression:public ASTNode
{
public:

};

class Identifier:public Expression
{
public:
	string name;

	Identifier(string name)
	{
		this->name = name;
		type = ASTNode::Iden;
	}
};

class BinaryExpression:public Expression
{
public:
	//left and right children nodes
	Expression *left;
	Expression *right;
	Token::Type op;

	BinaryExpression(Token::Type op,Expression *left,Expression *right)
	{
		this->left = left;
		this->right = right;
		this->op = op;

		type = ASTNode::BinaryExpr;
	}
};

class NumberLiteral:public Expression
{
public:
	float value;

	NumberLiteral(float val)
	{
		value = val;
		type = ASTNode::NumberLiteral;
	}

	NumberLiteral(string val)
	{
		value = (float)atof(val.c_str());
		type = ASTNode::NumberLiteral;
	}
};

class StringLiteral:public Expression
{
public:
	std::string value;

	StringLiteral(std::string val)
	{
		value = val;
		type = ASTNode::StringLiteral;
	}
};

class BoolLiteral:public Expression
{
public:
	bool value;

	BoolLiteral(bool val)
	{
		value = val;
		type = ASTNode::BoolLiteral;
	}
};

class NullLiteral:public Expression
{
public:

	NullLiteral()
	{
		type = ASTNode::NullLiteral;
	}
};

class NegExpr:public Expression
{
public:
	Expression* child;
	NegExpr()
	{
		type = ASTNode::Neg;
	}
};

class PropertyAccess:public Expression
{
public:
	Expression *obj;
	string name;

	PropertyAccess(Expression *lhs,string rhs)
	{
		obj = lhs;
		name = rhs;

		type = ASTNode::PropAccess;
	}
};

class Arguments;

class CallExpr:public Expression
{
public:
	Expression *obj;
	Arguments* args;

	CallExpr(Expression* lhs,Arguments* rhs)
	{
		obj = lhs;
		args = rhs;

		type = ASTNode::FunctionCall;
	}
};

class NewExpr:public Expression
{
public:
	string name;
	Arguments* args;

	NewExpr(string n,Arguments* rhs)
	{
		name = n;
		args = rhs;

		type = ASTNode::New;
	}
};

class VarExpr:public Expression
{
public:
	string name;
	string typeName;

	VarExpr(string n,string t)
	{
		name = n;
		typeName = t;

		type = ASTNode::Var;
	}
};

class Arguments:public Expression
{
public:
	vector<Expression*> args;

	void AddArg(Expression* expr)
	{
		args.push_back(expr);
	}
};

class IfStatement:public Statement
{
public:
	Expression *expr;
	Block *block;
	Statement* elseStmt;

	IfStatement(Expression *e,Block *b)
	{
		expr = e;
		block = b;

		elseStmt = NULL;
		type = ASTNode::IfStmt;
	}
};

class WhileStatement:public Statement
{
public:
	Expression* expr;
	Block* block;

	WhileStatement(Expression* e,Block* b)
	{
		type = ASTNode::WhileStmt;
		expr = e;
		block = b;
	}
};

class ReturnStatement:public Statement
{
public:
	Expression* expr;

	ReturnStatement()
	{
		type = ASTNode::ReturnStmt;
	}
};

class EnumStatement:public Statement
{
public:
	string name;
	vector<string> values;

	EnumStatement(string n)
	{
		name = n;
		type = ASTNode::Enum;
	}

	void AddValue(string val)
	{
		values.push_back(val);
	}
};


class EmptyStatement:public Statement
{

};

}