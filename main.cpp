#include <iostream>
#include <cassert>

#include "NumberSerializer.h"
#include "StringSerializer.h"
#include "CollectionSerializer.h"

int main()
{
	try
	{
		Serializer::ByteWriter writer;

		// =========================
		// 1. PRIMITIVES TEST
		// =========================
		Serializer::IntSerializer::serialize(writer, 42);
		Serializer::FloatSerializer::serialize(writer, 3.14f);
		Serializer::BoolSerializer::serialize(writer, true);
		Serializer::CharSerializer::serialize(writer, 'A');

		// =========================
		// 2. STRING TEST
		// =========================
		Serializer::StringSerializer::serialize(writer, "Hello Serialization");

		// =========================
		// 3. ARRAY TEST
		// =========================
		//std::vector<int32_t> arr = { 1, 2, 3, 4, 5 };

		//Serializer::ArraySerializer::serialize<int32_t>(
		//	writer,
		//	arr,
		//	Serializer::IntSerializer::serialize
		//);

		// =========================
		// 4. DICTIONARY TEST
		// =========================
		/*std::unordered_map<int32_t, int32_t> dict =
		{
			{1, 10},
			{2, 20},
			{3, 30}
		};

		Serializer::DictionarySerializer::serialize<int32_t, int32_t>(
			writer,
			dict,
			Serializer::IntSerializer::serialize,
			Serializer::IntSerializer::serialize
		);*/

		// =========================
		// SAVE FILE
		// =========================
		Serializer::SaveToFile(writer.getBuffer(), "save.dat");

		// =========================
		// LOAD FILE
		// =========================
		auto loadedData = Serializer::LoadFromFile("save.dat");
		Serializer::ByteReader reader(loadedData);

		// =========================
		// 5. DESERIALIZATION TEST
		// =========================

		int a = Serializer::IntSerializer::deserialize(reader);
		float b = Serializer::FloatSerializer::deserialize(reader);
		bool c = Serializer::BoolSerializer::deserialize(reader);
		char ch = Serializer::CharSerializer::deserialize(reader);

		std::string str = Serializer::StringSerializer::deserialize(reader);

		//auto arr2 = Serializer::ArraySerializer::deserialize<int32_t>(
		//	reader,
		//	Serializer::IntSerializer::deserialize
		//);

		//auto dict2 = Serializer::DictionarySerializer::deserialize<int32_t, int32_t>(
		//	reader,
		//	Serializer::IntSerializer::deserialize,
		//	Serializer::IntSerializer::deserialize
		//);

		// =========================
		// 6. ASSERTIONS
		// =========================

		assert(a == 42);
		assert(b > 3.13f && b < 3.15f);
		assert(c == true);
		assert(ch == 'A');

		assert(str == "Hello Serialization");

		//assert(arr2 == arr);

		//assert(dict2.size() == dict.size());
		//for (const auto& [k, v] : dict)
		//	assert(dict2.at(k) == v);

		// =========================
		// OUTPUT
		// =========================
		std::cout << "TEST PASSED\n";
		std::cout << "Values: " << a << " " << b << " " << c << " " << ch << "\n";
		std::cout << "String: " << str << "\n";
		//std::cout << "Array size: " << arr2.size() << "\n";
		//std::cout << "Dictionary size: " << dict2.size() << "\n";

		return 0;
	}
	catch (const std::exception& e)
	{
		std::cerr << "TEST FAILED: " << e.what() << "\n";
		return 1;
	}
}