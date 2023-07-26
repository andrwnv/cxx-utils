#pragma once

#include <type_traits>
#include <cstdint>
#include <vector>
#include <array>

namespace utils
{

/**
 * Function for swap endian representation.
 * @param[in] value Value for endian swap.
 * @return          Endian swaped value.
 */
template <typename Type>
auto swapEndian(Type value) -> Type
{
    union Bytes
    {
        Type value;
        std::array<uint8_t, sizeof(Type)> bytes;
    } source, destination;

    source.value = value;
    std::copy(source.bytes.rbegin(), source.bytes.rend(), destination.bytes.begin());
    return destination.value;
}

} // namespace utils

/*

Example of use:

uint32_t testValue            =  0b0010'0010'0010'0010'0100'0100'0100'0100;
std::bitset<32> reversedValue = {0b0100'0100'0100'0100'0010'0010'0010'0010};

std::cout << swapEndian(testValue) << " ?== " << reversedValue.to_ulong() << " == " 
            << ((swapEndian(testValue) == reversedValue.to_ulong()) ? "true" : "false");

*/
