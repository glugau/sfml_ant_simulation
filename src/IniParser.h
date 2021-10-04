#pragma once

#include <string>
#include <fstream>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <sstream>

class IniParser
{
private:
	class Section
	{
	public:
		std::unordered_map<std::string, std::string> values;
		std::string& operator[](const std::string& key)
		{
			return values[key];
		}
	};
	std::unordered_map<std::string, Section*> section_map;
	std::string target_file;

public:
	IniParser(const std::string& file)
		: target_file(file)
	{
		std::ifstream file_stream;
		file_stream.open(file);

		std::vector<std::string> lines;

		{
			std::string line;
			while (std::getline(file_stream, line))
			{
				lines.push_back(line);
			}
				
		}

		std::string current_section_name = "";
		for (std::string& line : lines)
		{
			if (line[0] == '[')
			{
				std::string section_name = "";
				bool valid = false;
				for (int i = 1; i < line.size(); i++)
				{
					if (line[i] == ']')
					{
						valid = true;
						break;
					}
					section_name += line[i];
				}
				if (valid)
				{
					current_section_name = section_name;
				}
			}
			else
			{
				std::vector<std::string> pair;

				{
					std::stringstream line_stream(line);
					std::string segment;
					while (std::getline(line_stream, segment, '='))
						pair.push_back(segment);
				}
				
				if (pair.size() == 2)
				{
					if (section_map[current_section_name] == nullptr)
						section_map[current_section_name] = new Section;

					Section& sec = *section_map[current_section_name];
					sec[pair[0]] = pair[1];
				}
			}
		}

		file_stream.close();
	}
	~IniParser()
	{
		save();
		for (std::pair<std::string, Section*> a : section_map)
		{
			delete a.second;
		}
	}
	void save()
	{
		std::ofstream file_stream(target_file);
		for (std::pair<std::string, Section*> a : section_map)
		{
			file_stream << '[' << a.first << "]\n";
			for (std::pair<std::string, std::string> b : a.second->values)
			{
				file_stream << b.first << "=" << b.second << std::endl;
			}
		}
	}
	Section& operator[](const std::string& section)
	{
		if (section_map[section] == nullptr)
			section_map[section] = new Section;
		return *section_map[section];
	}
};