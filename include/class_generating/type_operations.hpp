#pragma once

namespace class_generating::type_operations
{
	template <typename ...Types>
	struct concatenate_structs_t : public Types... {};
}
