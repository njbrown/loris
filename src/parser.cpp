#include "../include/dragonscript/parser.hpp"

using namespace dragonscript;

Program* Parser::GetProgram()
{
	return program;
}

Parser::Parser()
{
	lex=NULL;
	tokens=NULL;
	program=NULL;
}
bool Parser::Parse(string code)
{
	if(lex)delete lex;
	lex = new Lexer;
		
	if(!lex->Parse(code))
		return false;

	tokens = lex->tokens;

	//begin parsing
	bool ok = true;
	program = ParseProgram(&ok);

	return ok;
}

Program* Parser::ParseProgram(bool *ok)
{
	Program* program=AddNode(new Program());

	Statement* stmt=nullptr;
	ClassDefinition* classDef=nullptr;
	FunctionDefinition* func=nullptr;
	//EnumStatement* enumStmt;
	while(tokens->HasMore())
	{
		Token tok = tokens->PeekToken();
		switch(tok.type)
		{
		case Token::Import:
			stmt = ParseImportStatement(CHECK_OK);
			stmt->line = tok.line;
			break;
		case Token::Class:
			classDef = ParseClassDefinition(CHECK_OK);
			classDef->line = tok.line;
			program->AddClass(classDef);
			break;
		case Token::Def:
			func = ParseFunctionDefinition(CHECK_OK);
			func->line = tok.line;
			program->AddFunction(func);
			break;
		case Token::SemiColon:
			//empty statement
			//advance and move along
			tokens->Advance();
			break;
		/*
		case Token::Enum:
				
			break;
		*/
		default:
			//error
			ReportUnexpectTokenError(tokens->NextToken());
			*ok=false;
			return NULL;
		}
	}

	return program;
}

//class attrib:
//	var iden (':' iden)? ';'
ClassAttribDefinition* Parser::ParseClassAttribDefinition(bool* ok)
{
	ClassAttribDefinition* attrib = AddNode(new ClassAttribDefinition());

	Consume(Token::Var,CHECK_OK);

	//name
	Expect(Token::Iden,CHECK_OK);//iden
	attrib->SetName(tokens->NextToken().token);

	//type is optional
	if(tokens->PeekTokenType()== Token::Colon)
	{
		Consume(Token::Colon,CHECK_OK);//':'

		//type
		Expect(Token::Iden,CHECK_OK);//iden
		attrib->SetType(tokens->NextToken().token);
			
	}

	//check for assignment
	if(tokens->PeekTokenType()== Token::Assign)
	{
		Consume(Token::Assign,CHECK_OK);//'='
		FunctionDefinition* func = AddNode(new FunctionDefinition());
		Expression* expr = ParseExpr(CHECK_OK);
		ExpressionStatement* exprStmt = AddNode(new ExpressionStatement(expr));
		func->AddStatement(exprStmt);

		attrib->init = func;
	}

	//;
	Consume(Token::SemiColon,CHECK_OK);

	return attrib;
}


//class definition
//	'class' iden ('extends' iden)? '{' (ClassAttribDefinition|functiondefinition)* '}'
ClassDefinition* Parser::ParseClassDefinition(bool* ok)
{
	ClassDefinition* classDef = AddNode(new ClassDefinition());

	Consume(Token::Class,CHECK_OK);// class
	Expect(Token::Iden,CHECK_OK);// class name
	classDef->SetName(tokens->NextToken().token);

	//(extends inden)?
	if(tokens->PeekTokenType() == Token::Extends)
	{
		Consume(Token::Extends,CHECK_OK);

		Expect(Token::Iden,CHECK_OK);
		classDef->SetSuperClass(tokens->NextToken().token);
	}

	Consume(Token::OpenCurlyBrace,CHECK_OK);//{

	//(ClassAttribDefinition|functiondefinition)*
	FunctionDefinition* func = nullptr;
	bool nextMemberIsStatic = false;
	while(tokens->HasMore() && tokens->PeekTokenType() != Token::CloseCurlyBrace)
	{
		if(tokens->PeekTokenType()==Token::Static)
		{
			nextMemberIsStatic = true;
			tokens->Advance();
		}
		else
			nextMemberIsStatic = false;

		switch(tokens->PeekTokenType())
		{
		case Token::Iden://constructor
			func = ParseFunctionDefinition(true,CHECK_OK);
			//func->SetStatic(nextMemberIsStatic);
			classDef->AddFunction(func);
			break;
		case Token::Def:
			func = ParseFunctionDefinition(CHECK_OK);
			func->SetStatic(nextMemberIsStatic);
			classDef->AddFunction(func);
			break;
		case Token::Var:
			ClassAttribDefinition* attrib=ParseClassAttribDefinition(CHECK_OK);
			attrib->SetStatic(nextMemberIsStatic);
			classDef->AddAttrib(attrib);
			break;
		}
	}

	Consume(Token::CloseCurlyBrace,CHECK_OK);//}

	return classDef;
}

bool Parser::ParseExpr(string code)
{
	if(lex)delete lex;
	lex = new Lexer;
		
	if(!lex->Parse(code))
		return false;

	tokens = lex->tokens;

	//begin parsing
	bool ok = true;

	expr = ParseAssignExpr(&ok);

	return ok;
}

Expression* Parser::ParseExpr(bool *ok)
{
	return ParseAssignExpr(CHECK_OK);
}

Expression* Parser::ParseAssignExpr(bool* ok)
{
	Expression *expr = ParseBinaryExpr(4,CHECK_OK);
	if(IsAssignOp(tokens->PeekTokenType()))
	{
		Token::Type op = tokens->NextToken().type;

		Expression *rhs = ParseBinaryExpr(4,CHECK_OK);
		expr = new BinaryExpression(op,expr,rhs);
	}

	//Consume(Token::SemiColon,CHECK_OK);

	return expr;
}

Expression* Parser::ParseVarExpr(bool* ok)
{
	Consume(Token::Var,CHECK_OK);//var

	Expect(Token::Iden,CHECK_OK);//iden
	string name = tokens->NextToken().token;

	if(tokens->PeekTokenType()==Token::Colon)
	{
		Consume(Token::Colon,CHECK_OK);//':'

		Expect(Token::Iden,CHECK_OK);//iden
		string type = tokens->NextToken().token;

		return new VarExpr(name,type);
	}

	return new VarExpr(name,"any");//type doesnt matter at the moment
}

bool Parser::IsAssignOp(Token::Type op)
{
	switch(op)
	{
	case Token::Assign:
	/*
	//the following are a part of the milestone2 spec
	case Token::AddAssign:
	case Token::SubAssign:
	case Token::MulAssign:
	case Token::DivAssign:
	*/
		return true;
	default:
		return false;
	}
}

//http://eli.thegreenplace.net/2012/08/02/parsing-expressions-by-precedence-climbing/
Expression* Parser::ParseBinaryExpr(int min_prec,bool *ok)
{
	Expression *lhs = ParsePrimary(CHECK_OK);
	Op op;

	while(true)
	{
		op = ParseOp(CHECK_OK);

		if(op.prec < min_prec)
			break;
			
		//if all goes well, it is safe to advance
		tokens->Advance();

		int next_min_prec;
		if(op.assoc ==  Op::LeftAssoc)
			next_min_prec = op.prec+1;
		else
			next_min_prec = op.prec;

		//recurse to get right hand side of tree
		Expression *rhs = ParseBinaryExpr(next_min_prec,CHECK_OK);

		//create binary expression and add left and right hand side
		BinaryExpression *bin = AddNode(new BinaryExpression(op.type,lhs,rhs));

		//bin->left = lhs;
		//bin->right = rhs;
		//bin->op = op.type;
		lhs = bin;
	}

	return lhs;
}

Op Parser::ParseOp(bool *ok)
{
	//assume next token is op, for now
	Op op;
	op.type = tokens->PeekTokenType();

	switch(op.type)
	{
	case Token::Assign:
		op.prec = 2;
		op.assoc = Op::LeftAssoc;
		break;
	case Token::Or:
		op.prec = 4;
		op.assoc = Op::LeftAssoc;
		break;
	case Token::And:
		op.prec = 5;
		op.assoc = Op::LeftAssoc;
		break;
	case Token::EQ:
	case Token::NEQ:
		op.prec = 6;
		op.assoc = Op::LeftAssoc;
		break;
	case Token::LT:
	case Token::GT:
	case Token::GTE:
	case Token::LTE:
		op.prec = 7;
		op.assoc = Op::LeftAssoc;
		break;
	case Token::Add:
	case Token::Sub:
		op.prec = 8;
		op.assoc = Op::LeftAssoc;
		break;
	case Token::Mul:
	case Token::Div:
		op.prec = 9;
		op.assoc = Op::LeftAssoc;
		break;
	default:
		//not a valid op
		//assume we've finished parsing
		op.prec = 0;
		break;
	}

	return op;
}

//only numbers for now
Expression* Parser::ParsePrimary(bool *ok)
{
	Token token = tokens->PeekToken();

	Expression *atom = NULL;

	switch(token.type)
	{
	case Token::String:
		atom = AddNode(new StringLiteral(token.token));
		tokens->Advance();
		break;
	case Token::Float:
	case Token::Integer:
		atom = AddNode(new NumberLiteral(token.token));
		tokens->Advance();
		break;
	//negative
	case Token::Sub:
		atom = ParseNegExpr(CHECK_OK);
		break;
	case Token::Null:
		atom = AddNode(new NullLiteral());
		tokens->Advance();
		break;
	case Token::True:
		atom = AddNode(new BoolLiteral(true));
		tokens->Advance();
		break;
	case Token::False:
		atom = AddNode(new BoolLiteral(false));
		tokens->Advance();
		break;
	case Token::Iden:
		atom = ParseMemberExpr(CHECK_OK);
		//tokens->Advance();
		break;
	//why is this here??
	case Token::Var:
		atom = ParseVarExpr(CHECK_OK);
		break;
	case Token::New:
		atom = ParseNewExpr(CHECK_OK);
		break;
	case Token::OpenParen:
		tokens->Advance();//consume (
		atom = ParseExpr(CHECK_OK);
		Consume(Token::CloseParen,CHECK_OK);//consume )
		break;
	default:
		*ok=false;
		ReportUnexpectTokenError(token);
		break;
	}

	return atom;
}

Expression* Parser::ParseLiteral(bool* ok)
{
	Token token = tokens->PeekToken();

	Expression *atom = NULL;

	switch(token.type)
	{
	case Token::String:
		atom = AddNode(new StringLiteral(token.token));
		tokens->Advance();
		break;
	case Token::Float:
	case Token::Integer:
		atom = AddNode(new NumberLiteral(token.token));
		tokens->Advance();
		break;
	//negative
	case Token::Sub:
		atom = ParseNegExpr(CHECK_OK);
		break;
	case Token::Null:
		atom = AddNode(new NullLiteral());
		tokens->Advance();
		break;
	case Token::True:
		atom = AddNode(new BoolLiteral(true));
		tokens->Advance();
		break;
	case Token::False:
		atom = AddNode(new BoolLiteral(false));
		tokens->Advance();
		break;
	default:
		*ok=false;
		ReportUnexpectTokenError(token);
		break;
	}

	return atom;
}

Expression* Parser::ParseNegExpr(bool* ok)
{
	Consume(Token::Sub,CHECK_OK);
	Token token = tokens->PeekToken();

	NegExpr *neg = AddNode(new NegExpr());

	//parse expression instead, let the vm throw the error
	neg->child = ParseExpr(CHECK_OK);

	/*
	switch(token.type)
	{
	case Token::Float:
	case Token::Integer:
		neg->child = AddNode(new NumberLiteral(token.token));
		tokens->Advance();
		break;
	case Token::Iden:
		neg->child = ParseMemberExpr(CHECK_OK);
		break;
	default:
		*ok=false;
		ReportUnexpectTokenError(token);
		return NULL;
		break;
	}
	*/

	return neg;
}

//'new' iden '(' args ')'
Expression* Parser::ParseNewExpr(bool *ok)
{
	tokens->Advance();// 'new'

	Expect(Token::Iden,CHECK_OK);
	string name = tokens->NextToken().token;

	tokens->Advance();//' (

	Arguments* args = ParseArgs(CHECK_OK);

	Expression* expr = AddNode(new NewExpr(name,args));

	Consume(Token::CloseParen,CHECK_OK);// ')'

	//suffix
	return ParseMemberExprSuffix(expr,CHECK_OK);
}

/*
iden expr_suffix?
*/
Expression* Parser::ParseMemberExpr(bool *ok)
{
	Expect(Token::Iden,CHECK_OK);
	Expression* expr = AddNode(new Identifier(tokens->NextToken().token));

	return ParseMemberExprSuffix(expr,CHECK_OK);
}

/*
( '.' iden | '[' expr ']'| '('args')' )*

these follow identifiers,list_literals and expressions between parentheses
*/
Expression* Parser::ParseMemberExprSuffix(Expression *expr,bool *ok)
{
	//Expression *e;
	Identifier* iden;
	Arguments* args;

	while(true)
	{
		switch(tokens->PeekTokenType())
		{
		case Token::OpenBracket:
			//todo: chnage to indexaccess
			/*
			tokens->Advance();// [

			e = ParseExpr(CHECK_OK);
			expr = AddNode(new PropertyAccess(expr,e));

			Consume(Token::CloseBracket,CHECK_OK);// ]
			*/
			break;
		case Token::Dot:
			tokens->Advance();// .
			Expect(Token::Iden,CHECK_OK);//next item must be an identifier

			//make into an identifier
			iden = AddNode(new Identifier(tokens->NextToken().token));
			expr = AddNode(new PropertyAccess(expr,iden->name));
			break;
		case Token::OpenParen:
			tokens->Advance();//' (

			args = ParseArgs(CHECK_OK);

			expr = AddNode(new CallExpr(expr,args));

			Consume(Token::CloseParen,CHECK_OK);// ')'
			break;
		default:
			return expr;
		}
	}
}

Arguments* Parser::ParseArgs(bool *ok)
{
	Arguments* args = AddNode(new Arguments());

	if(tokens->PeekTokenType()!=Token::CloseParen)
	{
		Expression* expr = ParseExpr(CHECK_OK);
		args->AddArg(expr);

		while(tokens->PeekTokenType()==Token::Comma)
		{
			//consume comma
			Consume(Token::Comma,CHECK_OK);//safer
			//tokens->Advance();//cheaper

			expr = ParseExpr(CHECK_OK);
			args->AddArg(expr);
		}
	}

	return args;
}
	
	

/*
syntax:
import iden(dot iden)* (asterisk)? semicolon
eg
import module;
import module.SubModule;
import module.Submodule.*;
*/
ImportStatement* Parser::ParseImportStatement(bool *ok)
{
	ImportStatement *stmt = AddNode(new ImportStatement());

	Consume(Token::Import,CHECK_OK);

	Expect(Token::Iden,CHECK_OK);
	Token name = tokens->NextToken();
	stmt->AddToPath(name.token);

	while(tokens->PeekTokenType()==Token::Dot)
	{
		tokens->Advance();// .

		//only one asterisk allowed
		if(tokens->PeekTokenType()==Token::Mul)// import module.*;
		{
			tokens->Advance();
			stmt->AddToPath("*");
			break;
		}
		else
		{
			Expect(Token::Iden,CHECK_OK);
			name = tokens->NextToken();
			stmt->AddToPath(name.token);
		}
			
	}

	Consume(Token::SemiColon,CHECK_OK);

	return stmt;
}
	
//param:
//	iden (colon iden)?
FunctionParameter* Parser::ParseParam(bool *ok)
{
	FunctionParameter* param = AddNode(new FunctionParameter());

	//iden
	Expect(Token::Iden,CHECK_OK);
	param->name = tokens->NextToken().token;

	//type is optional at the moment
	if(tokens->PeekTokenType()==Token::Colon)
	{
		//colon
		Consume(Token::Colon,CHECK_OK);

		//iden
		Expect(Token::Iden,CHECK_OK);
		param->type = tokens->NextToken().token;
	}
	else
	{
		param->type = "any";
	}

	return param;
}

//return type of void* because of CHECK_OK
void* Parser::ParseParamList(FunctionDefinition* func,bool *ok)
{
	//parameter list
	//	param?
	//| param ( comma param )*
	if(tokens->PeekTokenType()==Token::Iden)
	{
		FunctionParameter* param = ParseParam(CHECK_OK);
		func->AddParam(param);

		while(tokens->PeekTokenType()==Token::Comma)
		{
			//consume comma
			Consume(Token::Comma,CHECK_OK);//more secure
			//tokens->Advance();//cheaper
				
			FunctionParameter* param = ParseParam(CHECK_OK);
			func->AddParam(param);
		}
	}

	return NULL;
}

//'def' iden '(' (functionparam)* ')' (':' iden)? '{' block '}'
FunctionDefinition* Parser::ParseFunctionDefinition(bool *ok)
{
	return ParseFunctionDefinition(false,CHECK_OK);
}
//('def')? iden '(' (functionparam)* ')' (':' iden)? '{' block '}'
FunctionDefinition* Parser::ParseFunctionDefinition(bool isConstructor,bool *ok)
{
	FunctionDefinition *func = AddNode(new FunctionDefinition());

	if(!isConstructor)//constructors have no 'def'
		Consume(Token::Def,CHECK_OK); //def

	//function name
	Expect(Token::Iden,CHECK_OK);
	func->SetName(tokens->NextToken().token);

	Consume(Token::OpenParen,CHECK_OK);// (

	ParseParamList(func,CHECK_OK);//params

	Consume(Token::CloseParen,CHECK_OK);// )

	//return type
	if(tokens->PeekTokenType()==Token::Colon)
	{
		Consume(Token::Colon,CHECK_OK);//':'
		Expect(Token::Iden,CHECK_OK);
		func->SetReturnType(tokens->NextToken().token);
	}
	else
	{
		func->SetReturnType("void");
	}

	Consume(Token::OpenCurlyBrace,CHECK_OK);// {

	//tofix: functions can only contain expression,for,while, and if statements
	Token::Type peek = tokens->PeekTokenType();
	while(peek != Token::CloseCurlyBrace && peek != Token::EOS)
	{
		Statement *stmt = ParseStatement(CHECK_OK);
		func->AddStatement(stmt);

		peek = tokens->PeekTokenType();
	}

	Consume(Token::CloseCurlyBrace,CHECK_OK);// }

	return func;
}
/*
VarStatement* ParseVarStatement(bool *ok)
{
	return nullptr;
}
*/
Statement* Parser::ParseStatement(bool *ok)
{
	Statement *stmt;
	Token tok = tokens->PeekToken(); 
	switch(tok.type)
	{
	case Token::OpenCurlyBrace:
		stmt = ParseBlock(CHECK_OK);
		stmt->line = tok.line;
		break;
	case Token::If:
		stmt = ParseIfStatement(CHECK_OK);
		stmt->line = tok.line;
		break;
	case Token::While:
		stmt = ParseWhileStatement(CHECK_OK);
		stmt->line = tok.line;
		break;
	case Token::Return:
		stmt = ParseReturnStatement(CHECK_OK);
		stmt->line = tok.line;
		break;
	//case Token::Var:
	//	stmt = ParseVarStatement(CHECK_OK);
	//	break;
	//case Token::Enum:
	//	stmt = ParseEnumStatement(CHECK_OK);
	//	break;
	default:
		stmt = ParseExprStatement(CHECK_OK);
		stmt->line = tok.line;
		break;
	}

	return stmt;
}

IfStatement* Parser::ParseIfStatement(bool *ok)
{
	Consume(Token::If,CHECK_OK);// if
	Consume(Token::OpenParen,CHECK_OK);// (

	Expression* expr = ParseExpr(CHECK_OK);// expr

	Consume(Token::CloseParen,CHECK_OK);// )

	Block* block = ParseBlock(CHECK_OK);//{ }

	IfStatement *stmt = AddNode(new IfStatement(expr,block));
		
	//check for an else
	if(tokens->PeekTokenType()==Token::Else)
	{
		tokens->NextToken();
		stmt->elseStmt = ParseStatement(CHECK_OK);
	}
		

	//IfStatement* stmt = AddNode(new IfStatement());
	return stmt;
}

WhileStatement* Parser::ParseWhileStatement(bool *ok)
{
	Consume(Token::While,CHECK_OK);// if
	Consume(Token::OpenParen,CHECK_OK);// (

	Expression* expr = ParseExpr(CHECK_OK);// expr

	Consume(Token::CloseParen,CHECK_OK);// )

	Block* block = ParseBlock(CHECK_OK);//{ }

	//IfStatement* stmt = AddNode(new IfStatement());
	return AddNode(new WhileStatement(expr,block));
}
/*
'enum' EnumName '{' (EnumValue ('=' literal )? )*  '}'
}
*/
EnumStatement* Parser::ParseEnumStatement(bool *ok)
{
	Consume(Token::Enum,CHECK_OK);//enum

	Expect(Token::Iden,CHECK_OK);// iden
	string name = tokens->NextToken().token;

	EnumStatement* stmt = AddNode(new EnumStatement(name));

	Consume(Token::OpenCurlyBrace,CHECK_OK);//{

	//read list of values
	if(tokens->PeekTokenType()!=Token::CloseParen)
	{
		Expect(Token::Iden,CHECK_OK);// iden
		name = tokens->NextToken().token;
		stmt->AddValue(name);

		while(tokens->PeekTokenType()==Token::Comma)
		{
			//consume comma
			//Consume(Token::Comma,CHECK_OK);
			tokens->Advance();//cheaper

			Expect(Token::Iden,CHECK_OK);// iden
			name = tokens->NextToken().token;
			stmt->AddValue(name);
		}
	}

	Consume(Token::CloseCurlyBrace,CHECK_OK);//}

	return stmt;
}

ReturnStatement* Parser::ParseReturnStatement(bool* ok)
{
	Consume(Token::Return,CHECK_OK);
	Expression* expr = ParseExpr(CHECK_OK);
	Consume(Token::SemiColon,CHECK_OK);//y not use ParseExprStatement

	ReturnStatement* retStmt = AddNode(new ReturnStatement());
	retStmt->expr = expr;

	return retStmt;
}

Block* Parser::ParseBlock(bool *ok)
{
	Block* block = AddNode(new Block());

	if(tokens->PeekTokenType()==Token::OpenCurlyBrace)
	{
		Consume(Token::OpenCurlyBrace,CHECK_OK);

		//parse all upcoming statements
		while(tokens->PeekTokenType()!=Token::CloseCurlyBrace)
		{
			Statement *stmt = ParseStatement(CHECK_OK);
			block->AddStatement(stmt);
		}

		Consume(Token::CloseCurlyBrace,CHECK_OK);
	}
	else
	{
		//parse single statement
		Statement *stmt = ParseStatement(CHECK_OK);
		block->AddStatement(stmt);
	}

	return block;
}

ExpressionStatement* Parser::ParseExprStatement(bool *ok)
{
	Expression *expr = ParseExpr(CHECK_OK);
	Consume(Token::SemiColon,CHECK_OK);

	ExpressionStatement *stmt = AddNode(new ExpressionStatement(expr));

	return stmt;
}

void Parser::ReportUnexpectTokenError(Token token)
{
	error = Error();
	error.line = token.line;
	error.message = "Unexpected ";
	error.message+=Token::GetTokenName(token.type);
}

Error Parser::GetError()
{
	return error;
}

/*
Expects a token
*/
void Parser::Expect(Token::Type type,bool *ok)
{
	Token peek = tokens->PeekToken();//peek
	if(peek.type==type)
		*ok=true;
	else
	{
		*ok=false;


		error = Error();
		error.line = peek.line;
		error.message = "Unexpected token ";
		error.message+=Token::GetTokenName(peek.type);
		error.message+=". Expected ";
		error.message+=Token::GetTokenName(type);
	}
}

/*
checks for an expected token and returns it
if the next token is the not the one expected,
ok is set to false and an error is reported
*/
void Parser::Consume(Token::Type type,bool *ok)
{
	Token tok = tokens->NextToken();//consume

	if(tok.type==type)
	{
		*ok=true;
	}
	else
	{
		*ok=false;
		
		error = Error();
		error.line = tok.line;
		error.message = "Unexpected ";
		error.message+=Token::GetTokenName(tok.type);
		error.message+=". Expected ";
		error.message+=Token::GetTokenName(type);
	}

}

template<class T>
T* Parser::AddNode(T* node)
{
	nodes.push_back(static_cast<ASTNode*>(node));

	return node;
}