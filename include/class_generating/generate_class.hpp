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
		template <typename Tag>
		using find_member_template_t = typename reflection::find_member_template<generate_class>::template by_tag<Tag>::type;
	protected:
		template <typename Tag> using find_member_type_t = typename generate_member::template generator_t<find_member_template_t<Tag>>;
		template <typename Tag, typename ...Args> using construct_member = generate_member::construct_arguments<Tag, Args...>;

		template <typename ...Args> 
		constexpr generate_class(Args&& ...args) : Args::template type<find_member_type_t> { std::forward<Args>(args) }... {}
	public:
		template <typename Tag> constexpr auto operator()(tags::tag<Tag>) -> find_member_type_t<Tag>& { return *this; }
		template <typename Tag> constexpr auto operator()(tags::tag<Tag>) const -> const find_member_type_t<Tag>& { return *this; }
	};
	namespace reflection
	{
		template <template <typename...> typename GeneratedClass, typename ...Members>
		template <util::fixed_string Name>
		struct find_member_template<GeneratedClass<Members...>>::by_tag<tags::name<Name>>
		{
			template <util::fixed_string, typename Template> struct named_template{};
		public:
			using type = std::invoke_result_t<decltype([]<typename Template>(named_template<Name, Template>) -> Template{ return {}; }),
				type_operations::concatenate_structs_t<named_template<reflection::get_name_v<Members>, Members>...>>;
		};
	}
}
