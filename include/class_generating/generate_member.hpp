#pragma once

#include "class_generating/tags.hpp"

namespace class_generating::generate_member
{
	template <typename Member> struct generator;
	template <typename Member> using generator_t = typename generator<Member>::type;

	template <typename Tag, typename ...Args>
	struct construct_arguments
	{
		constexpr construct_arguments(tags::tag<Tag>, Args ...values) : values{std::move(values)...} {}
		std::tuple<Args...> values;

		//dispathing tag due to difficulty of variadic templates parsing(generate_class constructor)
		template <template <typename...> typename T> using type = T<Tag>; 
	};

	template <typename Tag, typename ...Args> construct_arguments(tags::tag<Tag>, Args ...args) -> construct_arguments<Tag, Args...>;

	template <typename T>
	class generated_member : public T
	{
	public:
		constexpr generated_member() = default;
		template <typename Tag, typename ...Args> 
		constexpr generated_member(construct_arguments<Tag, Args...> args) : T{std::move(std::get<Args>(args.values))...} {}
	};
}
