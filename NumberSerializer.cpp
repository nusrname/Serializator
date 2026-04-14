#pragma once
#include "NumberSerializer.h"
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <string.h>
#include <fstream>
#include <string>


namespace NumberSerializer 
{
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

	ByteReader::ByteReader(const std::vector<byte>& input) : data(input), pos(0) {}

	bool ByteReader::hasMore(size_t count) const
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

	void IntSerializer::serialize(ByteWriter& writer, int32_t value)
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

#pragma region Functions


	uint32_t crc32(const uint8_t* data, size_t length)
	{
		static uint32_t table[256];
		static bool initialized = false;

		if (!initialized)
		{
			for (uint32_t i = 0; i < 256; i++)
			{
				uint32_t crc = i;
				for (int j = 0; j < 8; j++)
					crc = (crc >> 1) ^ (0xEDB88320 & (-(int)(crc & 1)));

				table[i] = crc;
			}
			initialized = true;
		}

		uint32_t crc = 0xFFFFFFFF;

		for (size_t i = 0; i < length; i++)
			crc = (crc >> 8) ^ table[(crc ^ data[i]) & 0xFF];

		return ~crc;
	}

	std::vector<byte> compressData(const std::vector<byte>& input)
	{
		std::vector<byte> output;
		output.reserve(input.size());

		size_t i = 0;
		while (i < input.size())
		{
			size_t run = 1;

			// ищем повтор
			while (i + run < input.size() && input[i] == input[i + run] && run < 255)
				run++;

			if (run >= 4) // выгодно сжимать
			{
				output.push_back(0x01);
				output.push_back(static_cast<byte>(run));
				output.push_back(input[i]);
				i += run;
			}
			else
			{
				size_t start = i;
				size_t count = 0;

				while (i < input.size() && count < 255)
				{
					// если начинается повтор — остановиться
					if (i + 3 < input.size() &&
						input[i] == input[i + 1] &&
						input[i] == input[i + 2] &&
						input[i] == input[i + 3])
						break;

					i++;
					count++;
				}

				output.push_back(0x00);
				output.push_back(static_cast<byte>(count));
				output.insert(output.end(), input.begin() + start, input.begin() + start + count);
			}
		}

		return output;
	}

	std::vector<byte> decompressData(const std::vector<byte>& input, size_t expectedSize)
	{
		std::vector<byte> output;
		output.reserve(expectedSize);

		size_t i = 0;

		while (i < input.size())
		{
			byte type = input[i++];
			byte length = input[i++];

			if (type == 0x00)
			{
				if (i + length > input.size())
					throw std::runtime_error("Corrupt RLE data");

				output.insert(output.end(), input.begin() + i, input.begin() + i + length);
				i += length;
			}
			else if (type == 0x01)
			{
				byte value = input[i++];

				for (int j = 0; j < length; j++)
					output.push_back(value);
			}
			else
			{
				throw std::runtime_error("Invalid RLE block");
			}
		}

		if (output.size() != expectedSize)
			throw std::runtime_error("Decompression size mismatch");

		return output;
	}

	static const uint32_t MAGIC = 0x4E534552; // "NSER"
	static const uint8_t VERSION = 1;

	void SaveToFile(const std::vector<byte>& data, const std::string& filename, bool useCompression)
	{
		std::vector<byte> payload;

		uint8_t flags = 0;

		if (useCompression)
		{
			payload = compressData(data);
			if (payload.size() < data.size())
				flags |= 0x01;
			else
				payload = data;
		}
		else
			payload = data;

		uint32_t crc = crc32(data.data(), data.size());

		std::ofstream file(filename, std::ios::binary);
		if (!file)
			throw std::runtime_error("File open failed");

		uint32_t magic = MAGIC;
		uint8_t version = VERSION;
		uint32_t originalSize = static_cast<uint32_t>(data.size());
		uint32_t payloadSize = static_cast<uint32_t>(payload.size());

		file.write((char*)&magic, 4);
		file.write((char*)&version, 1);
		file.write((char*)&flags, 1);
		file.write((char*)&originalSize, 4);
		file.write((char*)&payloadSize, 4);
		file.write((char*)&crc, 4);

		file.write((char*)payload.data(), payload.size());

		if (!file.good())
			throw std::runtime_error("Write failed");
	}

	std::vector<byte> LoadFromFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::binary);
		if (!file)
			throw std::runtime_error("File open failed");

		uint32_t magic;
		uint8_t version;
		uint8_t flags;
		uint32_t originalSize;
		uint32_t payloadSize;
		uint32_t storedCrc;

		file.read((char*)&magic, 4);
		file.read((char*)&version, 1);
		file.read((char*)&flags, 1);
		file.read((char*)&originalSize, 4);
		file.read((char*)&payloadSize, 4);
		file.read((char*)&storedCrc, 4);

		if (!file.good())
			throw std::runtime_error("Header read failed");

		if (magic != MAGIC)
			throw std::runtime_error("Invalid format");

		if (version != VERSION)
			throw std::runtime_error("Unsupported version");

		if (payloadSize > 100 * 1024 * 1024)
			throw std::runtime_error("File too large");

		std::vector<byte> payload(payloadSize);
		file.read((char*)payload.data(), payloadSize);

		if (!file.good())
			throw std::runtime_error("Payload read failed");

		std::vector<byte> data;

		if (flags & 0x01)
			data = decompressData(payload, originalSize);
		else
			data = payload;

		if (data.size() != originalSize)
			throw std::runtime_error("Size mismatch");

		uint32_t actualCrc = crc32(data.data(), data.size());

		if (actualCrc != storedCrc)
			throw std::runtime_error("CRC mismatch");

		return data;
	}

#pragma endregion
}