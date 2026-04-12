#pragma once
#include "NumberSerializer.h"
using namespace NumberSerializer;


#pragma region ByteWriter

void ByteWriter::clear()
{
	buffer.clear();
}

const std::vector<byte>& ByteWriter::getBuffer() const
{
	return buffer;
}

size_t ByteWriter::size() const
{
	return buffer.size();
}

// Write value

void ByteWriter::writeByte(byte value)
{
	buffer.push_back(value);
}

void ByteWriter::writeBytes(const byte* data, size_t count)
{
	buffer.insert(buffer.end(), data, data + count);
}

void ByteWriter::writeInt(int32_t value)
{
	for (byte i = 0; i < 4; i++)
		buffer.push_back(static_cast<byte>((value >> (i * 8)) & 0xFF));
}

void ByteWriter::writeFloat(float value)
{
	uint32_t bits;
	std::memcpy(&bits, &value, sizeof(float));
	writeInt(static_cast<int32_t>(bits));
}

#pragma endregion

#pragma region ByteReader

ByteReader::ByteReader(const std::vector<byte>& input) : data(input), pos(0) { }

bool ByteReader::hasMore(size_t count = 1) const
{
	return pos + count <= data.size();
}

byte ByteReader::readByte()
{
	if (!hasMore(1))
		throw std::runtime_error("Out of bounds read (byte)");

	return data[pos++];
}

int32_t ByteReader::readInt()
{
	if (!hasMore(4))
		throw std::runtime_error("Out of bounds read (int)");

	int32_t value = 0;

	for (int i = 0; i < 4; i++)
	{
		value |= (static_cast<int32_t>(data[pos++]) << (i * 8));
	}

	return value;
}

float ByteReader::readFloat()
{
	uint32_t bits = static_cast<uint32_t>(readInt());

	float value;
	std::memcpy(&value, &bits, sizeof(float));

	return value;
}

size_t ByteReader::getPosition() const
{
	return pos;
}

#pragma endregion

#pragma region BoolSerializer

void BoolSerializer::serialize(ByteWriter& writer, bool value)
{
	writer.writeByte(static_cast<byte>(DataType::Bool));
	writer.writeByte(value ? 1 : 0);
}

bool BoolSerializer::deserialize(ByteReader& reader)
{
	auto type = static_cast<DataType>(reader.readByte());

	if (type != DataType::Bool)
		throw std::runtime_error("Type mismatch: expected Bool");

	return reader.readByte() != 0;
}

#pragma endregion

#pragma region IntSerializer

void IntSerializer::serialize(ByteWriter & writer, int32_t value)
{
	writer.writeByte(static_cast<byte>(DataType::Int));
	writer.writeInt(value);
}

int32_t IntSerializer::deserialize(ByteReader& reader)
{
	auto type = static_cast<DataType>(reader.readByte());

	if (type != DataType::Int)
		throw std::runtime_error("Type mismatch: expected Int");

	return reader.readInt();
}

#pragma endregion

#pragma region FloatSerializer

void FloatSerializer::serialize(ByteWriter& writer, float value)
{
	writer.writeByte(static_cast<byte>(DataType::Float));
	writer.writeFloat(value);
}

float FloatSerializer::deserialize(ByteReader& reader)
{
	auto type = static_cast<DataType>(reader.readByte());

	if (type != DataType::Float)
		throw std::runtime_error("Type mismatch: expected Float");

	return reader.readFloat();
}

#pragma endregion
