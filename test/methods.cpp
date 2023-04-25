#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <functional>

#include "class_generating/generate_class.hpp"
#include "class_generating/method.hpp"

namespace
{
	constexpr static std::size_t MAX_YEARS_MAX = 100;
	class Functions
		: public class_generating::generate_class
		<
			class_generating::method<"foo", class_generating::function_signature<void, int>, class_generating::method_options<>>,
			class_generating::method<"foo", class_generating::function_signature<void, std::string>, class_generating::method_options<"const">>,
			class_generating::method<"const", class_generating::function_signature<void, std::string>, class_generating::method_options<"const">>
		>
	{
	public:
		using generate_class::operator();
		Functions() = default;
	};
}

int main()
{
	srand((unsigned) time(NULL));
	auto functions = Functions{};
	functions(class_generating::access_member_by_name<"foo">)(1);
	const auto& cFunctions = functions;
	cFunctions(class_generating::access_member_by_name<"foo">)("asd");
	functions(class_generating::access_member_by_name<"const">)("asd");
}
