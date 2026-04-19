#include <iostream>
#include <cassert>
#include <vector>
#include <unordered_map>

#include "NumberSerializer.h"
#include "StringSerializer.h"
#include "CollectionSerializer.h"
#include <cstdint>
#include <exception>
#include <string>

static void PrimitivesTest(Serializer::ByteWriter& writer)
{
	Serializer::IntSerializer::serialize(writer, 42);
	Serializer::FloatSerializer::serialize(writer, 3.14f);
	Serializer::BoolSerializer::serialize(writer, true);
	Serializer::CharSerializer::serialize(writer, 'A');
}

static void StringTest(Serializer::ByteWriter& writer)
{
	Serializer::StringSerializer::serialize(writer, "Hello Serialization");
}

static void ArrayTest(Serializer::ByteWriter& writer)
{
	std::vector<int32_t> arr = { 1, 2, 3, 4, 5 };

	Serializer::ArraySerializer::serialize<int32_t>(
		writer,
		arr,
		static_cast<Serializer::SerializeFunc<int32_t>>(Serializer::IntSerializer::serialize)
	);
}

static void DictionaryTest(Serializer::ByteWriter& writer)
{
	std::unordered_map<int32_t, int32_t> dict =
	{
		{1, 10},
		{2, 20},
		{3, 30}
	};

	Serializer::DictionarySerializer::serialize<int32_t, int32_t>(
		writer,
		dict,
		static_cast<Serializer::SerializeFunc<int32_t>>(Serializer::IntSerializer::serialize),
		static_cast<Serializer::SerializeFunc<int32_t>>(Serializer::IntSerializer::serialize)
	);
}

static void ListTest(Serializer::ByteWriter& writer)
{
	std::vector<int32_t> list = { 10, 20, 30, 40 };

	Serializer::ListSerializer::serialize<int32_t>(
		writer,
		list,
		static_cast<Serializer::SerializeFunc<int32_t>>(Serializer::IntSerializer::serialize)
	);
}

int main()
{
	try
	{
		Serializer::ByteWriter writer;

		PrimitivesTest(writer);
		StringTest(writer);
		ArrayTest(writer);
		DictionaryTest(writer);
		ListTest(writer);
		std::vector<int32_t> list = { 10, 20, 30, 40 };
		std::vector<int32_t> arr = { 1, 2, 3, 4, 5 };
		std::unordered_map<int32_t, int32_t> dict =
		{
			{1, 10},
			{2, 20},
			{3, 30}
		};

		Serializer::SaveToFile(writer.getBuffer(), "save.dat");
		auto loadedData = Serializer::LoadFromFile("save.dat");
		Serializer::ByteReader reader(loadedData);

		// 5. DESERIALIZATION TEST

		int a = Serializer::IntSerializer::deserialize(reader);
		float b = Serializer::FloatSerializer::deserialize(reader);
		bool c = Serializer::BoolSerializer::deserialize(reader);
		char ch = Serializer::CharSerializer::deserialize(reader);

		std::string str = Serializer::StringSerializer::deserialize(reader);

		auto arr2 = Serializer::ArraySerializer::deserialize<int32_t>(
			reader,
			Serializer::IntSerializer::deserialize
		);

		auto dict2 = Serializer::DictionarySerializer::deserialize<int32_t, int32_t>(
			reader,
			Serializer::IntSerializer::deserialize,
			Serializer::IntSerializer::deserialize
		);

		auto list2 = Serializer::ListSerializer::deserialize<int32_t>(
			reader,
			Serializer::IntSerializer::deserialize
		);

		// 6. ASSERTIONS
		assert(a == 42);
		assert(b > 3.13f && b < 3.15f);
		assert(c == true);
		assert(ch == 'A');

		assert(str == "Hello Serialization");

		assert(arr2 == arr);
		assert(list2 == list);

		assert(dict2.size() == dict.size());
		for (const auto& [k, v] : dict)
			assert(dict2.at(k) == v);

		// OUTPUT
		std::cout << "TEST PASSED\n";
		std::cout << "Values: " << a << " " << b << " " << c << " " << ch << "\n";
		std::cout << "String: " << str << "\n";
		std::cout << "Array size: " << arr2.size() << "\n";
		std::cout << "Dictionary size: " << dict2.size() << "\n";

		return 0;
	}
	catch (const std::exception& e)
	{
		std::cerr << "TEST FAILED: " << e.what() << "\n";
		return 1;
	}
}