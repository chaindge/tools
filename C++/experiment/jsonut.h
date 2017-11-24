#ifndef __JSONUT__
#define __JSONUT__

#include <vector>
#include <string>
#include <fstream>

#include "json/document.h"
#include "json/writer.h"
#include "json/prettywriter.h"

typedef rapidjson::Document		jsutDocument;
typedef rapidjson::Value		jsutValue;
typedef rapidjson::StringBuffer	jsutStringBuffer;
typedef rapidjson::Writer<jsutStringBuffer>	jsutWriter;
typedef rapidjson::PrettyWriter<jsutStringBuffer> jsutWriterX;
typedef rapidjson::MemoryPoolAllocator<> jsutAllocator;


/**
 *	util class for read from json file or write json to file
 */
class json_stream
{
public:
	static bool save(jsutDocument& doc, const char* path, bool pretty = false);
	static bool read(jsutDocument& doc, const char* filename);

public:
	json_stream(jsutDocument& doc)
		: _doc(doc)
		, _pretty(false) {}


	json_stream& operator<<(const std::string& json_str) {
		_doc.Parse<0>(json_str.c_str());
		return *this;
	}

	json_stream& operator<<(const char* json_str) {
		_doc.Parse<0>(json_str);
		return *this;
	}

	json_stream& operator >> (std::string& json_str) {
		jsutStringBuffer sb;
		_pretty ? _doc.Accept(jsutWriterX(sb)) : _doc.Accept(jsutWriter(sb));
		json_str = sb.GetString();
		return *this;
	}

	inline void pretty() {
		_pretty = true;
	}

	inline void tight() {
		_pretty = false;
	}

	inline bool save(const char* path, bool pretty = false) {
		return save(_doc, path, pretty);
	}

	inline bool read(const char* filename) {
		return read(_doc, filename);
	}
private:
	jsutDocument& _doc;
	bool _pretty;

};


/**
 *	util class for encode json quickly
 *	
 */
class json_coder
{
public:
	json_coder(jsutDocument& doc)
		: _doc(doc)
		, _alloc(doc.GetAllocator()) {}

	json_coder(jsutDocument& doc, jsutAllocator& alloc)
		: _doc(doc)
		, _alloc(alloc) {}


	inline jsutDocument& doc() {
		return _doc;
	}

	inline jsutAllocator& alloc() {
		return _alloc;
	}

	/**
	 *	add member wrapers
	 */
	template <class NameType, class ValueType>
	inline bool add_member(const NameType& name, ValueType value)
	{
		return add_member(_doc, name, value);
	}

	template <class NameType, class ValueType>
	bool add_member(jsutValue& json, const NameType& name, ValueType value)
	{
		assert(json.IsObject());
		if (!json.IsObject())
			return false;

		jsutValue js_name, js_value;
		to_json(js_name, name);
		to_json(js_value, value);

		json.AddMember(js_name, js_value, _alloc);

		return true;
	}

	template <class NameType>
	inline bool add_member(const NameType& name, jsutValue& value)
	{
		return add_member(_doc, name, value);
	}

	template <class NameType>
	bool add_member(jsutValue& json, const NameType& name, jsutValue& value)
	{
		assert(json.IsObject());
		if (!json.IsObject())
			return false;

		jsutValue js_name;
		to_json(js_name, name);

		json.AddMember(js_name, value, _alloc);

		return true;
	}


	/**
	*	push back wrapers
	*/
	template <class ValueType>
	bool push_back(jsutValue& json, ValueType value)
	{
		assert(json.IsArray());
		if (!json.IsArray())
			return false;

		jsutValue js_value;
		to_json(js_value, value);

		json.PushBack(js_value, _alloc);
		return true;
	}

	//template <>
	inline bool push_back(jsutValue& json, const jsutValue& value)
	{
		assert(json.IsArray());
		if (!json.IsArray())
			return false;

		json.PushBack(const_cast<jsutValue&>(value), _alloc);
		return true;
	}


	/**
	 *	templates for adding array to document quickly
	 */
	template<class NameType, class ValueType>
	inline bool add_array(const NameType& name, const std::vector<ValueType>& vec)
	{
		return add_array(_doc, name, vec);
	}

	template<class NameType, class ValueType>
	bool add_array(jsutValue& json, const NameType& name, const std::vector<ValueType>& vec)
	{
		assert(json.IsObject());
		if (!json.IsObject())
			return false;

		jsutValue js_array(rapidjson::kArrayType);
		for (auto& item : vec)
		{
			jsutValue js_item;
			to_json(js_item, const_cast<ValueType&>(item));
			js_array.PushBack(js_item, _alloc);
		}

		jsutValue js_name;
		to_json(js_name, name);
		json.AddMember(js_name, js_array, _alloc);

		return true;
	}

	// Converter: [](json_coder* coder, jsValue& json, ValueType value) { }
	template<class NameType, class ValueType, class Converter>
	bool add_array(const NameType& name, const std::vector<ValueType>& vec, Converter to_json)
	{
		return add_array(_doc, name, vec, to_json);
	}


	template<class NameType, class ValueType, class Converter>
	bool add_array(jsutValue& json, const NameType& name, const std::vector<ValueType>& vec, Converter convert)
	{
		assert(json.IsObject());
		if (!json.IsObject())
			return false;

		jsutValue js_array(rapidjson::kArrayType);
		for (auto& item : vec)
		{
			jsutValue js_item;
			convert(this, js_item, const_cast<ValueType&>(item));
			js_array.PushBack(js_item, _alloc);
		}

		jsutValue js_name;
		to_json(js_name, name);
		json.AddMember(js_name, js_array, _alloc);

		return true;
	}


	template<class T>
	inline void to_json(jsutValue& json, T value) {
		typedef typename std::remove_cv<T>::type value_type;
		json_traits<value_type>::to_json(this, json, value);
	}

private:
	jsutDocument& _doc;
	jsutAllocator& _alloc;

};

template <class T >
struct json_traits {
	static inline void to_json(json_coder* coder, jsutValue& json, T value) {
		json.Set(value);
	}
};

//template <class T>
//struct json_traits<T&> {
//	static inline void to_json(json_coder* coder, jsutValue& json, T& value) {
//		json.Set(value);
//	}
//};

template <class T>
struct json_traits<T*> {
	static inline void to_json(json_coder* coder, jsutValue& json, T* value) {
		json.SetUint64((uint64_t)value);
	}
};


template <>
struct json_traits<std::string> {
	static inline void to_json(json_coder* coder, jsutValue& json, const std::string& value) {
		json.Set(value.c_str(), coder->alloc());
	}
};

template <>
struct json_traits<char*> {
	static inline void to_json(json_coder* coder, jsutValue& json, char* value) {
		json.SetString(value, coder->alloc());
	}
};

template <>
struct json_traits<const char*> {
	static inline void to_json(json_coder* coder, jsutValue& json, const char* value) {
		json.SetString(value, coder->alloc());
	}
};


#endif //__JSONUT__
