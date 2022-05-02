#include "NativeDialog.hpp"
#include "Utility.hpp"

#ifdef _WIN32
#include "Windows.h"
#endif

InputResult BasicInput(const std::string& prompt)
{
	return InputResult();
}
