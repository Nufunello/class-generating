#pragma once

#include "class_generating/tags.hpp"
#include "class_generating/generate_member.hpp"
#include "class_generating/fixed_string.hpp"
#include "class_generating/reflection.hpp"
#include "class_generating/type_operations.hpp"

namespace class_generating
{
	template <typename Tag> static constexpr auto access_member = tags::tag<Tag>{};
	template <util::fixed_string Name> static constexpr auto access_member_by_name = access_member<tags::name<Name>>;

	template <typename... Members>
	class generate_class
		: public generate_member::generator_t<Members>...
	{
		template <typename ...Tags>
		struct find_member_type
		{
			using type = std::decay_t
			<
				std::invoke_result_t
				<
					decltype([]<typename ...Results>(type_operations::array<Results...> results)
					{
						static_assert(sizeof...(Results) > 0, "No member found by tags");
						static_assert(sizeof...(Results) == 1, "More than one member found by tags");
						if constexpr (sizeof...(Results) == 1)
						{
							return std::invoke([]<typename First>(type_operations::array<First>)
								-> generate_member::generator_t<First> { return {}; }, results);
						}
					}), 
					typename reflection::find_member_template<generate_class>::template by_tag<Tags...>::type
				>
			>;
		};

		template <typename T, typename = void> struct has_type : std::false_type{};
		template <typename T> static constexpr bool has_type_v = has_type<T>::value;

	protected:
		template <typename ...Tags> using find_member_type_t = typename find_member_type<Tags...>::type;
		template <typename Tag, typename ...Args> using construct_member = generate_member::construct_arguments<Tag, Args...>;

		template <typename ...Args> 
		constexpr generate_class(Args&& ...args) : Args::template type<find_member_type_t> { std::forward<Args>(args) }... {}
	public:
		template <typename Tag, typename Return = find_member_type<Tag, tags::excludes_member_specification<"const">>>
		requires (has_type_v<Return>) constexpr auto operator()(tags::tag<Tag>) -> Return::type { return *this; }
		template <typename Tag> requires (!has_type_v<find_member_type<Tag, tags::excludes_member_specification<"const">>>)
		constexpr auto operator()(tags::tag<Tag>) -> find_member_type_t<Tag> { return *this; }

		template <typename Tag> constexpr auto operator()(tags::tag<Tag>) const
			-> find_member_type_t<Tag, tags::includes_member_specification<false, "const">>& { return *this; }
	};
	
	template <typename... Members>
	template <typename T>
	struct generate_class<Members...>::has_type<T, std::void_t<typename T::type>> : std::true_type {};

	namespace reflection
	{
		template <util::fixed_string Name, typename Member>
		struct condition_for_tag<tags::name<Name>, Member>
		{
			static constexpr bool value = std::is_same_v<tags::name<Name>, tags::name<reflection::get_name_v<Member>>>;
		};
	}
}
