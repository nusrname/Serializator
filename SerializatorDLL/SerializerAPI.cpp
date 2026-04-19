#include "SerializerAPI.h"
#include "NumberSerializer.h"
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
        std::vector<byte> data(input, input + inputSize);

        ByteWriter writer;

        // Пример: просто копируем (дальше вставишь свою логику)
        writer.writeBytes(data.data(), data.size());

        const auto& buffer = writer.getBuffer();

        *outputSize = static_cast<int>(buffer.size());

        unsigned char* result = new unsigned char[*outputSize];
        std::memcpy(result, buffer.data(), *outputSize);

        return result;
    }

    SERIALIZER_API unsigned char* DeserializeData(
        const unsigned char* input,
        int inputSize,
        int* outputSize)
    {
        std::vector<byte> data(input, input + inputSize);

        ByteReader reader(data);

        std::vector<byte> resultData;

        while (reader.hasMore())
        {
            resultData.push_back(reader.readByte());
        }

        *outputSize = static_cast<int>(resultData.size());

        unsigned char* result = new unsigned char[*outputSize];
        std::memcpy(result, resultData.data(), *outputSize);

        return result;
    }

    SERIALIZER_API void FreeMemory(unsigned char* ptr)
    {
        delete[] ptr;
    }
}