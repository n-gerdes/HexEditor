#include "file.h"
#include "program_data.h"
#include "utility.h"

hex_editor::file::file()
{

}

hex_editor::file::WorkspaceBuffer& hex_editor::file::get_current_buffer() const
{
	return *current_buffer;
}

void hex_editor::file::record_buffer(hex_editor::program_data& program)
{
	make_buffer_current(); //If you got to the current page state by undoing a lot, this essentially unrecords the reversed commands
	buffers.emplace_front(get_current_buffer()); //Push back a copy of the current buffer
	current_buffer = buffers.begin(); //Now that the buffer has been copied, point it forward to the copy while preserving the original

	if (buffers.size() > program.get_history_size()+1) //If it records too many steps it starts deleting the extras, to preserve memory
		buffers.pop_back();
}

void hex_editor::file::delete_bytes(size_t first_byte, size_t last_byte)
{
	std::vector<byte>& bytes = current_buffer->bytes;
	if (last_byte >= bytes.size())
		last_byte = bytes.size() - 1;
	if (first_byte >= bytes.size())
		first_byte = bytes.size() - 1;

	std::vector<byte> new_bytes;
	new_bytes.reserve(bytes.size() - (last_byte - first_byte + 1));
	for (size_t i = 0; i < bytes.size(); ++i)
	{
		if (i < first_byte || i > last_byte)
			new_bytes.push_back(bytes[i]);
	}
	bytes = new_bytes;
	if (get_selected_byte() > bytes.size())
		select_byte(bytes.size() - 1);
}

void hex_editor::file::make_buffer_current()
{
	while (buffers.begin() != current_buffer)
		buffers.pop_front();
}

void hex_editor::file::reset_buffers()
{
	buffers.clear();
	buffers.emplace_back();
	current_buffer = buffers.begin();
}

void hex_editor::file::undo()
{
	if (&(get_current_buffer()) != &buffers.back()) //If the current buffer is not the last, move up one buffer
		++current_buffer;
}

void hex_editor::file::redo()
{
	if (current_buffer != buffers.begin()) //If there's room to go back, do so.
		--current_buffer;
}

void hex_editor::file::new_file(hex_editor::program_data& program, const std::string& filename)
{
	reset_buffers();
	//get_current_buffer().bytes.clear();
	directory = filename;
	file_is_open = true;
}

void hex_editor::file::open(hex_editor::program_data& program, const std::string& directory)
{
	std::ifstream file_stream; //Create file stream to read file
	file_stream.open(directory, std::ios::binary); //Try to open said file
	file::directory = directory; //Remember which directory was opened
	if (file_stream.fail())
	{
		program.print("Error opening file");
		return;
	}

	char byte_from_file; //Create a receptacle for bytes to be read from the file to
	reset_buffers();

	//get_current_buffer().bytes.clear(); //Clear the current array of bytes
	while (file_stream.good()) //Read from the file until it reaches the end or it encounters an error
	{
		file_stream.get(byte_from_file);
		get_current_buffer().bytes.emplace_back(static_cast<unsigned>(byte_from_file)); //This works because it can implicitly convert from unsigned char to hex_editor::byte thanks to the constructor for it
	}
	get_current_buffer().bytes.pop_back(); //I think it's reading the last byte in twice for some reason, so I'm popping the copy out here.
	
	file_stream.close(); //Remember to relinquish control of the file to the operating system for now
	file_is_open = true;
	program.print("opened file " + directory);
}

bool hex_editor::file::is_open()
{
	return file_is_open;
}

void hex_editor::file::select_byte(size_t byte_to_select)
{
	get_current_buffer().selected_byte = byte_to_select;
}

void hex_editor::file::print_page(hex_editor::program_data& program)
{
	if (!is_open()) return; //Can't print anything if there's no open file yet

	std::cout << "file: " << get_directory() << std::endl;
	const static std::string BYTE_SEPARATOR = "  ";
	const static std::string SELECTED_BYTE_SEPARATOR_LEFT = " [";
	const static std::string SELECTED_BYTE_SEPARATOR_RIGHT = "] ";

	std::cout << std::endl << std::endl; //Essentially carve out more room to display a new page

	if (get_selected_byte() >= get_current_buffer().bytes.size())
		select_byte(get_current_buffer().bytes.size() - 1);

	const size_t bytes_per_page = program.get_bytes_per_page(); //This is how many bytes are displayed per "page"
	const size_t page_number = get_selected_byte() / bytes_per_page; //This determines which page it should display, based on which byte is selected and how many bytes there are per page

	const size_t number_of_pages = get_number_of_pages(program);
	
	size_t starting_display_byte = page_number * bytes_per_page; //The ID of the first byte displayed on this page

	auto display_page_number = [&]()
	{
		if (get_current_buffer().bytes.size() == 0)
			std::cout << "Page 0 / 0" << std::endl;
		else
			std::cout << std::endl << "Page " << page_number+1 << " / " << number_of_pages << std::endl;
	};

	if (get_current_buffer().bytes.size() != 0)
	{
		//first it'll print the header
		const size_t first_byteOnPage = page_number * bytes_per_page;
		const size_t bytesLeftInfile = get_current_buffer().bytes.size() - first_byteOnPage;
		const int total_precursor_chars = std::to_string(get_size()).size() + 1;
		const int headerspaces = total_precursor_chars + 1;
		for (size_t headerspace = 0; headerspace < headerspaces; headerspace++)
			std::cout << " ";

		for (unsigned int header = 0; header < program.get_bytes_per_line() && header < bytesLeftInfile; header++)
		{
			std::string headerText = std::to_string(header); //The numeral above this column
			switch (program.get_display_mode())
			{
			case hex_editor::format_mode::hexadecimal:
			{
				if (headerText.size() == 1)
				{
					headerText = " " + headerText;
				}
				break;
			}

			case hex_editor::format_mode::decimal: //Ensure the correct size for each column header
			{
				if (headerText.size() == 1)
				{
					headerText = "  " + headerText;
				}
				else if (headerText.size() == 2)
				{
					headerText = " " + headerText;
				}
				break;
			}

			case hex_editor::format_mode::binary:
			{
				headerText = "    " + headerText + "  ";
				if (header <= 9)
				{
					headerText += " ";
				}
				break;
			}
			}

			std::cout << BYTE_SEPARATOR << headerText << BYTE_SEPARATOR;
		}

		//Then it'll print the page
		std::cout << std::endl << std::endl;
		for (unsigned int line = 0; line < program.get_lines_per_page(); line++) //Relative display position on this page
		{
			size_t first_byte_of_line = starting_display_byte + (program.get_bytes_per_line() * line);
			if (first_byte_of_line >= get_current_buffer().bytes.size())
			{
				display_page_number();
				return;
			}

			std::string line_display = std::to_string((program.get_bytes_per_line() * line) + starting_display_byte); //This is essentially the line number, or the location of the first byte on this line
			const int spaces = total_precursor_chars - line_display.size();
			for (size_t space = 0; space < spaces; space++)
				std::cout << " ";

			std::cout << line_display;
			std::cout << "  ";
			for (unsigned int byte_row = 0; byte_row < program.get_bytes_per_line(); byte_row++)
			{
				size_t actual_byte = starting_display_byte + (program.get_bytes_per_line() * line) + byte_row; //Actual location of the byte being displayed

				if (actual_byte >= get_current_buffer().bytes.size()) //Make sure it can't try to display bytes past the end of a file where they don't exist
				{
					display_page_number();
					return;
				}
				std::string display_value;
				if (program.get_display_mode() == hex_editor::format_mode::hexadecimal)
				{
					display_value = get_current_buffer().bytes[actual_byte].get_value_hex();
				}
				else if (program.get_display_mode() == hex_editor::format_mode::decimal)
				{
					display_value = get_current_buffer().bytes[actual_byte].get_value_dec();
				}
				else if (program.get_display_mode() == hex_editor::format_mode::binary)
				{
					display_value = get_current_buffer().bytes[actual_byte].get_value_binary();
				}

				if (actual_byte == get_selected_byte())
				{
					std::cout << SELECTED_BYTE_SEPARATOR_LEFT << display_value << SELECTED_BYTE_SEPARATOR_RIGHT;
				}
				else
				{
					std::cout << BYTE_SEPARATOR << display_value << BYTE_SEPARATOR;
				}
			}
			std::cout << std::endl;
		}
	}
	else
	{
		std::cout << "[file is empty]" << std::endl << std::endl << std::endl;
	}
	display_page_number();
}

void hex_editor::file::append(hex_editor::program_data& program, const std::string& byte, size_t count)
{
	if (!is_open()) return; //Can't append anything to an unopened file

	for (size_t i = 0; i < count; i++)
		get_current_buffer().bytes.emplace_back(byte, program);
}

void hex_editor::file::delete_byte(size_t byte_location)
{
	if (!is_open()) return;

	if (byte_location < get_current_buffer().bytes.size())
	{
		std::vector<byte> copy{ get_current_buffer().bytes };
		get_current_buffer().bytes.clear();

		for (size_t i = 0; i < byte_location; i++)
			get_current_buffer().bytes.push_back(copy[i]);
		
		for (size_t i = byte_location + 1; i < copy.size(); i++)
			get_current_buffer().bytes.push_back(copy[i]);

		if (get_current_buffer().bytes.size() < 1)
			get_current_buffer().bytes.emplace_back(0);

		if (get_selected_byte() >= get_current_buffer().bytes.size())
			select_byte(get_current_buffer().bytes.size() - 1);
	}
}

void hex_editor::file::shrink(size_t bytes_to_remove)
{
	if (!is_open()) return;
	for (size_t i = 0; i < bytes_to_remove; i++)
		get_current_buffer().bytes.pop_back();
}

size_t hex_editor::file::get_selected_byte()
{
	return get_current_buffer().selected_byte;
}

uint8_t hex_editor::file::get_raw_byte_value(size_t byteindex) const
{
	return get_current_buffer().bytes[byteindex].get_value();
}

void hex_editor::file::change_byte(hex_editor::program_data& program, size_t byte_id, const std::string& entered_value)
{
	if (!is_open()) return;
	get_current_buffer().bytes[byte_id].set_value(entered_value, program);
}

void hex_editor::file::change_byte(size_t byte_id, unsigned char entered_value)
{
	if (!is_open()) return;
	get_current_buffer().bytes[byte_id].set_value(entered_value);
}

int hex_editor::file::write_to_file()
{
	if (!is_open())
	{
		return 2;
	}

	std::ofstream file_stream;
	file_stream.open(directory, std::ios::binary);
	try
	{
		if (file_stream.is_open())
		{
			for (size_t i = 0; i < get_current_buffer().bytes.size(); i++)
			{
				unsigned char bytevalue = get_current_buffer().bytes[i].get_value();
				//file_stream << bytevalue;
				file_stream.put(bytevalue);
			}
			file_stream.close();
			return 0;
		}
		else
		{
			return 1;
		}
	}
	catch (const std::exception& E)
	{
		return 1;
	}
}

void hex_editor::file::set_directory(const std::string& new_path)
{
	directory = new_path;
}

const std::string& hex_editor::file::get_directory() const
{
	return directory;
}

size_t hex_editor::file::get_size()
{
	return get_current_buffer().bytes.size();
}

size_t hex_editor::file::get_number_of_pages(hex_editor::program_data& program)
{
	size_t bytes_per_page = program.get_bytes_per_page();
	size_t number_of_pages = get_current_buffer().bytes.size() / bytes_per_page;
	if (number_of_pages * bytes_per_page < get_current_buffer().bytes.size()) //A fraction of a page still counts as a whole page for page-counting purposes
		number_of_pages++;

	return number_of_pages;
}

size_t hex_editor::file::get_page_number(hex_editor::program_data& program)
{
	return 1 + (get_selected_byte() / program.get_bytes_per_page());
}