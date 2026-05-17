#include "pch.h"
#include "SerializerAPI.h"
#include "NumberSerializer.h"
#include "CollectionSerializer.h"
#include <vector>
#include <cstring>

using namespace Serializer;

extern "C"
{
    SERIALIZER_API unsigned char* SerializeData(
        const unsigned char* input,
        int inputSize,
        int* outputSize)
    {
        // интерпретируем вход как int32 массив
        const int32_t* ints =
            reinterpret_cast<const int32_t*>(input);

        int count = inputSize / sizeof(int32_t);

        std::vector<int32_t> data(ints, ints + count);

        ByteWriter writer;

        ArraySerializer::serialize<int32_t>(
            writer,
            data,
            IntSerializer::serialize
        );

        const auto& buffer = writer.getBuffer();

        *outputSize = static_cast<int>(buffer.size());

        unsigned char* result =
            new unsigned char[*outputSize];

        std::memcpy(result, buffer.data(), *outputSize);

        return result;
    }

    SERIALIZER_API unsigned char* DeserializeData(
        const unsigned char* input,
        int inputSize,
        int* outputSize)
    {
        if (!input || inputSize <= 0)
        {
            *outputSize = 0;
            return nullptr;
        }

        std::vector<byte> data(input, input + inputSize);

        ByteReader reader(data);

        ByteWriter writer;

        while (reader.hasMore(1))
        {
            byte b = reader.readByte();
            writer.writeByte(b);
        }

        const auto& buffer = writer.getBuffer();

        *outputSize = static_cast<int>(buffer.size());

        unsigned char* result = new unsigned char[*outputSize];

        if (*outputSize > 0)
            std::memcpy(result, buffer.data(), *outputSize);

        return result;
    }

    SERIALIZER_API void FreeMemory(unsigned char* ptr)
    {
        delete[] ptr;
    }
}