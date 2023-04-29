#pragma once

#include "class_generating/tags.hpp"
#include "class_generating/type_operations.hpp"

namespace class_generating::generate_member
{
	template <typename This, typename Member> struct generator;
	template <typename This, typename Member> using generator_t = typename generator<This, Member>::type;

	template <typename Tags, typename Args>
	struct construct_arguments;
	template <typename ...Tags, typename ...Args>
	struct construct_arguments<type_operations::array<Tags...>, type_operations::array<Args...>>
	{
		constexpr construct_arguments(tags::tags<Tags...>, Args ...values) : values{std::move(values)...} {}
		std::tuple<Args...> values;

		//dispathing tag due to difficulty of variadic templates parsing(generate_class constructor)
		template <template <typename...> typename T> using type = T<Tags...>;
	};

	template <typename ...Tags, typename ...Args> construct_arguments(tags::tags<Tags...>, Args ...args)
		-> construct_arguments<type_operations::array<Tags...>, type_operations::array<Args...>>;

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
