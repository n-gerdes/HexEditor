// hex_editor.cpp : Defines the entry point for the application.
//

#include "hex_editor.h"
#include "byte.h"
#include "program_data.h"
#include <random>
#include <time.h>
#include "utility.h"

void process_command(hex_editor::program_data& program)
{

	auto command = program.get_commands().get_command(hex_editor::utility::to_lowercase(program.tokens[0]));
	if (command == nullptr)
	{
		program.print("Unrecognized command");
	}
	else
	{
		if (program.get_commands().command_matches(command, "repeat")) //If the command is the repeat command or an alias of it, switch back to the previous command and run that
		{
			program.input = program.get_previous_input();
			program.tokens = program.get_tokenizer().extract_tokens(program.input, program);
			process_command(program);
			return;
		}
		else //If it's not the repeat command, save the previous tokens in case the next one is
		{
			program.remember_previous_input();
		}
		if (program.get_current_file() == nullptr)
		{
			if (program.get_commands().is_universal_command(command))
			{
				(*command)(program, program.tokens);
			}
			else
			{
				program.print("Error: This command can only be used if a file is open");
			}
		}
		else
		{
			try
			{
				(*command)(program, program.tokens);
			}
			catch (const std::exception& E)
			{
				program.print("Error: Failed to execute command");
			}
		}
	}
	
}

void clear_screen()
{
	for (size_t i = 0; i < 100; i++)
		std::cout << std::endl;
}

void print_messages(const hex_editor::program_data& program)
{
	for (size_t i = 0; i < program.messages.size(); i++)
		std::cout << program.messages[i] << std::endl;
	
}

static hex_editor::program_data program;

int main()
{
	std::cout << "Text Hex Editor" << std::endl;
	srand(time(NULL));
	program.get_commands().register_commands();
	program.load_editor_settings();

	while (program.is_open())
	{
		while (program.input.size() == 0)
			std::getline(std::cin, program.input);
		
		program.tokens = program.get_tokenizer().extract_tokens(program.input, program);
		process_command(program); //Process input from the user

		clear_screen();

		if(program.get_current_file())
			program.get_current_file()->print_page(program); //Update the display

		program.print_messages(); //If there are any messages to print, do that now.

		program.input = "";
	}

	program.save_editor_settings();
}
