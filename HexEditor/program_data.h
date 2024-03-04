#include "hex_editor.h"
#include "file.h"
#include "tokenizer.h"
#include "command_registry.h"

#ifndef PROGRAM_DATA_H
#define PROGRAM_DATA_H

//Meant to be used as a singleton.
struct hex_editor::program_data
{
	std::string input;
	std::vector<std::string> tokens;
	std::vector<std::string> messages;

	std::shared_ptr<hex_editor::file>& get_current_file();
	std::map<std::string, std::shared_ptr<file>>& get_open_files();
	hex_editor::tokenizer& get_tokenizer();
	hex_editor::command_registry& get_commands();

	void set_open(bool value);
	hex_editor::format_mode get_display_mode() const;
	unsigned char get_bytes_per_line() const;
	unsigned char get_lines_per_page() const;
	unsigned int get_bytes_per_page() const;
	//unsigned char get_lines_per_page() const;
	unsigned char get_history_size() const;
	bool is_open() const;

	void set_bytes_per_line(unsigned char bytes);
	void set_lines_per_page(unsigned char lines);
	void set_history_size(unsigned char length);

	void print(const std::string& Message);

	void set_display_mode(hex_editor::format_mode Mode);

	void print_messages();

	void set_to_default_editor_settings();
	void save_editor_settings();
	void load_editor_settings();

	const std::string& get_previous_input() const;
	void remember_previous_input();

private:

	bool open = true;
	hex_editor::tokenizer tokenizer;
	std::shared_ptr<hex_editor::file> current_file;
	hex_editor::command_registry commands;
	std::map<std::string, std::shared_ptr<file>> open_files;

	unsigned char bytes_per_line = 10;
	unsigned char lines_per_page = 20;
	unsigned char undo_history_size = 10;

	hex_editor::format_mode display_mode{ hex_editor::format_mode::hexadecimal };

	std::string previous_input;

};

#endif