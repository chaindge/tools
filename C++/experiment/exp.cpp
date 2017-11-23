#include "type_checker.h"
#include "test_case.h"

int main(int argc, const char* argv[])
{
	type_checker();


	jsutDocument doc;
	doc.SetObject();

	json_coder_test(doc);
	json_stream_test(doc);

	return 0;
}

