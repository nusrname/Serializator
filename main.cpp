#include <iostream>
#include "NumberSerializer.h"
using namespace NumberSerializer;

int main()
{
	ByteWriter writer;

	IntSerializer::serialize(writer, 42);
	FloatSerializer::serialize(writer, 3.14f);
	BoolSerializer::serialize(writer, true);

	SaveToFile(writer.getBuffer(), "save.dat");

	// Загрузка
	auto loadedData = LoadFromFile("save.dat");

	ByteReader reader(loadedData);

	int a = IntSerializer::deserialize(reader);
	float b = FloatSerializer::deserialize(reader);
	bool c = BoolSerializer::deserialize(reader);

	std::cout << a << " " << b << " " << c;

	return 0;
}