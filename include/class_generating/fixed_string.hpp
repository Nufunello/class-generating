#pragma once

#include <type_traits>

namespace class_generating::util
{
	template <size_t Length, typename Indexes>
	struct fixed_string;
	
	template <size_t Length, size_t ...Indexes>
	struct fixed_string<Length, std::index_sequence<Indexes...>>
	{
		char string[Length];
		constexpr fixed_string(const char (&string)[Length])
			: string{string[Indexes]...}
		{}
	};

	template <size_t Length>
	fixed_string(const char (&string)[Length]) -> fixed_string<Length, std::make_index_sequence<Length>>;
}
