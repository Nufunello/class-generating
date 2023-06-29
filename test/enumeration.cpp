#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <functional>

#include "class_generating/enumeration.hpp"

namespace
{
	void IntegerConstexpr()
	{
		using Integer = class_generating::generate_enumeration
		<
			"Integer", int
			, class_generating::enumeration::field<"One", 1>
			, class_generating::enumeration::field<"Two", 2>
		>;

		static_assert(Integer::value<"One"> == Integer{1});
		static_assert(Integer::value<"Two"> == Integer{2});

		static_assert(Integer::value<"One"> != Integer{2});
		static_assert(Integer::value<"Two"> != Integer{1});
	}
	void IncrementConstexpr()
	{
		using Increment = class_generating::generate_enumeration
		<
			"Increment", float
			, class_generating::enumeration::field<"One", 1>
			, class_generating::enumeration::empty_field<"Two">
		>;

		static_assert(Increment::value<"One"> == Increment{1.f});
		static_assert(Increment::value<"Two"> == Increment{2.f});

		static_assert(Increment::value<"One"> != Increment{2.f});
		static_assert(Increment::value<"Two"> != Increment{1.f});
	}
	void ZeroConstexpr()
	{
		using Zero = class_generating::generate_enumeration
		<
			"Zero", float
			, class_generating::enumeration::empty_field<"Zero">
			, class_generating::enumeration::field<"One", 1>
		>;

		static_assert(Zero::value<"Zero"> == Zero{0.f});
		static_assert(Zero::value<"One"> == Zero{1.f});

		static_assert(Zero::value<"Zero"> != Zero{1.f});
		static_assert(Zero::value<"One"> != Zero{0.f});
	}
	void ZeroIncrementConstexpr()
	{
		using Zero = class_generating::generate_enumeration
		<
			"Zero", float
			, class_generating::enumeration::empty_field<"Zero">
			, class_generating::enumeration::empty_field<"One">
		>;

		static_assert(Zero::value<"Zero"> == Zero{0.f});
		static_assert(Zero::value<"One"> == Zero{1.f});

		static_assert(Zero::value<"Zero"> != Zero{1.f});
		static_assert(Zero::value<"One"> != Zero{0.f});
	}
}

int main()
{
}