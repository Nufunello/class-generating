#pragma once

#include "class_generating/fixed_string.hpp"

namespace class_generating::tags
{
	template <typename Tag> struct tag{};
	template <util::fixed_string Name> struct name{};
}
