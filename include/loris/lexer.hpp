
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
#include "error.hpp"

using namespace std;

namespace dragonscript
{

struct Token
{
	enum Type
	{
		Dot,
		//keywords
		Import,
		Iden,
		Function,
		Def,
		Module,
		New,
		Class,
		Extends,
		Implements,
		Static,
		Enum,
		Var,
		If,
		Else,
		Elif,
		For,
		While,
		Break,
		Return,
		OpenBracket,
		CloseBracket,
		OpenParen,
		CloseParen,
		OpenCurlyBrace,
		CloseCurlyBrace,
		Colon,
		SemiColon,
		Comma,
		Native,
		/* literals */
		String,
		Integer,
		Float,
		Null,
		False,
		True,
		/* logical expressions and arithmetic*/
		Or,
		And,
		Add,
		Sub,
		Mul,
		Div,
		AddAssign,
		SubAssign,
		MulAssign,
		DivAssign,
		Not,
		EQ,
		NEQ,
		Assign,
		LT,
		GT,
		LTE,
		GTE,
		EOS //End of Stream
	};

	Type type;
	string token;
	int line;

	static string GetTokenName(Token::Type type)
	{
		#define TOK_NAME_ASSOC(x) if(type==(x)) return #x;
		#define TOKEN_MAP_NAME(x,y) if(type==(x)) return y;

		//TOK_NAME_ASSOC(Token::);

		//TOKEN_MAP_NAME(Token::,"");
		TOKEN_MAP_NAME(Token::Import,"import");
		TOKEN_MAP_NAME(Token::Iden,"identifier");
		TOKEN_MAP_NAME(Token::Module,"module");
		TOKEN_MAP_NAME(Token::Dot,".");
		TOKEN_MAP_NAME(Token::SemiColon,";");
		TOKEN_MAP_NAME(Token::EOS,"End of File");
		TOKEN_MAP_NAME(Token::Function,"function");
		TOKEN_MAP_NAME(Token::Comma,",");
		TOKEN_MAP_NAME(Token::If,"if");
		TOKEN_MAP_NAME(Token::For,"for");
		TOKEN_MAP_NAME(Token::While,"while");
		TOKEN_MAP_NAME(Token::OpenParen,"(");
		TOKEN_MAP_NAME(Token::CloseParen,")");
		TOKEN_MAP_NAME(Token::OpenBracket,"[");
		TOKEN_MAP_NAME(Token::CloseBracket,"]");
		TOKEN_MAP_NAME(Token::OpenCurlyBrace,"{");
		TOKEN_MAP_NAME(Token::CloseCurlyBrace,"}");
		TOKEN_MAP_NAME(Token::Assign,"=");

		TOKEN_MAP_NAME(Token::EOS,"End of Stream");
		
		/*
		TOK_NAME_ASSOC(Token::Iden);
		TOK_NAME_ASSOC(Token::Import);
		TOK_NAME_ASSOC(Token::Module);
		TOK_NAME_ASSOC(Token::Dot);
		TOK_NAME_ASSOC(Token::SemiColon);
		TOK_NAME_ASSOC(Token::EOS);
		*/

		return "";
	}

	static Token EOSToken()
	{
		Token token;
		token.line=-1;
		token.type = Token::EOS;

		return token;
	}

	static Token EOSToken(int line)
	{
		Token token;
		token.line=line;
		token.type = Token::EOS;

		return token;
	}
};

class TokenStream
{
public:
	vector<Token> tokens;
	unsigned int index;

	TokenStream();
	void AddToken(Token t);
	void AddToken(Token::Type type,string tok,int line);

	Token NextToken();
	Token::Type PeekTokenType(unsigned int look_ahead=0);
	Token PeekToken(unsigned int look_ahead=0);
	void Advance();
	
	bool HasMore();
};

class CharStream
{
public:
	string chars;
	unsigned int index;

	CharStream(string code);
	int NextChar();
	void Advance();
	int PeekChar(unsigned int look_ahead=0);
	bool HasMore();
};

class Lexer
{
public:
	Error error;
	TokenStream *tokens;
	CharStream *stream;
	int line;
	bool Parse(string code);

	Lexer();
	~Lexer();

private:

	bool IsWhiteSpace();

	void ReadWhiteSpace();

	bool IsIdentifier();
	void ReadIdentifier();

	bool IsNumber();
	void ReadNumber();

	void ReadSingleQuotedString();
	void ReadDoubleQuotedString();

	string StripComments(string);

	void AddToken(Token::Type type,bool advance=true);
};

}
