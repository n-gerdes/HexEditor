#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "hex_editor.h"

//tokenizer class is responsible for taking raw input from the user and turning it into a list of tokens
class hex_editor::tokenizer
{
public:

	std::vector<std::string> extract_tokens(const std::string& input, const program_data& program);

protected:

private:

	void post_processing(std::string& token, const program_data& program) const;

};

#endif