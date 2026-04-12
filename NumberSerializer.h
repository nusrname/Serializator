#pragma once
#include <cstdint>
#include <vector>
#include <stdexcept>
#include <cstring>

namespace NumberSerializer
{
	using byte = uint8_t;

	enum class DataType : uint8_t
	{
		Null  = 0x00,
		Bool  = 0x01,
		Int	  = 0x02,
		Float = 0x03
	};

	class ByteWriter
	{
	private:
		std::vector<byte> buffer;

	public:
		void clear();
		const std::vector<byte>& getBuffer() const;
		size_t size() const;

		// Write value
		void writeByte(byte value);
		void writeBytes(const byte* data, size_t count);
		void writeInt(int32_t value);
		void writeFloat(float value);
	};

	class ByteReader
	{
	private:
		const std::vector<byte>& data;
		size_t pos;

	public:
		ByteReader(const std::vector<byte>& input);

		bool hasMore(size_t count = 1) const;

		byte readByte();
		int32_t readInt();
		float readFloat();

		size_t getPosition() const;
	};

	class BoolSerializer
	{
	public:
		static void serialize(ByteWriter& writer, bool value);
		static bool deserialize(ByteReader& reader);
	};

	class IntSerializer
	{
	public:
		static void serialize(ByteWriter& writer, int32_t value);
		static int32_t deserialize(ByteReader& reader);
	};

	class FloatSerializer
	{
	public:
		static void serialize(ByteWriter& writer, float value);
		static float deserialize(ByteReader& reader);
	};
}