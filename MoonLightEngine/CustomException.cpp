#include "CustomException.h"
#include <sstream>

CustomException::CustomException(int line, const char* file) noexcept : line(line), file(file)
{
}

const char* CustomException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();

	return whatBuffer.c_str();
}

const char* CustomException::GetType() const noexcept
{
	return "Custom Exception";
}

int CustomException::GetLine() const noexcept
{
	return 0;
}

const std::string& CustomException::GetFile() const noexcept
{
	// TODO: insert return statement here
}

std::string CustomException::GetOriginString() const noexcept
{
	return std::string();
}
