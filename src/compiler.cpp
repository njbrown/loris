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
#include "../include/dragonscript/compiler.hpp"

using namespace dragonscript;

Assembly* Compiler::GetAssembly()
{
	return assembly;
}

void Compiler::AddSource(string filename,string code)
{
	SourceCode src = {filename,code};
	sources.push_back(src);
}

bool Compiler::Compile(bool debug)
{
	this->debug = debug;

	assembly = new Assembly;

	for(size_t i=0;i<sources.size();i++)
	{
		SourceCode src = sources[i];
		assembly->sourceNames.push_back(src.filename);

		if(!parser.Parse(src.source))
		{
			error = parser.GetError();
			error.filename = src.filename;
			return false;
		}
		src.program = parser.GetProgram();

		/* CLASS EXTRACTION */
		//loop through each class
		Program* program = parser.GetProgram();
		for(size_t c=0;c<program->classes.size();c++)
		{
			ClassDefinition* classDef = program->classes[c];
			Class* cls = new Class;

			//set name
			cls->sourceIndex = i;
			cls->name = classDef->name;
			cls->parentName = classDef->superClass;

			//extract attributes
			for(size_t a=0;a<classDef->attribs.size();a++)
			{
				ClassAttribDefinition* attr = classDef->attribs[a];
				ClassAttrib classAttr = {attr->name,attr->isStatic};
				if(attr->isStatic && attr->init)
				{
					//add init func if static
					classAttr.init = CompileFunction(attr->init);

					//add 'return' at end of init so a value can be returned
					DSInstr i = {OpCode::Return};
					classAttr.init->instr.push_back(i);
				}

				cls->attribs.push_back(classAttr);
			}

			//extract functions
			for(size_t j=0;j<classDef->functions.size();j++)
			{
				FunctionDefinition* funcDefNode = classDef->functions[j];


				Function* func = CompileFunction(funcDefNode);
				func->name = funcDefNode->name;
				func->sourceIndex = i;
				func->isStatic = funcDefNode->isStatic;

				cls->methods[funcDefNode->name]=func;
			}

			assembly->AddClass(cls);
		}

		for(size_t f=0;f<program->functions.size();f++)
		{
			FunctionDefinition* funcDefNode = program->functions[f];

			Function* func = CompileFunction(funcDefNode);
			func->name = funcDefNode->name;
			func->sourceIndex = i;

			assembly->AddFunction(func);
		}

	}

	//todo:
	//resolve parent class at instance creation
	//this allows for classes to be defined in other
	//libs and loaded after this lib has
		
	//resolve parent classes
	for(auto iter=assembly->classes.begin();iter!=assembly->classes.end();iter++)
	{
		if(iter->second->parentName=="")
			continue;

		if(iter->second->parentName==iter->second->name)
		{
			error = Error();
			error.message = string("class ")+iter->second->parentName+" cannot inherit itself";
			return false;
		}

		Class* parent = assembly->GetClass(iter->second->parentName);
		if(!parent)
		{
			error = Error();
			error.message = string("unable to find class ")+iter->second->parentName;
			return false;
		}
		else
		{
			iter->second->parent = parent;
		}
	}
		

	return true;
}

//compiles function node into instructions
Function* Compiler::CompileFunction(FunctionDefinition* funcDef)
{
	Function* func = new Function;

	//args are automatically variables
	/*
	vector<FunctionParameter*>::iterator iter;
	for(iter = funcDef->params.begin();iter != funcDef->params.end();iter++)
		func->args.push_back((*iter)->name);
	*/

	for(size_t k=0;k<funcDef->params.size();k++)
		func->args.push_back(funcDef->params[k]->name);

	DSInstr instr;

	for(size_t i=0;i<funcDef->statements.size();i++)
	{
		Statement* stmt = funcDef->statements[i];

		//can safely push line here
		PushLineOp(func,stmt->line);

		switch (stmt->type)
		{
		case ASTNode::ExprStmt:
				
			CompileExpression(func,((ExpressionStatement*)stmt)->expr);
			break;
		case ASTNode::IfStmt:
			CompileIfStatement(func,(IfStatement*)stmt);
			break;
		case ASTNode::WhileStmt:
			CompileWhileStatement(func,(WhileStatement*)stmt);
			break;
		case ASTNode::ReturnStmt:
			CompileExpression(func,((ReturnStatement*)stmt)->expr);
			instr.op = OpCode::Return;
			func->instr.push_back(instr);
			break;
		default:
			break;
		}
	}

	return func;
}

void Compiler::PushLineOp(Function* func,int line)
{
	//this is only done in debug mode
	if(!debug)
		return;

	DSInstr instr;
	instr.op = OpCode::Line;
	instr.val = line;

	func->instr.push_back(instr);
}

void Compiler::CompileBlock(Function* func,Block* block)
{
	//DSInstr instr;

	for(size_t i=0;i<block->statements.size();i++)
	{
		Statement* stmt = block->statements[i];
		CompileStatement(func,stmt);
	}
}

void Compiler::CompileStatement(Function* func,Statement* stmt)
{
	DSInstr instr;

	switch (stmt->type)
	{
	case ASTNode::ExprStmt:
		CompileExpressionStatement(func,((ExpressionStatement*)stmt));
		break;
	case ASTNode::IfStmt:
		CompileIfStatement(func,(IfStatement*)stmt);
		break;
	case ASTNode::WhileStmt:
		CompileWhileStatement(func,(WhileStatement*)stmt);
		break;
	case ASTNode::ReturnStmt:
		CompileExpression(func,((ReturnStatement*)stmt)->expr);
		instr.op = OpCode::Return;
		func->instr.push_back(instr);
		break;
	case ASTNode::BlockStmt:
		CompileBlock(func,(Block*)stmt);
		break;
	default:
		break;
	}
}

/*
Here's the reason for this intermediate function
when there is no assignment, the object returned from the expression
stays on the stack, taking up space until the function being called exits
the garbage collector cannot release these objects while in the function either,
even though they are not being used

if the expression statement being parsed isnt an assignment expression, append a 
'pop' instruction to the instruction list
*/
void Compiler::CompileExpressionStatement(Function* func,ExpressionStatement* stmt)
{
	//compile as normal
	CompileExpression(func,stmt->expr);

	//if expression isnt a binary expression, 
	if(stmt->expr->type == ASTNode::BinaryExpr)
	{
		BinaryExpression* binExpr = (BinaryExpression*)stmt->expr;
		//if it's an assign opertation
		if(binExpr->op!=Token::Assign)
		{
			//not an assign op
			//value returned will be left on the stack
			//so pop it
			DSInstr instr = {OpCode::Pop};
			func->instr.push_back(instr);
		}
	}
	else
	{
		//other expressions such as new or function calls are not binary
		//and their results need to be popped
		DSInstr instr = {OpCode::Pop};
		func->instr.push_back(instr);
	}
}

//an expression always pushes a value to the top of the stack 
//after it is executed
void Compiler::CompileExpression(Function* func,Expression* expr)
{
	BinaryExpression* binExpr;
	DSInstr instr;
	PropertyAccess* propExpr;
	CallExpr* callExpr;
	NewExpr* newExpr;
		
	string strVal = "";
	double numVal;
	bool boolVal;

	switch(expr->type)
	{
	case ASTNode::BinaryExpr:
		//push left and right values
		//push op
		binExpr = (BinaryExpression*)expr;
			
		switch(binExpr->op)
		{

		/* COMPARISONS */
		case Token::GT:
			CompileExpression(func,binExpr->left);
			CompileExpression(func,binExpr->right);
			instr.op = OpCode::IsGreaterThan;
			func->instr.push_back(instr);
			break;
		case Token::LT:
			CompileExpression(func,binExpr->left);
			CompileExpression(func,binExpr->right);
			instr.op = OpCode::IsLessThan;
			func->instr.push_back(instr);
			break;
		case Token::GTE:
			CompileExpression(func,binExpr->left);
			CompileExpression(func,binExpr->right);
			instr.op = OpCode::IsGreaterThanOrEqual;
			func->instr.push_back(instr);
			break;
		case Token::LTE:
			CompileExpression(func,binExpr->left);
			CompileExpression(func,binExpr->right);
			instr.op = OpCode::IsLessThanOrEqual;
			func->instr.push_back(instr);
			break;
		case Token::EQ:
			CompileExpression(func,binExpr->left);
			CompileExpression(func,binExpr->right);
			instr.op = OpCode::IsEqual;
			func->instr.push_back(instr);
			break;
		case Token::NEQ:
			CompileExpression(func,binExpr->left);
			CompileExpression(func,binExpr->right);
			instr.op = OpCode::IsNotEqual;
			func->instr.push_back(instr);
			break;
		/* MATH EXPRESSIONS */
		case Token::Add:
			CompileExpression(func,binExpr->left);
			CompileExpression(func,binExpr->right);
			instr.op = OpCode::Add;
			func->instr.push_back(instr);
			break;

		case Token::Sub:
			CompileExpression(func,binExpr->left);
			CompileExpression(func,binExpr->right);
			instr.op = OpCode::Sub;
			func->instr.push_back(instr);
			break;

		case Token::Mul:
			CompileExpression(func,binExpr->left);
			CompileExpression(func,binExpr->right);
			instr.op = OpCode::Mul;
			func->instr.push_back(instr);
			break;

		case Token::Div:
			CompileExpression(func,binExpr->left);
			CompileExpression(func,binExpr->right);
			instr.op = OpCode::Div;
			func->instr.push_back(instr);
			break;

		case Token::Assign:
				
			if(binExpr->left->type == ASTNode::Iden)
			{
				//if left node is just an identifier then
				//calculate right node and assign the value to that local

				CompileExpression(func,binExpr->right);

				func->strings.push_back(((Identifier*)binExpr->left)->name);
				instr.op = OpCode::StoreLocal;
				instr.val = func->strings.size()-1;
				func->instr.push_back(instr);
			}
			else if(binExpr->left->type == ASTNode::Var)
			{
				//same as iden
				CompileExpression(func,binExpr->right);

				func->strings.push_back(((VarExpr*)binExpr->left)->name);
				instr.op = OpCode::StoreLocal;
				instr.val = func->strings.size()-1;
				func->instr.push_back(instr);
			}
			else if(binExpr->left->type == ASTNode::PropAccess)
			{
				//if left node is a property access node:
				//1) evaluate node's right hand expression
				//2) evaluate node's expression (a recursion of property access and member calls)
				//3) call StoreProp instr with the string index as value

				CompileExpression(func,binExpr->right);
				CompileExpression(func,((PropertyAccess*)binExpr->left)->obj);

				func->strings.push_back(((PropertyAccess*)binExpr->left)->name);
				instr.op = OpCode::StoreProp;
				instr.val = func->strings.size()-1;
				func->instr.push_back(instr);
			}else
			{
				//a function call, this actually doesnt make sense being at the top of the
				//left expression tree of an assignment statement
				//if a value is returned assigning a value to it will cause
				//nothing to happen
			}
				
			break;
		}
		break;
	case ASTNode::Neg:
		//compile expression
		CompileExpression(func,((NegExpr*)expr)->child);

		//push negation opcode
		instr.op = OpCode::Neg;
		func->instr.push_back(instr);
		break;
	case ASTNode::Iden:
		//load local to top of stack
		func->strings.push_back(((Identifier*)expr)->name);
			
		//index is length of strings-1
		instr.val = func->strings.size()-1;
		instr.op = OpCode::LoadLocal;
		func->instr.push_back(instr);
		break;
	case ASTNode::BoolLiteral:
		boolVal = ((BoolLiteral*)expr)->value;
		instr.op = OpCode::LoadBool;
		instr.val = boolVal?1:0; //0 for false, 1 for true
		func->instr.push_back(instr);
		break;
	case ASTNode::NullLiteral:
		instr.op = OpCode::LoadNull;
		func->instr.push_back(instr);
		break;
	case ASTNode::NumberLiteral:
		//create constant
		numVal = ((NumberLiteral*)expr)->value;
		func->constants.push_back(Value::CreateNumber(numVal));

		//opcode for loading it
		instr.op = OpCode::LoadConstant;
		instr.val = func->constants.size()-1;
		func->instr.push_back(instr);

		break;
	case ASTNode::StringLiteral:
		/*should i store it as a string then load it
		or should i store it as a string constant
		decisons...decisions...
		*/
		strVal = ((StringLiteral*)expr)->value;
		func->constants.push_back(Value::CreateString(strVal.c_str()));

		instr.op = OpCode::LoadConstant;
		instr.val = func->constants.size()-1;
		func->instr.push_back(instr);
		break;

	case ASTNode::PropAccess:
		//eval expr, object should be at the top
		//call LoadProp to get property

		//step 1
		propExpr = (PropertyAccess*)expr;
		CompileExpression(func,propExpr->obj);

		//step 2
		func->strings.push_back(((PropertyAccess*)expr)->name);
		instr.op = OpCode::LoadProp;
		instr.val = func->strings.size()-1;
		func->instr.push_back(instr);
		break;

	case ASTNode::FunctionCall:
		callExpr = (CallExpr*)expr;
			
		//compile all arguments
		/*
		for(size_t i=0;i<callExpr->args->args.size();i++)
		{
			CompileExpression(func,callExpr->args->args[i]);
		}
		*/
		/*
		IMPORTANT FIX!
		the args list gets flushed after each function call or object
		instantiation
		if an argument is a function call or an object instantiation ten
		the function will be called using only the latter args

		calling AddArg after all the args have been evaulated fixes this
		since it's guaranteed that the args will be on the stack no matter
		what happens between their evaluation

		it means that the args will be added in reverse however
		this needs to be fixed in the appropriate section of code
		*/
		/*
		for(size_t i=0;i<callExpr->args->args.size();i++)
		{
			instr.op = OpCode::AddArg;
			func->instr.push_back(instr);
		}
		*/

		//if callExpr->obj is an Identifier, it is a static function call
		//else the function is being called from an object
		if(callExpr->obj->type == ASTNode::Iden)
		{
			/* PUSH ARGS START */
			for(size_t i=0;i<callExpr->args->args.size();i++)
			{
				CompileExpression(func,callExpr->args->args[i]);
			}

			for(size_t i=0;i<callExpr->args->args.size();i++)
			{
				instr.op = OpCode::AddArg;
				func->instr.push_back(instr);
			}
			/* PUSH ARGS END */

			instr.op = OpCode::CallFunction;
			func->strings.push_back(((Identifier*)callExpr->obj)->name);
			instr.val = func->strings.size()-1;
			func->instr.push_back(instr);

		}
		else if(callExpr->obj->type == ASTNode::PropAccess)
		{
			/*
			since functions arent first class objects, the PropAccess
			isnt compiled
			Its obj property is compiled
			the CallMethod opcode requires the val to be the index of the function
			name in the string table
			*/
			propExpr = (PropertyAccess*)callExpr->obj;

			CompileExpression(func,propExpr->obj);

			/* PUSH ARGS START */
			//args are pushed after prop is evaluated in the case of chaining
			//self.objects.get(0).update(dt);
			//after 'get' is called, the args will be cleared
			//it is important that the args be pushed after any previous function calls
			//are executed
			for(size_t i=0;i<callExpr->args->args.size();i++)
			{
				CompileExpression(func,callExpr->args->args[i]);
			}

			for(size_t i=0;i<callExpr->args->args.size();i++)
			{
				instr.op = OpCode::AddArg;
				func->instr.push_back(instr);
			}
			/* PUSH ARGS END */

			instr.op = OpCode::CallMethod;
			func->strings.push_back(propExpr->name);
			instr.val = func->strings.size()-1;
			func->instr.push_back(instr);
		}
		else
		{
			//SHOULD NEVER BE HERE!!
			assert(false);
		}

		break;
	case ASTNode::New:
		newExpr = (NewExpr*)expr;
			
		for(size_t i=0;i<newExpr->args->args.size();i++)
		{
			CompileExpression(func,newExpr->args->args[i]);
		}
			
		/*
		IMPORTANT FIX!
		the args list gets flushed after each function call or object
		instantiation
		if an argument is a function call or an object instantiation ten
		the function will be called using only the latter args

		calling AddArg after all the args have been evaulated fixes this
		since it's guaranteed that the args will be on the stack no matter
		what happens between their evaluation

		it means that the args will be added in reverse however
		this needs to be fixed in the appropriate section of code
		*/
		for(size_t i=0;i<newExpr->args->args.size();i++)
		{
			instr.op = OpCode::AddArg;
			func->instr.push_back(instr);
		}

		instr.op = OpCode::CreateInstance;
		func->strings.push_back(newExpr->name);
		instr.val = func->strings.size()-1;
		func->instr.push_back(instr);

		break;
	}
}

void Compiler::CompileWhileStatement(Function* func,WhileStatement* stmt)
{
	int exprPos = func->instr.size();//index of next stmt which is expr

	//compile comparison expression
	CompileExpression(func,stmt->expr);

	//if expression is false, jump to end of block
	//we dot know the position of the end of the block as yet so we
	//store jump op and get index
	int jumpOpIndex = func->instr.size();
	DSInstr instr = {OpCode::JumpIfFalse};
	func->instr.push_back(instr);

	//compile block
	CompileBlock(func,stmt->block);

	//jump back to expression
	instr.op = OpCode::Jump;
	instr.val = exprPos;
	func->instr.push_back(instr);

	//insert nop expr and get index
	int nopIndex = func->instr.size();
	func->instr[jumpOpIndex].val = nopIndex;
	instr.op = OpCode::Nop;
	func->instr.push_back(instr);
}

/*
basic concept format:
if expression
jump if false - to block end
block
jump to nop
blockend
nop

format with else chain example
if expression
jump if false - to block end
block
jump to nop
blockend
	else
nop

*/
void Compiler::CompileIfStatement(Function* func,IfStatement* stmt)
{
	//IF EXPRESSION
	CompileExpression(func,stmt->expr);

	//JUMP IF FALSE TO BLOCK END
	//if expression is false, jump to end of block
	//we dot know the position of the end of the block as yet so we
	//store jump op and get index
	int blockEndIndex = func->instr.size();
	DSInstr instr = {OpCode::JumpIfFalse};
	func->instr.push_back(instr);

	//BLOCK
	CompileBlock(func,stmt->block);
		
	//JUMP TO NOP
	//if the expression was true then the body was executed
	//we skip the others statements in the else chain
	//store jump index for setting later
	int endChainIndex = func->instr.size();
	instr.op = OpCode::Jump;
	func->instr.push_back(instr);

	//END OF BLOCK
	int nopIndex = func->instr.size();
	func->instr[blockEndIndex].val = nopIndex;
	instr.op = OpCode::Nop;
	func->instr.push_back(instr);

	//ELSE CHAIN
	//if there's an else, compile that too
	if(stmt->elseStmt)
	{
		CompileStatement(func,stmt->elseStmt);
	}

	//NOP
	nopIndex = func->instr.size();
	func->instr[endChainIndex].val = nopIndex;
	instr.op = OpCode::Nop;
	func->instr.push_back(instr);
}

Error Compiler::GetError()
{
	return error;
}