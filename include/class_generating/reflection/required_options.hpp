#pragma once

namespace class_generating::reflection
{
	template <typename Object, typename RequiredOptions, bool Only> struct has_required_options : std::true_type{};
	template <typename Object, typename RequiredOptions, bool Only>
	static constexpr bool has_required_options_v = has_required_options<Object, RequiredOptions, Only>::value;

	template <typename Object, typename RequiredOptions> struct has_not_required_options : std::true_type{};
	template <typename Object, typename RequiredOptions>
	static constexpr bool has_not_required_options_v = has_not_required_options<Object, RequiredOptions>::value;
}