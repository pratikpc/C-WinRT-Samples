#pragma once

// This file controls the Handle
// The Handle holds Supreme Importance in How things are done

#include <Windows.h>
#include <algorithm>
#include <assert.h>

#ifdef _DEBUG
#	include <crtdbg.h>
#	ifndef VERIFY_FUNCTION
#		define VERIFY_FUNCTION(expression, result) assert(expression == result)
#	endif
#endif

namespace TUESL::Utility::Handler
{
	// Inspired from Kenny Kerr's Work

	template <typename Handler_Type>
	struct UniqueHandler
	{
		using POINTER		  = typename Handler_Type::POINTER;
		using CONST_POINTER = typename Handler_Type::CONST_POINTER;

	 private:
		POINTER m_value;

	 public:
		// No Copy Constructors allowed
		UniqueHandler(const UniqueHandler&) = delete;
		auto operator=(const UniqueHandler&) -> UniqueHandler& = delete;

		explicit UniqueHandler(POINTER value = Handler_Type::invalid()) noexcept :
			 m_value{value}
		{
		}

		UniqueHandler(UniqueHandler&& other) noexcept :
			 m_value{other.release()} // Only One Single Copy May Exist
		// This Handle is Unique
		{
		}

		UniqueHandler& operator=(UniqueHandler&& other) noexcept
		{
			if (this != &other)
				reset(other.release());

			return *this;
		}

		~UniqueHandler() noexcept
		{
			close();
		}

		bool empty() const noexcept
		{
			return m_value == Handler_Type::invalid();
		}
		bool hasValue() const noexcept
		{
			return !empty();
		}

		void close()
		{
			// Close only if it has a value
			if (hasValue())
				Handler_Type::close(m_value);
		}
		auto get() const noexcept
		{
			return m_value;
		}
		auto get() noexcept
		{
			return m_value;
		}

		auto getAddressOf() const noexcept
		{
			_ASSERTE(empty() && "getAddressOf");
			// If value is Invalid, aka NULL, It May Wreck all kinds of Havoc
			// As Reference to NULL is Undefined Behaviour
			return &m_value;
		}
		auto getAddressOf() noexcept
		{
			_ASSERTE(empty() && "getAddressOf");
			// If value is Invalid, aka NULL, It May Wreck all kinds of Havoc
			return &m_value;
		}

		// Returns the previous value
		auto release() noexcept(noexcept(Handler_Type::invalid()))
		{
			const auto old_value = m_value;
			m_value					= Handler_Type::invalid();
			return old_value;
		}

		bool reset(const POINTER p_new_value = Handler_Type::invalid())
		{
			if (m_value != p_new_value)
			{
				close();
				m_value = p_new_value;
			}

			// If the UniqueHandler is empty, than reset has succeded
			return empty();
		}

		auto swap(const UniqueHandler& right)
		{
			return std::swap(m_value, right.m_value);
		}
	};

	template <typename Handler_Type>
	bool operator==(const UniqueHandler<Handler_Type>& left,
						 const UniqueHandler<Handler_Type>& right)
	{
		return left.get() == right.get();
	}
	template <typename Handler_Type>
	bool operator!=(const UniqueHandler<Handler_Type>& left,
						 const UniqueHandler<Handler_Type>& right)
	{
		return !(left == right);
	}

	template <typename Handler_Type>
	bool operator<(const UniqueHandler<Handler_Type>& left,
						const UniqueHandler<Handler_Type>& right)
	{
		return left.get() < right.get();
	}
	template <typename Handler_Type>
	bool operator>(const UniqueHandler<Handler_Type>& left,
						const UniqueHandler<Handler_Type>& right)
	{
		return !(left < right);
	}

} // namespace TUESL::Utility::Handler
