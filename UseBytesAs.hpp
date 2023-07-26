#pragma once

#include <type_traits>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <vector>
#include <array>


namespace utils
{

/**
 * Extends type_traits to be able to check container for whether its array or vector.
 */
template<typename T>
struct is_array_or_vector 
{
    enum { value = false };
};

template<typename T, typename... Allocator>
struct is_array_or_vector<std::vector<T, Allocator...>> 
{
    enum { value = true };
};

template<typename T, std::size_t N>
struct is_array_or_vector<std::array<T, N>> 
{
    enum { value = true };
};

template<class T>
inline constexpr bool is_array_or_vector_v = is_array_or_vector<T>::value;

/**
 * Function represent a byte data in container as some signed/unsigned integer types (like uint8_t, int8_t, uint16_t, int16_t, etc.).
 * @param[in] container Container from which bytes will be extracted for representation in the desired data type.
 * @param[in] offset    Parameter for offset by certain number of container elements.
 * @return              Resulting representation of the N bytes of the container in the requested type.
 */
template<
    typename OutputType,
    typename Container, typename InputType = typename Container::value_type, 

    std::enable_if_t<std::is_arithmetic_v<OutputType>, bool> = true, 
    std::enable_if_t<std::is_arithmetic_v<InputType> || sizeof(InputType) == 1, bool> = true,
    std::enable_if_t<is_array_or_vector_v<Container>, bool> = true
>
auto useBytesAs(Container const& container, size_t offset = 0) -> OutputType 
{
    OutputType result;
    std::memcpy(&result, container.data() + offset * sizeof(InputType), sizeof(result));
    return result;
}

} // namespace utils


/*

Example of use:

std::vector<char>      data16 { 2, 4, 12, 5 };
std::array<uint8_t, 4> data32 { 2, 4, 12, 5 };

std::cout << useBytesAs<uint16_t>(data16) << std::endl;
std::cout << useBytesAs<uint32_t>(data32) << std::endl;

*/
