#include "byte.h"
#include "program_data.h"
#include "utility.h"

hex_editor::byte::byte()
{

}

hex_editor::byte::byte(const std::string& initial_value, const hex_editor::program_data& program)
{
	set_value(initial_value, program);
}

unsigned char hex_editor::byte::get_value()
{
	return value;
}

std::string hex_editor::byte::get_value_dec()
{
	std::string dec_value = std::to_string(static_cast<unsigned>(value));
	if (dec_value.size() == 2)
		dec_value = " " + dec_value;

	if (dec_value.size() == 1)
		dec_value = "  " + dec_value;

	return dec_value;
}

std::string hex_editor::byte::get_value_hex()
{
	return hex_editor::utility::dec_to_hex(get_value());
}

std::string hex_editor::byte::get_value_binary()
{
	return hex_editor::utility::dec_to_binary(get_value());
}

void hex_editor::byte::set_value(unsigned char dec_value)
{
	value = dec_value;
}

//Dynamically sets the value of this byte using an input string. It uses the length of the string to assume the type it's getting (decimal, binary, or hexadecimal)
void hex_editor::byte::set_value(const std::string& string_value, const hex_editor::program_data& program)
{
	value = hex_editor::utility::convert_token_to_unsigned_char(string_value, program.get_display_mode());
}
