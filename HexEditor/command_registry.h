#include "hex_editor.h"

#ifndef COMMAND_REGISTRY_H
#define COMMAND_REGISTRY_H

class hex_editor::command_registry
{

public:

	typedef void(*command_function_pointer_type)(hex_editor::program_data&, const std::vector<std::string>& tokens);

	void register_commands();

	void register_command(const std::string& command_word, command_function_pointer_type address, const std::string& help);
	int register_alias(const std::string& original_command, const std::string& alias);
	int delete_alias(const std::string& alias);

	command_function_pointer_type get_command(const std::string& keyword);

	void display_command_info(hex_editor::program_data& program);

	void save_aliases_to_file(std::ofstream& file);

	void register_default_aliases();
	void delete_all_aliases();

	bool is_universal_command(const std::string& command) const; //Check if a command is allowed to execute at any time, even if no file is open
	bool is_universal_command(command_function_pointer_type func) const;

	void make_universal(command_function_pointer_type func);
	void make_universal(const std::string& command_name);

	bool command_matches(const std::string& first_cmd, const std::string& second_cmd) const; //Equivalence check to see if an alias or command matches the other
	bool command_matches(command_function_pointer_type func, const std::string& cmd) const;

private:

	struct command_help
	{
		std::string command_name;
		std::string instructions;
		std::vector<std::string> aliases;

		command_help(const std::string& name, const std::string& instructions) : command_name(name), instructions(instructions) {};

		bool operator<(const command_help& other) const //Necessary for std::sort to operate on command_help objects
		{
			return command_name < other.command_name; //Thankfully alphebetizing is already handled by std::string
		}
	};

	std::map<std::string, command_function_pointer_type> commands; //Stores all commands, including originals and aliases
	std::vector<command_function_pointer_type> universal_commands; //A list of commands that can be executed at any time, even if a file is not open
	std::vector<command_help> help; //Stores commands along with their help text and a list of their aliases
	/*The difference between aliasesand regular commands is in how they're registered. aliases are registered to other commands,
	and the memory address associated with them is set to whatever the master command is. Instead of displaying the function of
	each alias in the help screen, aliases are merely listed after the main command name
	*/
};

#endif
