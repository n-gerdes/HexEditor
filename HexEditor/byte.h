#include "hex_editor.h"

#ifndef BYTE_H
#define BYTE_H

class hex_editor::byte
{
public:

	byte();
	byte(unsigned char initial_value) : value(initial_value) {}
	byte(const std::string& initial_value, const hex_editor::program_data& program);

	unsigned char get_value();

	std::string get_value_binary();
	std::string get_value_dec();
	std::string get_value_hex();

	void set_value(unsigned char dec_value);
	void set_value(const std::string& string_value, const hex_editor::program_data& program);

private:
	unsigned char value{ 0 };
};

#endif