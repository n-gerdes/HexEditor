#include "hex_editor.h"
#ifndef UTILITY_H
#define UTILITY_H
namespace hex_editor
{
	namespace utility
	{
		std::string dec_to_hex(unsigned char dec);
		unsigned char hex_to_dec(const std::string& hex);
		unsigned char binary_to_dec(const std::string& binary);
		std::string dec_to_binary(unsigned char dec);
		std::string hex_to_binary(const std::string& hex);
		std::string binary_to_hex(const std::string& binary);
		int get_random_int(int lower_bound, int upper_bound);
		std::string& make_lowercase(std::string& string);
		std::string to_lowercase(const std::string& string);

		bool is_valid_hex(const std::string& hex);
		bool is_valid_dec(const std::string& dec);
		bool is_valid_binary(const std::string& binary);
		bool contains_hex_letters(const std::string& str);
		format_mode detect_formatting(const std::string& num, format_mode preferred);
		//std::string format_number(const std::string& num, const program_data& program);
		unsigned char convert_token_to_unsigned_char(const std::string& token, hex_editor::format_mode preferred); //Converts a user-entered token to a numeric byte, can throw an error if that's not valid
	}
}
#endif
