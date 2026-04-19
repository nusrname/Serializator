#pragma once
#include "CollectionSerializer.h"
#include <stdexcept>
#include <algorithm>
#include <cstdint>
#include <unordered_map>
#include <utility>
#include <vector>
#include "NumberSerializer.h"

namespace Serializer
{
	template<typename T>
	void ArraySerializer::serialize(ByteWriter& writer,
		const std::vector<T>& data,
		SerializeFunc<T> serializer)
	{
		writer.writeByte(static_cast<byte>(DataType::Array));

		writer.writeInt(static_cast<int32_t>(data.size()));

		for (const auto& item : data)
			serializer(writer, item);
	}

	template<typename T>
	std::vector<T> ArraySerializer::deserialize(ByteReader& reader,
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
	void ListSerializer::serialize(ByteWriter& writer,
		const std::list<T>& data,
		SerializeFunc<T> serializer)
	{
		writer.writeByte(static_cast<byte>(DataType::List));

		writer.writeInt(static_cast<int32_t>(data.size()));

		for (const auto& item : data)
			serializer(writer, item);
	}

	template<typename T>
	std::list<T> ListSerializer::deserialize(ByteReader& reader,
		DeserializeFunc<T> deserializer)
	{
		auto type = static_cast<DataType>(reader.readByte());

		if (type != DataType::List)
			throw std::runtime_error("Type mismatch: List");

		int32_t size = reader.readInt();

		if (size < 0)
			throw std::runtime_error("Corrupted List size");

		std::list<T> result;

		for (int i = 0; i < size; i++)
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
		writer.writeInt(static_cast<int32_t>(dict.size()));

		for (const auto& [k, v] : dict)
		{
			keySerializer(writer, k);
			valueSerializer(writer, v);
		}
	}

	template<typename K, typename V>
	std::unordered_map<K, V> DictionarySerializer::deserialize(ByteReader& reader,
		DeserializeFunc<K> keyDeserializer,
		DeserializeFunc<V> valueDeserializer)
	{
		const auto type = static_cast<DataType>(reader.readByte());
		if (type != DataType::Dictionary)
			throw std::runtime_error("Type mismatch: expected Dictionary");

		const int32_t size = reader.readInt();
		if (size < 0)
			throw std::runtime_error("Corrupted Dictionary size");

		std::unordered_map<K, V> result;
		result.reserve(static_cast<size_t>(size));

		for (int32_t i = 0; i < size; ++i)
		{
			K key = keyDeserializer(reader);
			V value = valueDeserializer(reader);
			result.emplace(std::move(key), std::move(value));
		}

		return result;
	}
}