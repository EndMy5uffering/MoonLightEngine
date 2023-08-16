#include "MLException.h"

const char* MLException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

int MLException::GetLine() const noexcept
{
	return line;
}

const std::string& MLException::GetFile() const noexcept
{
	return file;
}

std::string MLException::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File] " << file << "\n" << "[Line] " << line;

	return oss.str();
}

