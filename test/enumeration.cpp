#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <functional>

#include "class_generating/enumeration.hpp"

using Enum = class_generating::generate_enumeration
<
	"Enum123", float 
	, class_generating::enumeration::field<"One", 1>
	, class_generating::enumeration::field<"Two", 2>
>;

namespace
{
	void constexppr()
	{
		static_assert(Enum::value<"One"> == Enum{1});
		static_assert(Enum::value<"Two"> == Enum{2});

		static_assert(Enum::value<"One"> != Enum{2});
		static_assert(Enum::value<"Two"> != Enum{1});
		std::cout << "Equal" << std::endl;
	}
}

int main()
{
	constexppr();
}