#include "hex_editor.h"
#include "byte.h"

#ifndef FILE_H
#define FILE_H

//A class to encapsulate the data pertaining to an open file & the operations you can perform with it. This is essentially the core of the editor.
class hex_editor::file
{
public:

	file();

	bool is_open();

	void open(hex_editor::program_data& program, const std::string& directory); //opens a file
	void new_file(hex_editor::program_data& program, const std::string& filename);
	void print_page(hex_editor::program_data& program); //prints out the current page

	void select_byte(size_t byte_to_select);

	size_t get_page_number(hex_editor::program_data& program);
	size_t get_number_of_pages(hex_editor::program_data& program);

	void append(hex_editor::program_data& program, const std::string& byte, size_t count);
	void change_byte(hex_editor::program_data& program, size_t byte_location, const std::string& new_value);
	void change_byte(size_t byte_location, unsigned char new_value);
	void delete_byte(size_t byte_location);

	size_t get_selected_byte(); //Get index of selected byte
	uint8_t get_raw_byte_value(size_t index_of_byte) const;
	int write_to_file(); //Commit changes to file

	const std::string& get_directory() const;
	void set_directory(const std::string& new_path);

	void shrink(size_t number_of_bytes_to_shrink_by);

	size_t get_size();

	void undo(); //Go to the next buffer denoting the "previous" step
	void redo();
	void record_buffer(program_data& program); //save a copy of the current buffer for undoing purposes. Also makes it current first, so if the user had undone anything before making a change, whatever they previously undid is now lost.
	void delete_bytes(size_t first_byte, size_t last_byte);
protected:

private:

	struct WorkspaceBuffer //Contains a list of bytes and a variable to track which byte is selected
	{
		std::vector<byte> bytes;
		size_t selected_byte = 0;
	};

	WorkspaceBuffer& get_current_buffer() const; //Gets the current workspace buffer
	
	void reset_buffers(); //Resets the buffers to nothing, essentially an initializer
	void make_buffer_current(); //Makes the selected workspace buffer current
	
	std::list<WorkspaceBuffer> buffers; //All the buffers of this file, which saves each change made.
	std::list<WorkspaceBuffer>::iterator current_buffer; //The buffer at index 0 is considered the most "present" buffer, while buffers higher up in the list are in the past. To undo, it just increments the current_buffer, and to redo it decrements.
	std::string directory;

	bool file_is_open = false;

};

#endif
