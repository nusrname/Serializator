#pragma once
#include <cstdint>
#include <vector>
#include <cstring>

namespace NumberSerializer
{
	typedef unsigned char byte;

	enum dataType
	{
		TYPE_BOOL = 0x01,
		TYPE_INT = 0x02,
		TYPE_FLOAT = 0x03,
		TYPE_NULL = 0x00
	};

	class NumberSerializer
	{
	protected:
		int length;
		std::vector<byte> buffer;

	public:
		NumberSerializer() : length(0) {}
		virtual ~NumberSerializer() {}

		virtual std::vector<byte> serialize() = 0;
		virtual void deserialize(const std::vector<byte>& data) = 0;

		int getLength() const { return length; }
		const std::vector<byte>& getBuffer() const { return buffer; }

	protected:
		void writeByte(byte value)
		{
			buffer.push_back(value);
			length++;
		}

		void writeInt(int value)
		{
			for (int i = 0; i < sizeof(int); i++)
			{
				buffer.push_back((value >> (i * 8)) & 0xFF);
				length++;
			}
		}

		void writeFloat(float value)
		{
			int intValue;
			memcpy(&intValue, &value, sizeof(float));
			writeInt(intValue);
		}

		byte readByte(const std::vector<byte>& data, int& pos)
		{
			if (pos < data.size())
				return data[pos++];
			return 0;
		}

		int readInt(const std::vector<byte>& data, int& pos)
		{
			int value = 0;
			for (int i = 0; i < sizeof(int); i++)
			{
				if (pos < data.size())
					value |= (data[pos++] << (i * 8));
			}

			return value;
		}

		float readFloat(const std::vector<byte>& data, int& pos)
		{
			int intValue = readInt(data, pos);
			float value;
			memcpy(&value, &intValue, sizeof(float));
			return value;
		}
	};

	class BoolSerializer : public NumberSerializer
	{
	private:
		byte data;

	protected:


	public:
		BoolSerializer(bool value = false) : data(value ? 1 : 0) {}
		BoolSerializer(const byte* rawData)
		{
			if (rawData)
				data = *rawData;
			else 
				data = 0;
		}

		~BoolSerializer() {}

		std::vector<byte> serialize() override
		{

		}

		void deserialize(const std::vector<byte>& data) override
		{

		}
	};

	class IntSerializer : NumberSerializer
	{
		int data;
	};

	class FloatSerializer : NumberSerializer
	{
		int mantissa, number;


	};
}