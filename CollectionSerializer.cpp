#pragma once
#include "CollectionSerializer.h"
#include <stdexcept>

namespace Serializer
{
	template<typename T>
	inline void ArraySerializer::serialize(ByteWriter& writer,
		const std::vector<T>& data,
		SerializeFunc<T> serializer)
	{
		writer.writeByte(static_cast<byte>(DataType::Array));

		writer.writeInt(static_cast<int32_t>(data.size()));

		for (const auto& item : data)
			serializer(writer, item);
	}

	template<typename T>
	inline std::vector<T> ArraySerializer::deserialize(ByteReader& reader,
		DeserializeFunc<T> deserializer)
	{
		auto type = static_cast<DataType>(reader.readByte());

		if (type != DataType::Array)
			throw std::runtime_error("Type mismatch: Array");

		int32_t size = reader.readInt();

		std::vector<T> result;
		result.reserve(size);

		for (int i = 0; i < size; i++)
			result.push_back(deserializer(reader));

		return result;
	}

	template<typename T>
	inline void ListSerializer::serialize(ByteWriter& writer,
		const std::vector<T>& data,
		SerializeFunc<T> serializer)
	{
		writer.writeByte(static_cast<byte>(DataType::List));

		writer.writeInt(static_cast<int32_t>(data.size()));

		for (const auto& item : data)
			serializer(writer, item);
	}

	template<typename T>
	inline std::vector<T> ListSerializer::deserialize(ByteReader& reader,
		DeserializeFunc<T> deserializer)
	{
		auto type = static_cast<DataType>(reader.readByte());

		if (type != DataType::List)
			throw std::runtime_error("Type mismatch: List");

		int32_t size = reader.readInt();

		std::vector<T> result;
		result.reserve(size);

		for (int i = 0; i < size; i++)
			result.push_back(deserializer(reader));

		return result;
	}

	template<typename K, typename V>
	inline void DictionarySerializer::serialize(ByteWriter& writer,
		const std::unordered_map<K, V>& dict,
		void(*keySerializer)(ByteWriter&, const K&),
		void(*valueSerializer)(ByteWriter&, const V&))
	{
		writer.writeByte(static_cast<byte>(DataType::Dictionary));

		writer.writeInt(static_cast<int32_t>(dict.size()));

		for (const auto& [k, v] : dict)
		{
			keySerializer(writer, k);
			valueSerializer(writer, v);
		}
	}

	template<typename K, typename V>
	inline std::unordered_map<K, V> DictionarySerializer::deserialize(ByteReader& reader,
		V(*keyDeserializer)(ByteReader&),
		V(*valueDeserializer)(ByteReader&))
	{
		auto type = static_cast<DataType>(reader.readByte());

		if (type != DataType::Dictionary)
			throw std::runtime_error("Type mismatch: Dictionary");

		int32_t size = reader.readInt();

		std::unordered_map<K, V> result;
		result.reserve(size);

		for (int i = 0; i < size; i++)
		{
			K key = keyDeserializer(reader);
			V value = valueDeserializer(reader);

			result.emplace(key, value);
		}

		return result;
	}

}