#pragma once

#ifdef SERIALIZER_EXPORTS
#define SERIALIZER_API __declspec(dllexport)
#else
#define SERIALIZER_API __declspec(dllimport)
#endif

extern "C"
{
    SERIALIZER_API unsigned char* SerializeData(
        const unsigned char* input,
        int inputSize,
        int* outputSize
    );

    SERIALIZER_API unsigned char* DeserializeData(
        const unsigned char* input,
        int inputSize,
        int* outputSize
    );

    SERIALIZER_API void FreeMemory(unsigned char* ptr);
}