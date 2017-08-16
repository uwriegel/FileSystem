#include "nantools.h"
#include "tools.h"

wstring get_string_value(const Local<Value>& value)
{
	if (value->IsNullOrUndefined())
		return wstring();

	const auto& utf8 = static_cast<Utf8String>(value);
	auto length = utf8.length();
	if (length == 0)
		return wstring();

	return move(convert_to_utf16(move(string(*utf8, length))));
}