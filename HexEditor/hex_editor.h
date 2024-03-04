// hex_editor.h : Include file for standard system include files,
// or project specific include files.

#ifndef HEX_EDITOR_H
#define HEX_EDITOR_H

#include <iostream>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <map>
#include <memory>
#include <list>

namespace hex_editor
{
	enum class format_mode : unsigned char { decimal, hexadecimal, binary };
	class byte;
	class file;
	class tokenizer;
	class command_registry;
	struct program_data; //Meant to be used as a singleton

}
#endif
