#include "StringSerializer.h"
#include "NumberSerializer.h"
#include <stdexcept>
#include <cstdint>
#include <string>

namespace Serializer
{
	void CharSerializer::serialize(ByteWriter& writer, char value)
	{
		writer.writeByte(static_cast<byte>(DataType::Char));
		writer.writeByte(static_cast<byte>(value));
	}

	char CharSerializer::deserialize(ByteReader& reader)
	{
		auto type = static_cast<DataType>(reader.readByte());

		if (type != DataType::Char)
			throw std::runtime_error("Type mismatch: expected Char");

		return static_cast<char>(reader.readByte());
	}

	void StringSerializer::serialize(ByteWriter& writer, const std::string& value)
	{
		writer.writeByte(static_cast<byte>(DataType::String));

		uint32_t size = static_cast<uint32_t>(value.size());
		writer.writeInt(size);

		if (size > 0)
			writer.writeBytes(reinterpret_cast<const byte*>(value.data()), size);
	}

	std::string StringSerializer::deserialize(ByteReader& reader)
	{
		auto type = static_cast<DataType>(reader.readByte());

		if (type != DataType::String)
			throw std::runtime_error("Type mismatch: expected String");

		uint32_t size = static_cast<uint32_t>(reader.readInt());

		if (!reader.hasMore(size))
			throw std::runtime_error("Corrupted string size");

		const byte* ptr = reader.readBytes(size);

		return std::string(reinterpret_cast<const char*>(ptr), size);
	}
}