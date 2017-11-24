#include "test_case.h"

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>

struct argument
{
	std::string uuid;
	std::string key;
	std::string value;
	int type;
};

struct cdata
{
	std::string uuid;
	std::string cmd;
	int flag;
	bool has_output;
	std::vector<argument> output;
};



struct ty {
	std::string key;
	std::string value;
	int flag;
	bool _test;
};

void cdata_to_json(cdata* pdata, json_coder& coder)
{
	jsutValue data(rapidjson::kObjectType);

	coder.add_member(data, "uuid", pdata->uuid);
	coder.add_member(data, "cmd", pdata->cmd);
	coder.add_member(data, "flag", pdata->flag);
	coder.add_member(data, "has_output", pdata->has_output);

	coder.add_array(data, "output", pdata->output,
		[](json_coder* coder, jsutValue& json, argument value)
	{
		json.SetObject();
		coder->add_member(json, "uuid", value.uuid);
		coder->add_member(json, "key", value.key);
		coder->add_member(json, "value", value.value);
		coder->add_member(json, "type", value.type);
	});

	coder.add_member("data", data);
}



void o_json(json_coder* coder, jsutValue& json, /*const*/ ty& t)
{
	json.SetObject();

	coder->add_member(json, "key_ref", t.key);

	coder->add_member(json, "value_ref", t.value);

	coder->add_member(json, "flag_ref", t.flag);

	coder->add_member(json, "test", t._test);

	char addr[24] = { 0 };
	sprintf_s(addr, "0x%p", &t);
	coder->add_member(json, "%p", &t);

	char* p = "char*";
	coder->add_member(json, "char*", p);
}


template <>
struct json_traits<std::pair<std::string, std::string>>
{
	static inline void to_json(json_coder* coder, jsutValue& json,
		const std::pair<std::string, std::string>& value) {
		json.SetObject();
		coder->add_member(json, value.first, value.second);
	}
};


void json_coder_test(jsutDocument& doc)
{
	json_coder code(doc);

	// add member test
	code.add_member("uuid", "dgsa12s1fa454ga");
	code.add_member("int", 3);
	code.add_member("bool", true);
	code.add_member("float", 25.64);
	std::string str = "obj";
	code.add_member(str, str);
	code.add_member("add const json value", jsutValue("const json value"));
	jsutValue va("json value");
	code.add_member("add json value", va);


	rapidjson::Value application(rapidjson::kObjectType);
	code.add_member(application, "mod", 1);
	code.add_member("application", application);


	// add array test
	std::vector<std::string> vec = {
		"report_graph.dll",
		"report_common.dll",
		"report_table.dll"
	};
	code.add_array("gallery_module", vec);
	code.add_array<std::string, std::string >("backstage_module",
	{
		"report_backstage.dll" ,
		"report_backstage_1.dll"
	});


	std::vector<std::pair<std::string, std::string>> vec_1;
	vec_1.push_back({ "key1","value1" });
	vec_1.push_back({ "key2","value2" });
	code.add_array("pair", vec_1);


	code.add_array<std::string, std::pair<std::string, std::string>>("pair_2",
	{
		{ "key1","value1" },
		{ "key2","value2" }
	});

	std::vector<ty> vec_3;
	for (int i = 0; i < 10; ++i)
		vec_3.push_back({ "123", "456", i, i % 2 == 0 });
	code.add_array("ty", vec_3, o_json);


	// push back test
	rapidjson::Value gallery_module(rapidjson::kArrayType);
	code.push_back(gallery_module, "");
	code.push_back(gallery_module, "report_graph.dll");
	code.push_back(gallery_module, ("report_common.dll"));
	code.push_back(gallery_module, ("report_table.dll"));
	code.push_back(gallery_module, ("report_picture.dll"));
	code.push_back(gallery_module, ("report_text.dll"));
	code.push_back(gallery_module, 125.3);
	code.push_back(gallery_module, true);
	code.push_back(gallery_module, rapidjson::Value("report_headerfooter.dll"));
	jsutValue vi("push_back ref test");
	code.push_back(gallery_module, vi);
	code.add_member("g1", gallery_module);

	cdata data;
	data.uuid = "e0529b10-5e1c-40a3-a3ab-15727cc2516a";
	data.cmd = "python.exe";
	data.flag = 0x01;
	data.has_output = true;

	int i = 10;
	while (i-- > 0)
	{
		data.output.push_back(
		{
			"e0529b10-5e1c-40a3-a3ab-15727cc2516a",
			"filename",
			"test_case.h",
			i
		});
	}

	cdata_to_json(&data, code);
}


void json_stream_test(jsutDocument& doc)
{
	json_stream stream(doc);
	stream.pretty();

	std::string str;
	stream >> str;
	stream.save("doc.json");
	std::cout << str;

	jsutDocument doc_copy;
	json_stream stream_cp(doc_copy);
	stream_cp << str;
	stream_cp.save("doc_copy.json", true);
}

