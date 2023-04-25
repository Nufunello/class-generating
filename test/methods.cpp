#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <functional>

#include "class_generating/generate_class.hpp"
#include "class_generating/method.hpp"

namespace
{
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

	class VirtualFunction
		: public class_generating::generate_class
		<
			VirtualFunction,
			class_generating::method<"foo", class_generating::function_signature<int, int>, class_generating::method_options<"virtual">>
		>
	{
	public:
		VirtualFunction() 
			: generate_class{ construct_member { class_generating::access_member_by_name<"foo">, [](VirtualFunction&, int value) { return value; } }}{}
	};

	class VirtualFunctionOverride
		: public VirtualFunction
	{
	public:
		using generate_class::operator();
		int operator()(int value, class_generating::tags::name<"foo"> = {}) override
		{
			return value * 2;
		}
	};

	void base()
	{
		auto functions = Functions{};
		functions(class_generating::access_member_by_name<"foo">)(1);
		const auto& cFunctions = functions;
		cFunctions(class_generating::access_member_by_name<"foo">)("2");
		functions(class_generating::access_member_by_name<"const">)("3");
	}
}

int main()
{
	srand((unsigned) time(NULL));
	VirtualFunction vf;
	VirtualFunctionOverride vfo;

	std::cout << "Expects 1, actual " << vf(class_generating::access_member_by_name<"foo">)(1) << std::endl;
	std::cout << "Expects 2, actual " << vfo(class_generating::access_member_by_name<"foo">)(1) << std::endl;
}
