#pragma once

#include "class_generating/fixed_string.hpp"
#include "class_generating/type_operations.hpp"

namespace class_generating::tags
{
	template <typename ...Tags> using tags = type_operations::array<Tags...>;
	template <util::fixed_string Name> struct name{};
}
