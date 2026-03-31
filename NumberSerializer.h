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
		void clear()
		{
			buffer.clear();
		}

		const std::vector<byte>& getBuffer() const
		{
			return buffer;
		}

		size_t size() const
		{
			return buffer.size();
		}

		void writeByte(byte value)
		{
			buffer.push_back(value);
		}

		void writeBytes(const byte* data, size_t count)
		{
			buffer.insert(buffer.end(), data, data + count);
		}

		void writeInt(int32_t value)
		{
			for (byte i = 0; i < 4; i++)
				buffer.push_back(static_cast<byte>((value >> (i * 8)) & 0xFF));
		}

		void writeFloat(float value)
		{
			uint32_t bits;
			std::memcpy(&bits, &value, sizeof(float));
			writeInt(static_cast<int32_t>(bits));
		}
	};

	class ByteReader
	{
	private:
		const std::vector<byte>& data;
		size_t pos;
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

		void writeInt(int32_t value)
		{
			for (byte i = 0; i < 4; i++)
				buffer.push_back(static_cast<byte>((value >> (i * 8)) & 0xFF));
		}

		void writeFloat(float value)
		{
			uint32_t bits;
			memcpy(&bits, &value, sizeof(float));
			writeInt(bits);
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
					value |= (static_cast<int>(data[pos++]) << (i * 8));
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
			buffer.clear();
			length = 0;

			//writeByte(Bool);
			writeByte(data);

			return buffer;
		}

		void deserialize(const std::vector<byte>& data) override
		{
			int pos = 0;

			byte type = readByte(data, pos);
			/*if (type != TYPE_BOOL)
				return;*/

			this->data = readByte(data, pos);
		}

		bool getValue() const { return data != 0; }
		void setValue(bool value) { data = value ? 1 : 0; }
	};

	class IntSerializer : public NumberSerializer
	{
	private:
		int data;

	public:
		IntSerializer(int value = 0) : data(value) {}
		IntSerializer(const byte* rawData)
		{
			if (rawData)
				memcpy(&data, rawData, sizeof(int));
			else
				data = 0;
		}

		std::vector<byte> serialize() override
		{
			buffer.clear();
			length = 0;

			//writeByte(TYPE_INT);
			writeInt(data);

			return buffer;
		}

		void deserialize(const std::vector<byte>& data) override
		{
			int pos = 0;

			byte type = readByte(data, pos);
			/*if (type != TYPE_INT)
				return;*/

			this->data = readInt(data, pos);
		}

		int getValue() const { return data; }
		void setValue(int value) { data = value; }
	};

	//class FloatSerializer : public NumberSerializer
	//{
	//private:
	//	int mantissa, exponent;
	//	float value;

	//public:
	//	FloatSerializer(float val = 0.0f) :value(val)
	//	{
	//		union { float f; int i; } converter;

	//		converter.f = val;

	//		exponent = ((converter.i >> 23) & 0xFF) - 127;
	//		mantissa = converter.i & 0x7FFFFF;
	//	}
	//	FloatSerializer(int mant, int exp) : mantissa(mant), exponent(exp) 
	//	{
	//		union { float f; int i; } converter;

	//		converter.i = (mantissa & 0x7FFFFF) | ((exponent + 127) << 23);
	//		if (mantissa < 0)
	//			converter.i |= 0x8000000;

	//		value = converter.f;
	//	}

	//	std::vector<byte> serialize() override
	//	{
	//		buffer.clear();
	//		length = 0;

	//		writeByte(TYPE_FLOAT);
	//		writeInt(mantissa);
	//		writeInt(exponent);

	//		return buffer;
	//	}

	//	void deserialize(const std::vector<byte>& data) override
	//	{
	//		int pos = 0;

	//		byte type = readByte(data, pos);
	//		if (type != TYPE_FLOAT)
	//			return;

	//		mantissa = readInt(data, pos);
	//		exponent = readInt(data, pos);

	//		union { float f; int i; } converter;

	//		converter.i = (mantissa & 0x7FFFFF) | ((exponent + 127) << 23);
	//		if (mantissa < 0)
	//			converter.i |= 0x8000000;

	//		value = converter.f;
	//	}

	//	float getValue() const { return value; }
	//	int getMantissa() const { return mantissa; }
	//	int getExponent() const { return exponent; }

	//	void setValue(float val)
	//	{
	//		value = val;

	//		union { float f; int i; } converter;

	//		converter.f = val;
	//		exponent = ((converter.i >> 23) & 0xFF) - 127;
	//		mantissa = converter.i & 0x7FFFFF;
	//	}
	//};
}