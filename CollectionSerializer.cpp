#pragma once
#include <unordered_map>
#include "CollectionSerializer.h"
#include "NumberSerializer.h"
#include <vector>

namespace CollectionSerializer
{
	using namespace NumberSerializer;

	template<typename T>
	void ArraySerializer::serialize(ByteWriter& writer,
		const std::vector<T>& data,
		SerializeFunc<T> serializer)
	{
		writer.writeByte(static_cast<byte>(DataType::Array));

		uint32_t size = static_cast<uint32_t>(data.size());
		writer.writeInt(size);

		for (const auto& item : data)
			serializer(writer, item);
	}

	template<typename T>
	std::vector<T> ArraySerializer::deserialize(ByteReader& reader,
		DeserializeFunc<T> deserializer)
	{
		auto type = static_cast<DataType>(reader.readByte());

		if (type != DataType::Array)
			throw std::runtime_error("Type mismatch: expected Array");

		uint32_t size = static_cast<uint32_t>(reader.readInt());

		std::vector<T> result;
		result.reserve(size);

		for (uint32_t i = 0; i < size; i++)
			result.push_back(deserializer(reader));

		return result;
	}

	template<typename T>
	void ListSerializer::serialize(ByteWriter& writer,
		const std::vector<T>& data,
		SerializeFunc<T> serializer)
	{
		writer.writeByte(static_cast<byte>(DataType::List));

		uint32_t size = static_cast<uint32_t>(data.size());
		writer.writeInt(size);

		for (const auto& item : data)
			serializer(writer, item);
	}

	template<typename T>
	std::vector<T> ListSerializer::deserialize(ByteReader& reader,
		DeserializeFunc<T> deserializer)
	{
		auto type = static_cast<DataType>(reader.readByte());

		if (type != DataType::List)
			throw std::runtime_error("Type mismatch: expected List");

		uint32_t size = static_cast<uint32_t>(reader.readInt());

		std::vector<T> result;
		result.reserve(size);

		for (uint32_t i = 0; i < size; i++)
			result.push_back(deserializer(reader));

		return result;
	}

	template<typename K, typename V>
	void DictionarySerializer::serialize(ByteWriter& writer,
		const std::unordered_map<K, V>& dict,
		SerializeFunc<K> keySerializer,
		SerializeFunc<V> valueSerializer)
	{
		writer.writeByte(static_cast<byte>(DataType::Dictionary));

		uint32_t size = static_cast<uint32_t>(dict.size());
		writer.writeInt(size);

		for (const auto& pair : dict)
		{
			keySerializer(writer, pair.first);
			valueSerializer(writer, pair.second);
		}
	}

	template<typename K, typename V>
	std::unordered_map<K, V> DictionarySerializer::deserialize(ByteReader& reader,
		DeserializeFunc<K> keyDeserializer,
		DeserializeFunc<V> valueDeserializer)
	{
		auto type = static_cast<DataType>(reader.readByte());

		if (type != DataType::Dictionary)
			throw std::runtime_error("Type mismatch: expected Dictionary");

		uint32_t size = static_cast<uint32_t>(reader.readInt());

		std::unordered_map<K, V> result;
		result.reserve(size);

		for (uint32_t i = 0; i < size; i++)
		{
			K key = keyDeserializer(reader);
			V value = valueDeserializer(reader);

			result.emplace(key, value);
		}

		return result;
	}
}