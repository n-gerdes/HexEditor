#include "hex_editor.h"
#include "program_data.h"
#include "utility.h"

std::string hex_editor::utility::dec_to_hex(unsigned char dec) 
{
/*This works by matching letters / characters to bit patterns.
4 bits has 16 possible combinations, and there are 16 different hex symbols.
The first character corresponds to the upper 4 bits, while the second character corresponds to the lower 4 bits.
*/
	unsigned char lower_4_bits = dec & 0b00001111; //Get the last 4 bits
	unsigned char upper_4_bits = (dec & 0b11110000) >> 4; //Get the first 4 bits

	auto get_char_from_bits = [](unsigned char bit_mask) -> char
	{
		switch (bit_mask)
		{
			case 0: {return '0'; }
			case 1: {return '1'; }
			case 2: {return '2'; }
			case 3: {return '3'; }
			case 4: {return '4'; }
			case 5: {return '5'; }
			case 6: {return '6'; }
			case 7: {return '7'; }
			case 8: {return '8'; }
			case 9: {return '9'; }
			case 10: {return 'A'; }
			case 11: {return 'B'; }
			case 12: {return 'C'; }
			case 13: {return 'D'; }
			case 14: {return 'E'; }
			case 15: {return 'F'; }
		}
		return '0';
	};

	char first_char = get_char_from_bits(upper_4_bits);
	char second_char = get_char_from_bits(lower_4_bits);

	std::string hex_value{ first_char };
	hex_value += second_char;

	return hex_value;
}

unsigned char hex_editor::utility::hex_to_dec(const std::string& hex)
{
	unsigned char place_value = std::pow(16, hex.size() - 1);
	unsigned char temp_value = 0;

	for (size_t i = 0; i < hex.size(); i++)
	{
		char digit = std::toupper(hex.at(i));

		int8_t value_of_digit = -1;
		switch (digit)
		{
			case '0': {value_of_digit = 0; break; }
			case '1': {value_of_digit = 1; break; }
			case '2': {value_of_digit = 2; break; }
			case '3': {value_of_digit = 3; break; }
			case '4': {value_of_digit = 4; break; }
			case '5': {value_of_digit = 5; break; }
			case '6': {value_of_digit = 6; break; }
			case '7': {value_of_digit = 7; break; }
			case '8': {value_of_digit = 8; break; }
			case '9': {value_of_digit = 9; break; }
			case 'A': {value_of_digit = 10; break; }
			case 'B': {value_of_digit = 11; break; }
			case 'C': {value_of_digit = 12; break; }
			case 'D': {value_of_digit = 13; break; }
			case 'E': {value_of_digit = 14; break; }
			case 'F': {value_of_digit = 15; break; }
			default:
			{
				std::exception E;
				throw E;
			}
		};

		temp_value += value_of_digit * place_value;
		place_value /= 16;
	}

	return temp_value;
}

std::string hex_editor::utility::hex_to_binary(const std::string& hex)
{
	return dec_to_binary(hex_to_dec(hex));
}

std::string hex_editor::utility::binary_to_hex(const std::string& binary)
{
	return dec_to_hex(binary_to_dec(binary));
}

unsigned char hex_editor::utility::binary_to_dec(const std::string& binary)
{
	unsigned char number = 0;
	for (unsigned char i = 0; i < 8; i++)
	{
		unsigned char index = 7 - i;

		if (binary[index] == '1')
		{
			unsigned char temp_bit = (1) << i;
			number = number | temp_bit;
		}
	}

	return number;
}

std::string hex_editor::utility::dec_to_binary(unsigned char dec)
{
	std::string binary = "00000000";
	for (unsigned char i = 0; i < 8; i++)
	{
		bool bit_value = (dec >> i) & 1;
		
		if(bit_value)
			binary[7 - i] = '1';
	}

	return binary;
}

int hex_editor::utility::get_random_int(int lower_bound, int upper_bound)
{
	if (upper_bound < lower_bound)
		std::swap(upper_bound, lower_bound);
	int range = upper_bound - lower_bound + 1;
	int roll = (rand() % range) + lower_bound;
	return roll;
}

bool hex_editor::utility::contains_hex_letters(const std::string& hex)
{
	for (size_t i = 0; i < hex.length(); i++)
	{
		char character = std::tolower(hex[i]);
		switch (character)
		{
		case 'a': break;
		case 'b': break;
		case 'c': break;
		case 'd': break;
		case 'e': break;
		case 'f': break;
		default: return false;
		}
	}
	return true;
}

bool hex_editor::utility::is_valid_hex(const std::string& hex)
{
	for (size_t i = 0; i < hex.length(); i++)
	{
		char character = std::tolower(hex[i]);
		switch (character)
		{
			case 'a': break;
			case 'b': break;
			case 'c': break;
			case 'd': break;
			case 'e': break;
			case 'f': break;
			case '0': break;
			case '1': break;
			case '2': break;
			case '3': break;
			case '4': break;
			case '5': break;
			case '6': break;
			case '7': break;
			case '8': break;
			case '9': break;
			default: return false;
		}
	}
	return true;
}

bool hex_editor::utility::is_valid_dec(const std::string& dec)
{
	for (size_t i = 0; i < dec.length(); i++)
	{
		char character = std::tolower(dec[i]);
		switch (character)
		{
			case '0': break;
			case '1': break;
			case '2': break;
			case '3': break;
			case '4': break;
			case '5': break;
			case '6': break;
			case '7': break;
			case '8': break;
			case '9': break;
			default: return false;
		}
	}
	return true;
}

bool hex_editor::utility::is_valid_binary(const std::string& binary)
{
	for (size_t i = 0; i < binary.length(); i++)
	{
		char character = std::tolower(binary[i]);
		switch (character)
		{
			case '0': break;
			case '1': break;
			default: return false;
		}
	}
	return true;
}

hex_editor::format_mode hex_editor::utility::detect_formatting(const std::string& num, hex_editor::format_mode preferred)
{
	bool valid_binary = true; //Also implies valid_dec and valid_hex
	bool valid_dec = true; //Also implies valid_hex
	bool valid_hex = true;
	const size_t& length = num.length();


	//Goes through the characters and determines which possible combinations it's legal for.
	for (size_t i = 0; i < length; i++)
	{
		char character = std::tolower(num[i]);
		switch (character)
		{
			case '0': break;
			case '1': break;
			case '2':
			{
				valid_binary = false;
				break;
			}
			case '3':
			{
				valid_binary = false;
				break;
			}
			case '4':
			{
				valid_binary = false;
				break;
			}
			case '5':
			{
				valid_binary = false;
				break;
			}
			case '6':
			{
				valid_binary = false;
				break;
			}
			case '7':
			{
				valid_binary = false;
				break;
			}
			case '8':
			{
				valid_binary = false;
				break;
			}
			case '9':
			{
				valid_binary = false;
				break;
			}
			case 'a':
			{
				valid_binary = false;
				valid_dec = false;
				break;
			}
			case 'b':
			{
				valid_binary = false;
				valid_dec = false;
				break;
			}
			case 'c':
			{
				valid_binary = false;
				valid_dec = false;
				break;
			}
			case 'd':
			{
				valid_binary = false;
				valid_dec = false;
				break;
			}
			case 'e':
			{
				valid_binary = false;
				valid_dec = false;
				break;
			}
			case 'f':
			{
				valid_binary = false;
				valid_dec = false;
				break;
			}
			default:
			{
				valid_binary = false;
				valid_dec = false;
				valid_hex = false;
				break;
			}
		}
	}

	if (preferred == hex_editor::format_mode::hexadecimal)
	{
		if (valid_binary && length > 3)
		{
			return hex_editor::format_mode::binary;
		}
		else if (valid_dec && length == 3) //If it's not valid binary but it is valid decimal that's too long to be hex, return that
		{
			return hex_editor::format_mode::decimal;
		}
		else if (valid_hex) //If it's not valid binary or hex, but it is valid hex, return that.
		{
			return hex_editor::format_mode::hexadecimal;
		}
		else //If it's not valid, throw an exception
		{
			std::exception E;
			throw E;
		}
	}
	else if (preferred == hex_editor::format_mode::decimal)
	{
		if (valid_binary && length > 3) //If it's valid binary and longer than normal 
		{
			return hex_editor::format_mode::binary;
		}
		else if (valid_dec) //If it's valid decimal (and potentially also valid hexadecimal)
		{
			return hex_editor::format_mode::decimal;
		}
		else if (valid_hex) //If it's valid hex but not good binary or dec
		{
			return hex_editor::format_mode::hexadecimal;
		}
		else
		{
			std::exception E; //If it's not valid, return error.
			throw E;
		}
	}
	else if (preferred == hex_editor::format_mode::binary)
	{
		if (valid_binary)
		{
			return hex_editor::format_mode::binary;
		}
		else if (valid_dec)
		{
			return hex_editor::format_mode::decimal;
		}
		else if (valid_hex)
		{
			return hex_editor::format_mode::hexadecimal;
		}
		else
		{
			std::exception E;
			throw E;
		}
	}
}

unsigned char hex_editor::utility::convert_token_to_unsigned_char(const std::string& token, hex_editor::format_mode preferred)
{
	std::exception E;
	if (!is_valid_hex(token) && !is_valid_dec(token) && !is_valid_binary(token))
	{
		throw E;
	}

	hex_editor::format_mode detected_type = detect_formatting(token, preferred);

	if (detected_type == hex_editor::format_mode::hexadecimal)
	{
		return hex_to_dec(token);
	}
	else if (detected_type == hex_editor::format_mode::decimal)
	{
		int value = std::stoi(token);
		if (value > 255 || value < 0)
			throw E;

		return static_cast<unsigned char>(value);
	}
	else if (detected_type == hex_editor::format_mode::binary)
	{
		return binary_to_dec(token);
	}
}

//Pass by Reference, changes input string
std::string& hex_editor::utility::make_lowercase(std::string& string)
{
	for (size_t i = 0; i < string.size(); i++)
		string[i] = std::tolower(string[i]);

	return string;
}

//Pass by value, returns a lowercase copy
std::string hex_editor::utility::to_lowercase(const std::string& string)
{
	std::string copy = string;
	return make_lowercase(copy);
}