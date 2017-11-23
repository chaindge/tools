#include "jsonut.h"

bool json_stream::save(jsutDocument& doc, const char* path, bool pretty /*= false*/) {

	jsutStringBuffer sb;
	pretty ? doc.Accept(jsutWriterX(sb)) : doc.Accept(jsutWriter(sb));

	// save
	bool ret = true;
	std::ofstream out(path);
	if (out)
		out << sb.GetString();
	if (!out.good() || out.fail())
		ret = false;
	out.close();
	return ret;
}

bool json_stream::read(jsutDocument& doc, const char* filename) {

	// Open stream
	std::ifstream stream(filename, std::ios::binary);
	if (!stream)
		return false;
	stream.unsetf(std::ios::skipws);

	// Determine stream size
	stream.seekg(0, std::ios::end);
	size_t size = stream.tellg();
	stream.seekg(0);

	// Load data and add terminating 0
	std::vector<char> data;
	data.resize(size + 1);
	stream.read(&data.front(), static_cast<std::streamsize>(size));
	data[size] = 0;

	doc.Parse<0>(&data.front());

	return true;
}
