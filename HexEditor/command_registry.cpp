#include "command_registry.h"
#include "program_data.h"
#include "utility.h"
#include <filesystem>
//This is where command functions are registered with their keywords and aliases

void hex_display(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	if (tokens.size() != 1)
		program.print("Error: invalid number of arguments");
	else
	{
		program.set_display_mode(hex_editor::format_mode::hexadecimal);
		program.save_editor_settings();
	}
}

void dec_display(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	if (tokens.size() != 1)
		program.print("Error: invalid number of arguments");
	else
	{
		program.set_display_mode(hex_editor::format_mode::decimal);
		program.save_editor_settings();
	}
}

void binary_display(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	if (tokens.size() != 1)
		program.print("Error: invalid number of arguments");
	else
	{
		program.set_display_mode(hex_editor::format_mode::binary);
		program.save_editor_settings();
	}
}

void select_byte(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	if (tokens.size() > 3)
	{
		program.print("Error: invalid number of arguments");
	}
	else
	{
		if (tokens.size() == 1)
		{
			program.print("Selected byte is " + std::to_string(program.get_current_file()->get_selected_byte()));
			return;
		}
		else if (tokens.size() == 2)
		{
			try
			{
				size_t selected_byte = std::stoi(tokens[1]);
				program.get_current_file()->record_buffer(program);
				program.get_current_file()->select_byte(selected_byte);
			}
			catch (const std::exception& E)
			{
				program.print("Error: Failed to parse arguments");
			}
		}
		else if (tokens.size() == 3)
		{
			std::string argument = hex_editor::utility::to_lowercase(tokens[1]);
			if (argument == "page")
			{
				try
				{
					int64_t page_number = std::stoi(tokens[2]);

					if (page_number > program.get_current_file()->get_number_of_pages(program))
					{
						page_number = program.get_current_file()->get_number_of_pages(program);
					}
					if (page_number <= 0)
					{
						program.get_current_file()->select_byte(0);
					}
					else
					{
						int64_t page_difference = (page_number - static_cast<int64_t>(program.get_current_file()->get_page_number(program)));
						program.get_current_file()->record_buffer(program);
						program.get_current_file()->select_byte(program.get_current_file()->get_selected_byte() + (page_difference * program.get_bytes_per_page()));
					}
				}
				catch (const std::exception& E)
				{
					program.print("Error: Failed to parse arguments");
				}
			}
			else if (argument == "byte")
			{
				try
				{
					size_t selected_byte = std::stoi(tokens[2]);
					program.get_current_file()->record_buffer(program);
					program.get_current_file()->select_byte(selected_byte);
				}
				catch (const std::exception& E)
				{
					program.print("Error: Failed to parse arguments");
				}
			}
			else
			{
				program.print("Error: Failed to parse arguments");
				return;
			}
			
		}
	}
}

void append_bytes(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	if (tokens.size() == 1)
	{
		program.print("Error: invalid number of arguments");
	}
	if (tokens.size() == 2)
	{
		program.get_current_file()->record_buffer(program);
		program.get_current_file()->append(program, tokens[1], 1);
	}
	else if (tokens.size() == 3)
	{
		program.get_current_file()->record_buffer(program);
		program.get_current_file()->append(program, tokens[1], std::stoi(tokens[2]));
	}
	else
	{
		program.print("Error: invalid number of arguments");
	}
}

void print_size(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	if (tokens.size() != 1)
		program.print("Error: invalid number of arguments");
	else
		program.print("file is " + std::to_string(program.get_current_file()->get_size()) + " bytes");
}

void set_byte(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	if (tokens.size() == 2)
	{
		try
		{
			program.get_current_file()->record_buffer(program);
			program.get_current_file()->change_byte(program, program.get_current_file()->get_selected_byte(), tokens[1]);
		}
		catch(const std::exception& E)
		{
			program.print("Error");
		}
	}
	else if (tokens.size() == 3)
	{
		try
		{
			program.get_current_file()->record_buffer(program);
			program.get_current_file()->change_byte(program, std::stoi(tokens[1]), tokens[2]);
		}
		catch (const std::exception& E)
		{
			program.print("Error: Failed to parse arguments");
		}
	}
	else if (tokens.size() == 3 || tokens.size() > 4)
	{
		program.print("Error: Invalid number of arguments");
	}
	else
	{
		size_t first_byte = std::stoi(tokens[1]);
		size_t last_byte = std::stoi(tokens[2]);
		//const std::string& new_value = hex_editor::utility::format_number(tokens[3], program);

		program.get_current_file()->record_buffer(program);

		size_t max_byte = program.get_current_file()->get_size() - 1;
		if (last_byte > max_byte)
			last_byte = max_byte;

		for (size_t byte_id = first_byte; byte_id <= last_byte; byte_id++)
			program.get_current_file()->change_byte(program, byte_id, tokens[3]);

	}
}

void shrink_file(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	if(tokens.size() == 2)
	{
		int bytes_shrunk = std::stoi(tokens[1]);
		program.get_current_file()->record_buffer(program);
		program.get_current_file()->shrink(bytes_shrunk);
	}
	else
	{
		program.print("Error: Invalid number of arguments");
	}
}

void resize_file(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	try
	{
		size_t new_size = std::stoi(tokens[1]);
		program.get_current_file()->record_buffer(program);
		if (program.get_current_file()->get_size() > new_size) //If file must shrink
		{
			program.get_current_file()->shrink(program.get_current_file()->get_size() - new_size);
		}
		else if (program.get_current_file()->get_size() < new_size) //If file must grow
		{
			program.get_current_file()->append(program, "0", new_size - program.get_current_file()->get_size());
		}
	}
	catch (const std::exception& E)
	{
		program.print("Error: Failed to parse arguments");
	}
}

void set_page_width(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	try
	{
		if (tokens.size() == 2)
		{
			program.set_bytes_per_line(std::stoi(tokens[1]));
			program.save_editor_settings();
		}
		else if (tokens.size() == 1)
		{
			program.print(std::to_string(program.get_bytes_per_line()) + " bytes per line.");
		}
		else
		{
			program.print("Error: Invalid number of arguments");
		}
	}
	catch (const std::exception& E)
	{
		program.print("Error: Failed to parse arguments");
	}
}

void set_page_length(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	try
	{
		if (tokens.size() == 2)
		{
			program.set_lines_per_page(std::stoi(tokens[1]));
			program.save_editor_settings();
		}
		else if (tokens.size() == 1)
		{
			program.print(std::to_string(program.get_bytes_per_line()) + " lines per page");
		}
		else
		{
			program.print("Error: Invalid number of arguments");
		}
	}
	catch (const std::exception& E)
	{
		program.print("Error: Failed to parse arguments");
	}
}

void go_to_page(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	if (tokens.size() != 2)
	{
		program.print("Error: invalid number of arguments");
	}
	else
	{
		try
		{
			int64_t page_number = std::stoi(tokens[1]);

			if (page_number > program.get_current_file()->get_number_of_pages(program))
			{
				page_number = program.get_current_file()->get_number_of_pages(program);
			}
			if (page_number <= 0)
			{
				program.get_current_file()->select_byte(0);
			}
			else
			{
				int64_t page_difference = (page_number - static_cast<int64_t>(program.get_current_file()->get_page_number(program)));
				program.get_current_file()->record_buffer(program);
				program.get_current_file()->select_byte(program.get_current_file()->get_selected_byte() + (page_difference * program.get_bytes_per_page()));
			}
		}
		catch (const std::exception& E)
		{
			program.print("Error: Failed to parse arguments");
		}
	}
}

void move_right(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	size_t new_selection = program.get_current_file()->get_selected_byte();
	if (tokens.size() == 1)
	{
		new_selection++;
	}
	else if (tokens.size() == 2)
	{
		try
		{
			int shift = std::stoi(tokens[1]);
			new_selection += shift;
		}
		catch (const std::exception& E)
		{
			program.print("Error: Failed to parse arguments");
			return;
		}
	}
	else
	{
		program.print("Error: Invalid number of arguments");
		return;
	}

	if (new_selection < 0)
		new_selection = 0;
	
	if (new_selection >= program.get_current_file()->get_size())
		new_selection = program.get_current_file()->get_size() - 1;
	
	program.get_current_file()->record_buffer(program);
	program.get_current_file()->select_byte(new_selection);
}

void move_left(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	size_t new_selection = program.get_current_file()->get_selected_byte();
	if (tokens.size() == 1)
	{
		new_selection--;
	}
	else if (tokens.size() == 2)
	{
		try
		{
			int shift = std::stoi(tokens[1]);
			new_selection -= shift;
		}
		catch (const std::exception& E)
		{
			program.print("Error: Failed to parse arguments");
			return;
		}
	}
	else
	{
		program.print("Error: Invalid number of arguments");
		return;
	}

	if (new_selection < 0)
		new_selection = 0;

	if (new_selection >= program.get_current_file()->get_size())
		new_selection = program.get_current_file()->get_size() - 1;

	program.get_current_file()->record_buffer(program);
	program.get_current_file()->select_byte(new_selection);
}

void move_up(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	size_t new_selection = program.get_current_file()->get_selected_byte();
	if (tokens.size() == 1)
	{
		new_selection -= program.get_bytes_per_line();
	}
	else if (tokens.size() == 2)
	{
		try
		{
			int shift = std::stoi(tokens[1]) * program.get_bytes_per_line();
			new_selection -= shift;
		}
		catch (const std::exception& E)
		{
			program.print("Error: Failed to parse arguments");
			return;
		}
	}
	else
	{
		program.print("Error: Invalid number of arguments");
		return;
	}

	if (new_selection < 0)
		new_selection = 0;

	if (new_selection >= program.get_current_file()->get_size())
		new_selection = program.get_current_file()->get_size() - 1;

	program.get_current_file()->record_buffer(program);
	program.get_current_file()->select_byte(new_selection);
}

void move_down(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	size_t new_selection = program.get_current_file()->get_selected_byte();
	if (tokens.size() == 1)
	{
		new_selection += program.get_bytes_per_line();
	}
	else if (tokens.size() == 2)
	{
		try
		{
			int shift = std::stoi(tokens[1]) * program.get_bytes_per_line();
			new_selection += shift;
		}
		catch (const std::exception& E)
		{
			program.print("Error: Failed to parse arguments");
			return;
		}
	}
	else
	{
		program.print("Error: Invalid number of arguments");
		return;
	}

	if (new_selection < 0)
		new_selection = 0;

	if (new_selection >= program.get_current_file()->get_size())
		new_selection = program.get_current_file()->get_size() - 1;

	program.get_current_file()->record_buffer(program);
	program.get_current_file()->select_byte(new_selection);
}

void save(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	if (tokens.size() == 1)
	{
		if (program.get_current_file())
		{
			program.save_editor_settings();
			int return_code = program.get_current_file()->write_to_file();
			switch (return_code)
			{
				case 0: {program.print("saved " + program.get_current_file()->get_directory()); break; }
				default: {program.print("Error: Failed to write"); break; }
			}

		}
	}
	else if (tokens.size() == 2)
	{
		try
		{
			if (program.get_open_files().count(tokens[2]) == 0)
			{
				program.print("Error: no such file open");
			}
			else
			{
				program.save_editor_settings();
				auto file = program.get_open_files()[tokens[2]];
				int error_code = file->write_to_file();
				switch (error_code)
				{
				case 0: {program.print("saved " + file->get_directory()); break; }
				default: {program.print("Error: Failed to write"); break; }
				}
			}
		}
		catch (const std::exception& E)
		{
			program.print("Error");
		}
	}
	else
	{
		program.print("Error: Invalid number of arguments");
	}
}

void save_as(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	if (tokens.size() == 2)
	{
		const std::string& new_name = tokens[1];
		auto open_file = program.get_current_file();
		open_file->set_directory(new_name);
		int error_code = open_file->write_to_file();

		switch (error_code)
		{
			case 0: {program.print("saved " + open_file->get_directory()); break; }
			default: {program.print("Error: Failed to write (code " + std::to_string(error_code)); break; }
		}
	}
	else
	{
		program.print("Error: Invalid number of arguments");
	}

}

void open_file(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	try //If file is already open, switch to it
	{
		std::shared_ptr<hex_editor::file> existing_file = program.get_open_files().at(tokens[1]);
		program.get_current_file() = existing_file;
	}
	catch (const std::exception& E) //If file is not currently open in memory
	{
		const std::string& filename = tokens[1];
		program.get_open_files().insert_or_assign(filename, std::make_shared<hex_editor::file>());
		auto newly_opened_file = program.get_open_files()[filename];
		newly_opened_file->open(program, filename);
		if (newly_opened_file->is_open())
			program.get_current_file() = newly_opened_file;
		else
		{
			program.get_open_files().erase(filename);
		}
	}
}

void close_file_by_directory(hex_editor::program_data& program, const std::string& name)
{
	if (program.get_open_files().count(name) == 0)
	{
		program.print("Error: no such file is open");
	}
	else
	{
		std::cout << "About to close " << name << ", save? (Y/N)" << std::endl;
		std::string save_confirmation = "";
		while (save_confirmation != "y" && save_confirmation != "n")
		{
			save_confirmation = "";
			std::cin >> save_confirmation;
			if (save_confirmation.size() == 1)
				save_confirmation[0] = std::tolower(save_confirmation[0]);
		}
		if (save_confirmation == "y")
		{
			program.save_editor_settings();
			auto file = program.get_open_files()[name];
			file->write_to_file();
			program.print("saved " + name + " & closed file");
		}
		else
		{
			program.print("Closed " + name + " without saving");
		}

		if (name == program.get_current_file()->get_directory())
			program.get_current_file() = nullptr;

		program.get_open_files().erase(name);

		if (program.get_current_file() == nullptr && program.get_open_files().size() != 0)
		{
			auto iterator = program.get_open_files().end();
			iterator--;
			program.get_current_file() = iterator->second;
			program.print("Resumed editing " + program.get_current_file()->get_directory());
		}
	}
};

void close_program(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	if (program.get_open_files().size() > 0)
	{
		for (auto i = program.get_open_files().begin(); i != program.get_open_files().end(); ++i)
			close_file_by_directory(program, i->first);
	}
	
	program.set_open(false);
	program.save_editor_settings();
}

void close_file_command(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{

	if (tokens.size() == 1)
	{
		if (program.get_open_files().size() == 0)
		{
			program.set_open(false);
			return;
		}
		close_file_by_directory(program, program.get_current_file()->get_directory());
	}
	else if (tokens.size() == 2)
	{
		close_file_by_directory(program, tokens[1]);
	}
	else
	{
		program.print("Error: Invalid number of arguments");
	}
}

void size(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	if (tokens.size() > 2)
	{
		program.print("Error: Invalid number of arguments");
	}
	else
	{
		size_t current_size = program.get_current_file()->get_size();
		if (tokens.size() == 1) //With only one token, it means the user simply wants to print the number of bytes
		{
			program.print("file is " + std::to_string(current_size) + " bytes");
		}
		else if(tokens.size() == 2)//otherwise it means they want to resize the file
		{
			try
			{
				size_t new_size = std::stoi(tokens[1]);
				if (new_size > current_size)
				{
					program.get_current_file()->record_buffer(program);
					program.get_current_file()->append(program, "0", new_size - current_size);
				}
				else
				{
					program.get_current_file()->record_buffer(program);
					program.get_current_file()->shrink(current_size - new_size);
				}
			}
			catch (const std::exception& E)
			{
				program.print("Error");
			}
		}
		else //Too many arguments
		{
			program.print("Error");
		}
	}
}

void print_help(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	program.print("");
	program.get_commands().display_command_info(program);
	program.print("Numbers can be substituted for a range within brackets to choose randomly (i.e., 'set [0-255]')");
}

void hex_editor::command_registry::display_command_info(hex_editor::program_data& program)
{
	for (size_t i = 0; i < help.size(); i++)
	{
		auto& cmd = help[i];
		std::string header = cmd.command_name;

		for (size_t j = 0; j < cmd.aliases.size(); j++)
		{
			if (j == 0)
				header += " (";

			header += cmd.aliases[j];

			if (j == cmd.aliases.size() - 1)
				header += ")";
			else
				header += ", ";
		}
		program.print(header + ": " + cmd.instructions);
	}
}

void add_alias_command(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	int return_code = 3;
	if (tokens.size() == 3)
	{
		return_code = program.get_commands().register_alias(tokens[1], tokens[2]);
		program.save_editor_settings();
	}

	switch (return_code)
	{
		case 0: {program.print("Registered '" + tokens[2] + "' as an alias for '" + tokens[1] + "'"); break; }
		case 1: {program.print("Error: keyword '" + tokens[2] + "' already exists!"); break; }
		case 2: {program.print("Error: Failed to register alias '" + tokens[2] + "'"); break; }
		case 3: {program.print("Error: Invalid number of arguments"); break; }
	}
}

void delete_alias_command(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	int error_code = 3;
	if (tokens.size() == 2)
	{
		error_code = program.get_commands().delete_alias(tokens[1]);
		program.save_editor_settings();
	}

	switch (error_code)
	{
		case 0: {program.print("Deleted alias '" + tokens[1] + "'"); break; }
		case 1: {program.print("Error: No such alias '" + tokens[1] + "' exists!"); break; }
		case 2: {program.print("Error: Failed to delete alias '" + tokens[1] + "'"); break; }
		case 3: {program.print("Error: Invalid number of arguments"); break; }
	}
}

void reset_to_default_settings_command(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	if (tokens.size() != 1)
		program.print("Error: Invalid arguments");
	else
	{
		program.set_to_default_editor_settings();
		program.save_editor_settings();
	}
}

void new_file_command(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	if (tokens.size() != 2)
		program.print("Error: Invalid number of arguments");
	else
	{
		const std::string& filename = tokens[1];
		if (std::filesystem::exists(filename))
		{
			program.print("Error: " + filename + " already exists.");
		}
		else
		{
			try //If file has been created and is open somewhere, return an error
			{
				std::shared_ptr<hex_editor::file> existing_file = program.get_open_files().at(tokens[1]);
				program.print("Error: " + filename + " has already been created and opened");
			}
			catch (const std::exception& E) //If file is not currently open in memory
			{
				program.get_open_files().insert_or_assign(filename, std::make_shared<hex_editor::file>());
				auto newly_opened_file = program.get_open_files()[filename];
				program.get_current_file() = newly_opened_file;
				newly_opened_file->new_file(program, filename);
					program.get_current_file() = newly_opened_file;
				program.print("Created file " + filename);
			}
		}
	}
}

void repeat_command(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	//Just a dummy function so that it has a pointer associated with it, allowing it to work with the framework of commands and aliases
	//The actual logic for the 'repeat' command is resolved in hex_editor.cpp, because it interacts with the core mechanism for how commands are resolved and interpreted
}

std::string make_lowercase(const std::string& string)
{
	std::string copy = string;
	for (size_t i = 0; i < string.length(); ++i)
		copy[i] = std::tolower(string[i]);
	return copy;
}

void undo_command(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	int steps = -1;
	if (tokens.size() == 1)
	{
		steps = 1;
	}
	else if(tokens.size() == 2)
	{
		std::string arg = make_lowercase(tokens[1]);
		if (arg == "max" || arg == "all")
		{
			steps = 255;
		}
		else
		{
			try {
				steps = hex_editor::utility::convert_token_to_unsigned_char(arg, hex_editor::format_mode::decimal);
			}
			catch (const std::exception& E)
			{
				program.print("value error");
			}
		}
	}
	else
	{
		program.print("Parsing error");
	}

	for(int i = 0; i < steps && i <= program.get_history_size(); ++i)
		program.get_current_file()->undo();
}

void redo_command(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	int steps = -1;
	if (tokens.size() == 1)
	{
		steps = 1;
	}
	else if (tokens.size() == 2)
	{
		std::string arg = make_lowercase(tokens[1]);
		if (arg == "max" || arg == "all")
		{
			steps = 255;
		}
		else
		{
			try {
				steps = hex_editor::utility::convert_token_to_unsigned_char(arg, hex_editor::format_mode::decimal);
			}
			catch (const std::exception& E)
			{
				program.print("value error");
			}
		}	
	}
	else
	{
		program.print("Parsing error");
	}

	for (int i = 0; i < steps && i <= program.get_history_size(); ++i)
		program.get_current_file()->redo();
}

void zero_command(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	program.get_current_file()->record_buffer(program);
	if (tokens.size() == 1)
	{
		for (size_t i = 0; i < program.get_current_file()->get_size(); ++i)
		{
			program.get_current_file()->change_byte(program, i, "0000");
		}
	}
	else
	{
		program.print("Invalid arguments");
	}
}

void scramble_command(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	program.get_current_file()->record_buffer(program);
	size_t bytecount = program.get_current_file()->get_size();
	for (size_t i = 0; i < bytecount; ++i)
	{
		std::string rand_int = "00" + std::to_string(hex_editor::utility::get_random_int(0, 255));
		program.get_current_file()->change_byte(program, i, rand_int);
	}
}

void history_command(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	if (tokens.size() == 1)
	{
		program.print("The undo command currently records up to " + std::to_string((unsigned)program.get_history_size()) + " steps.");
	}
	else if (tokens.size() == 2)
	{
		try
		{
			unsigned char new_history_size = hex_editor::utility::convert_token_to_unsigned_char(tokens[1], hex_editor::format_mode::decimal);
			program.set_history_size(new_history_size);
			program.print("Set undo history to " + std::to_string(new_history_size) + " steps");
			program.save_editor_settings();
		}
		catch (const std::exception& E)
		{
			program.print("value error");
		}
	}
	else
	{
		program.print("Parsing error");
	}
}

void refresh(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	//Screen automatically refreshes after every command, so no additional logic is necessary for this function
	std::cout << "refreshing..." << std::endl;
	/*Or at least that SHOULD be true, but since the 'repeat' command function is also empty, the compiler thinks it can cleverly optimize
	space by giving them the same memory address. Unfortunately this causes it to mistake this command for the 'repeat' command, so
	they have to be semantically different to prevent this glitch.
	This printed text isn't even visible unless they scroll up anyway, so it makes little difference.
	*/
}

void randomize_byte(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	int num_args = tokens.size() - 1;
	size_t max_byte = program.get_current_file()->get_size() - 1;
	switch (num_args)
	{
		case 0: //With no arguments, it randomizes the selected byte by setting its value anywhere from 0 to 255
		{
			program.get_current_file()->record_buffer(program);
			unsigned char rand_int = hex_editor::utility::get_random_int(0, 255);
			auto file = program.get_current_file();
			size_t byte_id = file->get_selected_byte();
			file->change_byte(byte_id, rand_int);
			break;
		}

		case 1: //With one argument it picks a random value from 0 to 255 and assigns it to the byte specified
		{
			program.get_current_file()->record_buffer(program);
			unsigned char rand_int = hex_editor::utility::get_random_int(0, 255);
			auto file = program.get_current_file();
			size_t byte_id = std::stoi(tokens[1]);

			if (byte_id > max_byte)
				byte_id = max_byte;

			file->change_byte(byte_id, rand_int);
			break;
		}

		case 2: //With two arguments it randomizes the currently selected byte to a value between the two extremes specified
		{
			program.get_current_file()->record_buffer(program);
			int lower_bound = std::stoi(tokens[1]);
			int upper_bound = std::stoi(tokens[2]);

			if (lower_bound < 0)
				lower_bound = 0;

			if (upper_bound > 255)
				upper_bound = 255;

			unsigned char rand_int = hex_editor::utility::get_random_int(lower_bound, upper_bound);
			auto file = program.get_current_file();
			size_t byte_id = file->get_selected_byte();
			file->change_byte(byte_id, rand_int);
			break;
		}

		case 3: //With three arguments it uses the second and third arguments to bound the range, and the first argument is the byte it randomizes
		{

			int lower_bound = std::stoi(tokens[2]);
			int upper_bound = std::stoi(tokens[3]);

			if (lower_bound < 0)
				lower_bound = 0;

			if (upper_bound > 255)
				upper_bound = 255;

			unsigned char rand_int = hex_editor::utility::get_random_int(lower_bound, upper_bound);
			auto file = program.get_current_file();
			size_t byte_id = std::stoi(tokens[1]);
			if (byte_id > max_byte)
				byte_id = max_byte;
			program.get_current_file()->record_buffer(program);
			file->change_byte(byte_id, rand_int);
			break;
		}

		case 4: //With 4 arguments it uses the first two bytes to specify the range of randomized bytes, and the second two for the range of values
		{			
			auto file = program.get_current_file();
			size_t first_byte = static_cast<size_t>(std::stoi(tokens[1]));
			size_t last_byte = static_cast<size_t>(std::stoi(tokens[2]));
			int lower_bound = std::stoi(tokens[3]);
			int upper_bound = std::stoi(tokens[4]);

			if (last_byte > max_byte)
				last_byte = max_byte;

			if (first_byte < 0)
				first_byte = 0;

			if (lower_bound < 0)
				lower_bound = 0;

			if (upper_bound > 255)
				upper_bound = 255;

			program.get_current_file()->record_buffer(program);

			for (size_t byte_id = first_byte; byte_id <= last_byte; byte_id++)
			{
				std::cout << lower_bound << " / " << upper_bound << std::endl;
				unsigned char rand_int = hex_editor::utility::get_random_int(lower_bound, upper_bound);
				file->change_byte(byte_id, rand_int);
			}
			break;
		}

		default: {program.print("Invalid number of arguments"); break; }
	}
}

void start_search(hex_editor::program_data& program, const std::vector<std::string>& tokens, size_t Startingindex);

void search_from_start_command(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	start_search(program, tokens, 0);
}

void search_from_current_command(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	start_search(program, tokens, program.get_current_file()->get_selected_byte());
}

void delete_byte_command(hex_editor::program_data& program, const std::vector<std::string>& tokens)
{
	size_t first_byte, last_byte;
	if (tokens.size() == 1) //Delete selected byte
	{
		first_byte = last_byte = program.get_current_file()->get_selected_byte();
	}
	else if (tokens.size() == 2) //Delete specified byte
	{
		try
		{
			first_byte = last_byte = std::stoi(tokens[1]);
		}
		catch (const std::exception& E)
		{
			program.print("Error: Malformed entry.");
			return;
		}
	}
	else if (tokens.size() == 3) //Delete bytes in range
	{
		try
		{
			first_byte = std::stoi(tokens[1]);
			last_byte = std::stoi(tokens[2]);
		}
		catch (const std::exception& E)
		{
			program.print("Error: Malformed entry.");
			return;
		}
	}
	else
	{
		program.print("Error: Invalid number of arguments.");
		return;
	}

	if (first_byte > last_byte)
	{
		std::swap(first_byte, last_byte);
	}
	program.get_current_file()->record_buffer(program);
	program.get_current_file()->delete_bytes(first_byte, last_byte);
}












//////////////////////////////////////////////  UTILITIES  //////////////////////////////////////////////////////////

void start_search(hex_editor::program_data& program, const std::vector<std::string>& tokens, size_t Startingindex)
{
	hex_editor::byte current_byte_value;
	size_t pattern_length = tokens.size() - 1;
	size_t current_index, index_in_tokens;

	if (tokens.size() < 2)
	{
		program.print("No byte pattern indicated");
		return;
	}

	//first do error checking to make sure it's searching for a valid byte pattern
	for (size_t i = 1; i < tokens.size(); ++i)
	{
		const std::string& token = tokens[i];
		if (!hex_editor::utility::is_valid_binary(token) && !hex_editor::utility::is_valid_hex(token) && !hex_editor::utility::is_valid_dec(token))
		{
			program.print("Malformed byte pattern.");
			return;
		}
	}

	auto Matches = [&](uint8_t raw_bytevalue, const std::string& token) -> bool
	{
		hex_editor::format_mode current_format_mode = program.get_display_mode();
		hex_editor::format_mode found_formatting = hex_editor::utility::detect_formatting(token, current_format_mode);

		uint8_t found_value;
		if (found_formatting == hex_editor::format_mode::binary)
		{
			found_value = hex_editor::utility::binary_to_dec(token);
		}
		else if (found_formatting == hex_editor::format_mode::hexadecimal)
		{
			found_value = hex_editor::utility::hex_to_dec(token);
		}
		else if (found_formatting == hex_editor::format_mode::decimal)
		{
			found_value = std::stoi(token);
		}

		return found_value == raw_bytevalue;
	};

	/*
	Checks the whole buffer for the given byte pattern. It starts the iterator at the beginning, and moves it up until it finds
	a match for the first pattern byte. After it finds a match, it compares the next byte in both the pattern and file,
	and so on until it either finds a match or doesn't.
	*/
	for (current_index = Startingindex; (current_index + tokens.size() - 1) < program.get_current_file()->get_size(); ++current_index)
	{
		for (index_in_tokens = 1; index_in_tokens < tokens.size(); ++index_in_tokens)
		{
			//It iterates until it finds tokens that don't match, and if it does, it breaks.
			if (!Matches(program.get_current_file()->get_raw_byte_value(current_index), tokens[index_in_tokens]))
				break;
			else
				++current_index;
		}

		if (index_in_tokens >= tokens.size()) //If it searched all the way to the end, it means they matched the whole way
		{
			break;
		}
	}

	if (current_index + tokens.size() - 1 >= program.get_current_file()->get_size())
	{
		program.print("Did not find byte pattern in file.");
	}
	else
	{
		program.get_current_file()->record_buffer(program);
		program.get_current_file()->select_byte(current_index - tokens.size() + 1);
	}
}

bool hex_editor::command_registry::command_matches(const std::string& first_cmd, const std::string& second_cmd) const
{
	auto first = commands.find(first_cmd);
	auto second = commands.find(second_cmd);

	if (first == commands.end() || second == commands.end())
		return false;

	return first->second == second->second;
}

bool hex_editor::command_registry::command_matches(command_function_pointer_type func, const std::string& cmd) const
{
	auto first = commands.find(cmd);

	if (first == commands.end() || func == nullptr)
		return false;

	return first->second == func;
}

hex_editor::command_registry::command_function_pointer_type hex_editor::command_registry::get_command(const std::string& keyword)
{
	auto found = commands.find(keyword);
	if (found != commands.end())
		return found->second;

	return nullptr;
}

void hex_editor::command_registry::register_command(const std::string& command_word, hex_editor::command_registry::command_function_pointer_type address, const std::string& help)
{
	commands.emplace(command_word, address);
	hex_editor::command_registry::help.emplace_back(command_word, help);
}

int hex_editor::command_registry::register_alias(const std::string& original_command, const std::string& alias)
{
	//Don't allow duplicate aliases
	if (commands.find(alias) != commands.end())
		return 1;

	commands.emplace(alias, get_command(original_command)); //Add the alias to the list of commands, paired with the same function the original points to

	//Now it needs to register the alias with the "help" command
	command_help* cmd_help_data = nullptr; //This is a pointer referencing the help struct for the original command. It needs to find the help struct from the list

	//Finding the associated help struct
	for (size_t i = 0; i < help.size(); i++) //Go through list of help structs until it finds the one associated with the command this is an alias for, and save its address
	{
		if (help[i].command_name == original_command)
		{
			cmd_help_data = &help[i];
			break;
		}
	}

	//If the help struct for this command is found, add this to the list of aliases for the command it's an alias for
	if (cmd_help_data != nullptr)
	{
		cmd_help_data->aliases.push_back(alias);
		return 0;
	}
	else
	{
		return 2;
	}
}

int hex_editor::command_registry::delete_alias(const std::string& alias)
{
	bool deleted_alias = false;
	for (size_t i = 0; i < help.size(); i++)
	{
		command_help& associated_help_object = help[i];
		std::vector<std::string>& alias_list = associated_help_object.aliases;

		auto found_alias = std::find(alias_list.begin(), alias_list.end(), alias);
		if (found_alias != alias_list.end())
		{
			commands.erase(alias); //This actually MUST come first, cause aliasList.erase(...) clears the string itself for some odd reason, even though it's const and that's not appropriate behavior for that function...
			alias_list.erase(found_alias); //Oh, it's because the original string is ultimately a reference to the erased element, so by erasing the element, it's erasing it indirectly!
			deleted_alias = true;
			break;
		}
	}

	if (!deleted_alias)
		return 1;

	return 0;

}

void hex_editor::command_registry::save_aliases_to_file(std::ofstream& file)
{
	for (size_t i = 0; i < help.size(); i++)
	{
		command_help& help_object = help[i];
		std::string command = help_object.command_name;

		for (size_t j = 0; j < help_object.aliases.size(); j++)
		{
			auto write_string_to_file = [&](const std::string& string)
			{
				unsigned char null_terminator = 0;
				for (size_t k = 0; k < string.length(); k++)
				{
					char character = string[k];
					unsigned char raw_byte = static_cast<unsigned char>(character);
					file << raw_byte;
				}
				file << null_terminator;
			};

			std::string alias = help_object.aliases[j];
			
			write_string_to_file(command);
			write_string_to_file(alias);
		}
	}
}

void hex_editor::command_registry::delete_all_aliases()
{
	for (size_t i = 0; i < help.size(); i++)
	{
		command_help& help_object = help[i];
		while (help_object.aliases.size() > 0)
		{
			const std::string& alias = help_object.aliases[0];
			delete_alias(alias);
		}
	}
}

void hex_editor::command_registry::register_default_aliases()
{
	register_alias("hexadecimal", "hex");

	register_alias("decimal", "dec");

	register_alias("binary", "bin");
	register_alias("binary", "bi");

	register_alias("save", "write");

	register_alias("save_as", "saveas");

	register_alias("open", "edit");

	register_alias("quit", "exit");
	register_alias("quit", "kill");
	register_alias("quit", "q");

	register_alias("delete", "del");
	register_alias("delete", "dl");
	register_alias("delete", "remove");
	register_alias("delete", "rem");
	register_alias("delete", "rm");
	register_alias("delete", "rv");

	register_alias("find", "search");
	register_alias("find", "fn");
	register_alias("find", "fnd");
	register_alias("find", "f");

	register_alias("find_first", "ff");

	register_alias("select", "sl");
	register_alias("select", "s");
	register_alias("select", "goto");
	register_alias("select", "jp");

	register_alias("append", "cat");
	register_alias("append", "concat");
	register_alias("append", "concatanate");
	register_alias("append", "app");
	register_alias("append", "ap");
	register_alias("append", "a");

	register_alias("getsize", "get_size");

	register_alias("resize", "setsize");
	register_alias("resize", "set_size");

	register_alias("width", "w");

	register_alias("height", "h");
	register_alias("height", "length");
	register_alias("height", "lines");
	register_alias("height", "l");
	register_alias("height", "ln");

	register_alias("page", "p");
	register_alias("page", "pg");

	register_alias("right", "rt");
	register_alias("right", "next");
	register_alias("right", "n");

	register_alias("left", "last");

	register_alias("up", "u");

	register_alias("down", "d");

	register_alias("delete_alias", "dl_alias");

	register_alias("new", "new_file");

	register_alias("repeat", "r");

	register_alias("random", "rand");
	register_alias("random", "rnd");
	register_alias("random", "roll");
	register_alias("random", "rng");

	register_alias("refresh", "view");
	register_alias("refresh", "v");
}

bool hex_editor::command_registry::is_universal_command(command_function_pointer_type func) const
{
	return std::find(universal_commands.begin(), universal_commands.end(), func) != universal_commands.end();
}

bool hex_editor::command_registry::is_universal_command(const std::string& command_name) const
{
	auto foundcommand = commands.find(command_name);
	if (foundcommand != commands.end())
		return is_universal_command(foundcommand->second);
	else
		return false;
}

void hex_editor::command_registry::make_universal(command_function_pointer_type func)
{
	universal_commands.push_back(func);
}

void hex_editor::command_registry::make_universal(const std::string& command_name)
{
	auto foundcommand = commands.find(command_name);
	if (foundcommand != commands.end())
		universal_commands.push_back(foundcommand->second);
}

void hex_editor::command_registry::register_commands()
{
	register_command("help", &print_help, "Show instructions");
	register_command("hexadecimal", &hex_display, "Change display to hexadecimal");
	register_command("decimal", &dec_display, "Changes display to decimal");
	register_command("binary", &binary_display, "Changes display to binary");
	register_command("save", &save, "Saves the currently opened file or file specified");
	register_command("save_as", &save_as, "Save the open file under a different name");
	register_command("open", &open_file, "Open the specified file");
	register_command("new", &new_file_command, "Create a new file");
	register_command("close", &close_file_command, "Close current or specified file");
	register_command("quit", &close_program, "Closes the hex editor");
	register_command("delete", &delete_byte_command, "Delete selected or specified byte");
	register_command("select", &select_byte, "Select a specified byte or page.");
	register_command("append", &append_bytes, "Append specified byte to end of file a specified number of times");
	register_command("getsize", &print_size, "Print the size of the open file");
	register_command("random", &randomize_byte, "Randomize value of byte; [first byte] [last byte] [lower bound] [upper bound]");
	register_command("set", &set_byte, "Set the value of the selected or specified byte");
	register_command("size", &size, "Get or set the size of the current file");
	register_command("resize", &resize_file, "Resize the current file to the specified number of bytes");
	register_command("width", &set_page_width, "Set the width of the page in bytes");
	register_command("height", &set_page_length, "Set the height of the page in bytes");
	register_command("page", &go_to_page, "Go to specified page");
	register_command("right", &move_right, "Move selection as many bytes to the right as specified (no argument for just 1)");
	register_command("left", &move_left, "Move selection as many bytes to the left as specified (no argument for just 1)");
	register_command("up", &move_up, "Move selection as many bytes upward as specified (no argument for just 1)");
	register_command("down", &move_down, "Move selection as many bytes down as specified (no argument for just 1)");
	register_command("alias", &add_alias_command, "Add a new alias. first argument is original command, second argument is the new alias.");
	register_command("delete_alias", &delete_alias_command, "Delete an alias");
	register_command("reset", &reset_to_default_settings_command, "Reset editor settings to the default");
	register_command("refresh", &refresh, "Refresh screen");
	register_command("repeat", &repeat_command, "Repeat previous command");
	register_command("scramble", &scramble_command, "Scrambles the entire file");
	register_command("undo", &undo_command, "Undoes the last change to the file");
	register_command("redo", &redo_command, "Redoes an undone change");
	register_command("history", &history_command, "Sets how many changes can be undone");
	register_command("zero", &zero_command, "Set all bytes to 0");
	register_command("find", &search_from_current_command, "Find the next specified pattern of bytes in buffer");
	register_command("find_first", &search_from_start_command, "Find the first instance of the specified pattern of bytes");
	//register_command("delete")
	//register_command("savesettings", &saveSettingsComand, "save editor settings");

	make_universal(&print_help);
	make_universal(&open_file);
	make_universal(&new_file_command);
	make_universal(&reset_to_default_settings_command);
	make_universal(&add_alias_command);
	make_universal(&delete_alias_command);
	make_universal(&close_program);
	make_universal(&repeat_command);
	make_universal(&refresh);
	make_universal(&history_command);

	std::sort(help.begin(), help.end()); //Thank you STL
}