#pragma once

#include <array>

namespace TUESL::Utility
{
	// Note that this gets the size_in_bytes of any given type
	// Note that char_bit used as not every OS has
	// 1 byte = 8 bit
	// And the standard guarantees that char is always a type
	// of 1 byte
	// Hence CHAR_BIT = No of Bits Equal to 1 Byte
	template <typename T>
	constexpr const std::size_t size_in_bytes = sizeof(T) * CHAR_BIT;

	// Returns Absolute Value
	// If the Integer value is Negative, return Positive Value of it

	template<typename ArithmeticType>
	ArithmeticType absoluteValue(const ArithmeticType p_value) noexcept
	{
		static_assert(std::is_arithmetic_v<ArithmeticType>, "Absolute Value can only be found for Integral Values");

		// If type is Unsigned the absolute value is same
		if constexpr(std::is_unsigned_v<ArithmeticType>)
			return p_value;

		return ((p_value < static_cast<ArithmeticType>(0))
			? -p_value // If Number is Negative, Return Positive
			: p_value);
	}
}