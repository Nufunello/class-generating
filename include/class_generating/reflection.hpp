#pragma once

#include "class_generating/type_operations.hpp"

namespace class_generating::reflection
{
	template <typename GeneratedClass> struct find_member_template;
	template <template <typename...> typename GeneratedClass, typename ...Members>
	struct find_member_template<GeneratedClass<Members...>>
	{
		template <typename Tag, typename ...Tags>
		struct by_tag
		{
		private:
			template <typename Class, typename ...T>
			using find_by_tag = typename find_member_template<Class>::template by_tag<T...>::type;

		public:
			using type = find_by_tag
			<
				find_by_tag<GeneratedClass<Members...>, Tags...>, Tag //filter by Tag result of filter by Tags
			>;
		};
	};

	template <template <typename...> typename GeneratedClass, typename ...Members>
	template <typename Tag>
	struct find_member_template<GeneratedClass<Members...>>
		::by_tag<Tag>
	{
	private:
		template <typename Applicable, typename ...Left> struct filter;
	public:
		using type = typename filter<type_operations::array<>, Members...>::type;
	};

	template <template <typename...> typename GeneratedClass, typename ...Members>
	template <typename Tag>
	template <typename ...Applicable>
	struct find_member_template<GeneratedClass<Members...>>
		::by_tag<Tag>
		::filter<type_operations::array<Applicable...>>
	{
		using type = type_operations::array<Applicable...>;
	};

	template <typename Tag, typename Member> struct condition_for_tag;
	template <typename Tag, typename Member>
	static constexpr bool condition_for_tag_v = condition_for_tag<Tag, Member>::value;

	template <template <typename...> typename GeneratedClass, typename ...Members>
	template <typename Tag>
	template <typename ...Applicable, typename Member, typename ...Left>
	struct find_member_template<GeneratedClass<Members...>>
		::by_tag<Tag>
		::filter
		<
			type_operations::array<Applicable...>, 
			Member, 
			Left...
		>
	{
		using type = find_member_template<GeneratedClass<Members...>>
			::by_tag<Tag>
			::filter
			<
				std::conditional_t
				<
					condition_for_tag_v<Tag, Member>,
					type_operations::array<Applicable..., Member>,
					type_operations::array<Applicable...>
				>,
				Left...
			>
			::type;
	};
}
