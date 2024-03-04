#include "tokenizer.h"
#include "byte.h"
#include "program_data.h"
#include "utility.h"

void hex_editor::tokenizer::post_processing(std::string& token, const program_data& program) const
{
	//If it's surrounded by brackets then it's meant to be resolved to a random number in a range
	if (token[0] == '[' && token[token.length() - 1] == ']')
	{
		token = token.substr(1, token.length() - 2); //Remove the brackets

		if (token.length() == 0) //Empty brackets resolve to 0
		{
			token = "0";
			return;
		}

		size_t pivot = 0;

		auto is_numeric = [&](char character) -> bool
		{
			switch (character)
			{
				case '0': return true;
				case '1': return true;
				case '2': return true;
				case '3': return true;
				case '4': return true;
				case '5': return true;
				case '6': return true;
				case '7': return true;
				case '8': return true;
				case '9': return true;
				default: return false;
			}
		};

		auto is_string_numeric = [&](const std::string& string) -> bool
		{
			for (size_t i = 0; i < string.length(); i++)
			{
				if (!is_numeric(string[i]))
					return false;
			}
			return true;
		};

		while (pivot < token.length() && !(token[pivot]==',' || token[pivot] == '-'))
			pivot++;

		if (pivot != token.length() && pivot != token.length() - 1 && pivot != 0) //If it found a pivot character (and it wasn't the first or last character)
		{
			std::string lower_bound_token = token.substr(0, pivot);
			std::string upper_bound_token = token.substr(pivot + 1);

			if (is_string_numeric(lower_bound_token) && is_string_numeric(upper_bound_token))
			{
				int upper_bound = std::stoi(upper_bound_token);
				int lower_bound = std::stoi(lower_bound_token);

				int roll = hex_editor::utility::get_random_int(lower_bound, upper_bound);

				if (program.get_display_mode() == hex_editor::format_mode::hexadecimal)
				{
					token = hex_editor::utility::dec_to_hex(roll);
				}
				else if(program.get_display_mode() == hex_editor::format_mode::binary)
				{
					token = hex_editor::utility::dec_to_binary(roll);
				}
				else
				{
					token = std::to_string(roll);
				}
				
			}
		}
	}
}

std::vector<std::string> hex_editor::tokenizer::extract_tokens(const std::string& input, const program_data& program)
{
	std::vector<std::string> tokens;

	std::string token;

	auto push_token = [&]()
	{
		if (token.size() > 0)
		{
			post_processing(token, program);
			tokens.push_back(token);
			token = "";
		}
	};

	for (size_t i = 0; i < input.size(); i++)
	{
		char character = input.at(i);
		if (character != ' ')
		{
			token += character;
		}
		else
		{
			push_token();
		}
	}
	push_token();
	return tokens;
}