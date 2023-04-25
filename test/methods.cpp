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
			Functions,
			class_generating::method<"foo", class_generating::function_signature<void, int>, class_generating::method_options<>>,
			class_generating::method<"foo", class_generating::function_signature<void, std::string>, class_generating::method_options<"const">>,
			class_generating::method<"const", class_generating::function_signature<void, std::string>, class_generating::method_options<"const">>
		>
	{
	public:
		using generate_class::operator();
		Functions()
			: generate_class
			{
				construct_member
				{
					class_generating::tags::tags
					<
						class_generating::tags::name<"foo">,
						class_generating::tags::excludes_member_specification<"const">
					>{},
					[](Functions& functions, int value) 
					{ 
						std::cout << "Non-const function" << std::endl
						<< value << std::endl;
					}
				},
				construct_member
				{
					class_generating::tags::tags
					<
						class_generating::tags::name<"foo">,
						class_generating::tags::includes_member_specification<false, "const">
					>{},
					[](const Functions& functions, std::string value) 
					{ 
						std::cout << "Const function" << std::endl
						<< value << std::endl;
					}
				},
				construct_member{class_generating::access_member_by_name<"const">, 
					[](const Functions& functions, std::string value) 
					{ 
						std::cout << "Const function" << std::endl
						<< value << std::endl;
					}},
			}
		{}
	};
}

int main()
{
	srand((unsigned) time(NULL));
	auto functions = Functions{};
	functions(class_generating::access_member_by_name<"foo">)(1);
	const auto& cFunctions = functions;
	cFunctions(class_generating::access_member_by_name<"foo">)("2");
	functions(class_generating::access_member_by_name<"const">)("3");
}
