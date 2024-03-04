#include "program_data.h"

std::shared_ptr<hex_editor::file>& hex_editor::program_data::get_current_file()
{
	return current_file;
}

std::map<std::string, std::shared_ptr<hex_editor::file>>& hex_editor::program_data::get_open_files()
{
	return open_files;
}

hex_editor::tokenizer& hex_editor::program_data::get_tokenizer()
{
	return tokenizer;
}

hex_editor::command_registry& hex_editor::program_data::get_commands()
{
	return commands;
}

void hex_editor::program_data::set_open(bool value)
{
	open = value;
}

hex_editor::format_mode hex_editor::program_data::get_display_mode() const
{
	return display_mode;
}

unsigned char hex_editor::program_data::get_bytes_per_line() const
{
	return bytes_per_line;
}

unsigned char hex_editor::program_data::get_lines_per_page() const
{
	return lines_per_page;
}

void hex_editor::program_data::set_bytes_per_line(unsigned char bytes)
{
	bytes_per_line = bytes;
}

void hex_editor::program_data::set_lines_per_page(unsigned char lines)
{
	lines_per_page = lines;
}

void hex_editor::program_data::set_history_size(unsigned char length)
{
	undo_history_size = length;
}

void hex_editor::program_data::print(const std::string& Message)
{
	messages.push_back(Message);
}

void hex_editor::program_data::set_display_mode(hex_editor::format_mode Mode)
{
	display_mode = Mode;
}

unsigned int hex_editor::program_data::get_bytes_per_page() const
{
	return get_bytes_per_line() * get_bytes_per_line();
}

unsigned char hex_editor::program_data::get_history_size() const
{
	return undo_history_size;
}

bool hex_editor::program_data::is_open() const
{
	return open;
}

void hex_editor::program_data::print_messages()
{
	for (size_t i = 0; i < messages.size(); i++)
		std::cout << messages[i] << std::endl;

	messages.clear();
}

const std::string& hex_editor::program_data::get_previous_input() const
{
	return previous_input;
}

void hex_editor::program_data::remember_previous_input()
{
	previous_input = input;
}

void hex_editor::program_data::set_to_default_editor_settings()
{
	std::cout << "Unable to load settings file, using defaults instead." << std::endl;
	commands.delete_all_aliases();
	commands.register_default_aliases();
	set_bytes_per_line(10);
	set_lines_per_page(20);
	set_history_size(10);
	set_display_mode(hex_editor::format_mode::hexadecimal);
}

void hex_editor::program_data::load_editor_settings()
{
	try
	{
		std::ifstream editor_settings;
		editor_settings.open("EditorSettings", std::ios::binary);
		char loaded_value = 0;
		if (editor_settings.is_open() && editor_settings.good())
		{
			editor_settings.get(loaded_value);
			//std::cout << "Retrieved " << loaded_value << " (" << static_cast<unsigned>(loaded_value) << ") from editor_settings\n";
			set_bytes_per_line(static_cast<unsigned>(loaded_value));

			editor_settings.get(loaded_value); //Next load lines per page
			//std::cout << "Retrieved " << loaded_value << " (" << static_cast<unsigned>(loaded_value) << ") from editor_settings\n";
			set_lines_per_page(static_cast<unsigned>(loaded_value));

			editor_settings.get(loaded_value);
			//std::cout << "Retrieved " << loaded_value << " (" << static_cast<unsigned>(loaded_value) << ") from editor_settings\n";
			
			switch (static_cast<unsigned>(loaded_value))
			{
				case 0: {set_display_mode(hex_editor::format_mode::decimal); break; }
				case 1: {set_display_mode(hex_editor::format_mode::hexadecimal); break; }
				case 2: {set_display_mode(hex_editor::format_mode::binary); break; }
				default: {set_display_mode(hex_editor::format_mode::hexadecimal); break; }
			}

			if (get_bytes_per_line() == 0)
				set_bytes_per_line(1);

			if (get_bytes_per_line() == 0)
				set_lines_per_page(1);

			editor_settings.get(loaded_value);
			set_history_size(loaded_value);

			while (editor_settings.good())
			{
				std::string command;
				std::string alias;

				auto load_string_from_file = [&](std::string& destination)
				{
					char loaded_char; //Vessel for characters loaded from file
					editor_settings.get(loaded_char); //Load the first character

					while (static_cast<unsigned>(loaded_char) != 0 && editor_settings.good()) //Keep loading characters until it finds a null terminator
					{
						destination += loaded_char;
						editor_settings.get(loaded_char);
					}
				};

				load_string_from_file(command);
				load_string_from_file(alias);

				commands.register_alias(command, alias);
				
			}

			editor_settings.close();
		}
		else
		{
			std::exception E;
			throw E;
		}
	}
	catch (const std::exception& E)
	{
		//If it can't load settings it sets the settings to their default
		set_to_default_editor_settings();
	}
}

void hex_editor::program_data::save_editor_settings()
{
	std::ofstream editor_settings;
	editor_settings.open("EditorSettings", std::ios::binary);
	if (editor_settings.is_open())
	{
		unsigned char display_modevalue = 0;
		switch (get_display_mode())
		{
			case hex_editor::format_mode::decimal: {display_modevalue = 0; break; }
			case hex_editor::format_mode::hexadecimal: {display_modevalue = 1; break; }
			case hex_editor::format_mode::binary: {display_modevalue = 2; break; }
		}
		editor_settings.put(bytes_per_line);
		editor_settings.put(lines_per_page);
		editor_settings.put(display_modevalue);
		editor_settings.put(undo_history_size);

		commands.save_aliases_to_file(editor_settings);

		editor_settings.close();
	}
}