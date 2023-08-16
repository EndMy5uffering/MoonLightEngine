#pragma once

#include <exception>
#include <string>
#include <sstream>

class MLException : public std::exception
{
public:
	MLException(int line, const char* file) noexcept : std::exception(), line(line), file(file) {}
	const char* what() const noexcept override;
	virtual const char* GetType() const noexcept { return "MLException"; }
	int GetLine() const noexcept;
	const std::string& GetFile() const noexcept;
	std::string GetOriginString() const noexcept;

private:
	int line;
	std::string file;
protected:
	mutable std::string whatBuffer;
};

