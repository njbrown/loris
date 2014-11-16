/*
Copyright (c) 2013 Nicolas Brown, nclsbrwn@gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
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

	/*
	returns EOF if out of bounds
	*/
	int NextChar();

	void Advance();

	/*
	returns EOF if out of bounds
	*/
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

	void AddToken(Token::Type type,bool advance=true);
	
};

}