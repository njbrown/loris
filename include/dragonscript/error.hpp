#pragma once
#include <string>

class Error
{
public:
	enum Type
	{
		NONE,
		UNKOWN_CHAR,
		UNEXPECTED_TOKEN,
		INVALID_OPERATION
	};

	Type code;
	std::string message;
	int line;
	std::string filename;

	Error()
	{
		code = NONE;
		line = -1;
	}

	static Error InvalidOperation(std::string message)
	{
		Error e = Error();
		e.message = message;
		e.code = INVALID_OPERATION;

		return e;
	}
};