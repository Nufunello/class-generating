#pragma once

#include "class_generating/tags.hpp"
#include "class_generating/type_operations.hpp"

namespace class_generating::generate_member
{
	template <typename This, typename Member> struct generator;
	template <typename This, typename Member> using generator_t = typename generator<This, Member>::type;

	template <typename ...Args>
	struct construct_arguments
	{
		constexpr construct_arguments(Args ...values) : values{std::forward<Args>(values)...} {}
		std::tuple<Args...> values;
	};

	template <typename T>
	class generated_member : public T
	{
	public:
		constexpr generated_member() = default;
		template <typename Tag, typename ...Args, size_t ...Indexes>
		constexpr generated_member(construct_arguments<Tag, Args...> args, std::index_sequence<Indexes...>)
			: T{std::move(std::get<Indexes>(args.values))...} {}
	};
}
