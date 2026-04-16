#pragma once
#include "NumberSerializer.h"
#include <string>

namespace Serializer
{
	class CharSerializer
	{
	public:
		static void serialize(ByteWriter& writer, char value);
		static char deserialize(ByteReader& reader);
	};

	class StringSerializer
	{
	public:
		static void serialize(ByteWriter& writer, const std::string& value);
		static std::string deserialize(ByteReader& reader);
	};
}