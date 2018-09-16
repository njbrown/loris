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

#include "lexer.hpp"
#include "ast.hpp"

namespace loris
{

//stolen from v8's source :p
#define CHECK_OK  ok);\
  if (!*ok) return nullptr;((void)0

class Statement;

struct Op
{
	enum Assoc
	{
		LeftAssoc,
		RightAssoc
	};

	Assoc assoc;
	Token::Type type;
	int prec;//precedence
};

class Parser
{
	/*
	holds node for cleaning up if parsing fails somewhere
	*/
	vector<ASTNode*> nodes;

	Lexer* lex;
	TokenStream* tokens;
	Program* program;
	
	Error error;
public:
	Expression* expr;//just for testing expression parsing

	Parser();
	Program *GetProgram();
	
	bool Parse(string code);

	Program* ParseProgram(bool *ok);

	//class attrib:
	//	var iden (':' iden)? ';'
	ClassAttribDefinition* ParseClassAttribDefinition(bool* ok);


	//class definition
	//	'class' iden ('extends' iden)? '{' (ClassAttribDefinition|functiondefinition)* '}'
	ClassDefinition* ParseClassDefinition(bool* ok);

	bool ParseExpr(string code);

	Expression* ParseExpr(bool *ok);

	Expression* ParseAssignExpr(bool* ok);

	Expression* ParseVarExpr(bool* ok);

	bool IsAssignOp(Token::Type op);

	//http://eli.thegreenplace.net/2012/08/02/parsing-expressions-by-precedence-climbing/
	Expression* ParseBinaryExpr(int min_prec,bool *ok);

	Op ParseOp(bool *ok);

	//only numbers for now
	Expression* ParsePrimary(bool *ok);

	Expression* ParseLiteral(bool* ok);

	Expression* ParseNegExpr(bool* ok);

	//'new' iden '(' args ')'
	Expression* ParseNewExpr(bool *ok);

	/*
	iden expr_suffix?
	*/
	Expression *ParseMemberExpr(bool *ok);

	/*
	( '.' iden | '[' expr ']'| '('args')' )*

	these follow identifiers,list_literals and expressions between parentheses
	*/
	Expression *ParseMemberExprSuffix(Expression *expr,bool *ok);

	Arguments* ParseArgs(bool *ok);

	/*
	syntax:
	import iden(dot iden)* (asterisk)? semicolon
	eg
	import module;
	import module.SubModule;
	import module.Submodule.*;
	*/
	ImportStatement *ParseImportStatement(bool *ok);
	
	//param:
	//	iden (colon iden)?
	FunctionParameter* ParseParam(bool *ok);

	//return type of void* because of CHECK_OK
	void* ParseParamList(FunctionDefinition* func,bool *ok);

	//'def' iden '(' (functionparam)* ')' (':' iden)? '{' block '}'
	FunctionDefinition *ParseFunctionDefinition(bool *ok);

	//('def')? iden '(' (functionparam)* ')' (':' iden)? '{' block '}'
	FunctionDefinition *ParseFunctionDefinition(bool isConstructor,bool *ok);

	/*
	VarStatement* ParseVarStatement(bool *ok)
	{
		return nullptr;
	}
	*/
	Statement *ParseStatement(bool *ok);

	IfStatement* ParseIfStatement(bool *ok);

	WhileStatement* ParseWhileStatement(bool *ok);

	/*
	'enum' EnumName '{' (EnumValue ('=' literal )? )*  '}'
	}
	*/
	EnumStatement* ParseEnumStatement(bool *ok);

	ReturnStatement* ParseReturnStatement(bool* ok);

	Block* ParseBlock(bool *ok);

	ExpressionStatement* ParseExprStatement(bool *ok);

	void ReportUnexpectTokenError(Token token);

	Error GetError();

private:
	/*
	Expects a token
	*/
	void Expect(Token::Type type,bool *ok);

	/*
	checks for an expected token and returns it
	if the next token is the not the one expected,
	ok is set to false and an error is reported
	*/
	void Consume(Token::Type type,bool *ok);

	template<class T>
	T* AddNode(T* node);

	void Cleanup();
};

}