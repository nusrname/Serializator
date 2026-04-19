#pragma once
#include <unordered_map>
#include <vector>
#include "NumberSerializer.h"

namespace Serializer
{
	template<typename T>
	using SerializeFunc = void(*)(ByteWriter&, T);

	template<typename T>
	using DeserializeFunc = T(*)(ByteReader&);

	class ArraySerializer
	{
	public:
		template<typename T>
		static void serialize(ByteWriter& writer,
			const std::vector<T>& data,
			SerializeFunc<T> serializer);

		template<typename T>
		static std::vector<T> deserialize(ByteReader& reader,
			DeserializeFunc<T> deserializer);
	};

	class ListSerializer
	{
	public:
		template<typename T>
		static void serialize(ByteWriter& writer,
			const std::list<T>& data,
			SerializeFunc<T> serializer);

		template<typename T>
		static std::list<T> deserialize(ByteReader& reader,
			DeserializeFunc<T> deserializer);
	};

	class DictionarySerializer
	{
	public:
		template<typename K, typename V>
		static void serialize(ByteWriter& writer,
			const std::unordered_map<K, V>& dict,
			SerializeFunc<K> keySerializer,
			SerializeFunc<V> valueSerializer);

		template<typename K, typename V>
		static std::unordered_map<K, V> deserialize(ByteReader& reader,
			DeserializeFunc<K> keyDeserializer,
			DeserializeFunc<V> valueDeserializer);
	};
}

#include "CollectionSerializer.cpp"