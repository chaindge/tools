#include <iostream>
#include <fstream>
#include <string>
#include <vector>

void read_file(const std::string& path, std::vector<std::string>& keydords) 
{
	std::ifstream file(path);
	if (file)
	{
		while (true)
		{
			std::string kw;

			if (file >> kw)
				keydords.push_back(kw);
			else
				break;
		}
	}

	file.close();
}

void parse(std::vector<std::string>& keywords, const std::string& head_append, const std::string& tail_append)
{
	std::vector<std::string> uniqe_kws;

	for (auto& kw : keywords) {
		if (std::find(uniqe_kws.begin(), uniqe_kws.end(), kw) == uniqe_kws.end())
			uniqe_kws.push_back(kw);
	}

	keywords.clear();

	for (auto& kw : uniqe_kws) {
		keywords.push_back(head_append + kw + tail_append);
	}
}

void print(std::vector<std::string>& keywords, const std::string& divide)
{
	for (auto& keyword : keywords) {
		std::cout << keyword << divide;
	}
}

int main(int argc, char** argv)
{
	if (argc < 4)
	{
		std::cout << "argument too less\n";
		return 0;
	}

	std::string path = argv[1];
	std::string head = argv[2];
	std::string tail = argv[3];
	std::string divide = "\n";

	if (argc == 5)
		divide = argv[4];

	std::vector<std::string> keywords;

	read_file(path, keywords);
	parse(keywords, head, tail);
	print(keywords, divide);

	return 0;
}
