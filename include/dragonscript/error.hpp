#pragma once
#include <string>

class Error
{
public:
	enum Type
	{
		UNKOWN_CHAR,
		UNEXPECTED_TOKEN,
		INVALID_OPERATION
	};

	Type code;
	std::string message;
	int line;
	std::string filename;

	static Error* InvalidOperation(std::string message)
	{
		Error* e = new Error();
		e->message = message;
		e->code = INVALID_OPERATION;

		return e;
	}
};