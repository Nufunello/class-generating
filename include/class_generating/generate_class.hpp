#pragma once

#include "class_generating/tags.hpp"
#include "class_generating/fixed_string.hpp"
#include "class_generating/type_operations.hpp"

#include "class_generating/generate_member.hpp"
#include "class_generating/member_specification.hpp"

#include "class_generating/reflection.hpp"
#include "class_generating/reflection/name.hpp"

namespace class_generating
{
	template <typename Tag> static constexpr auto access_member = tags::tags<Tag>{};
	template <util::fixed_string Name> static constexpr auto access_member_by_name = access_member<tags::name<Name>>;

	template <typename This, typename... Members>
	class generate_class
		: public generate_member::generator_t<This, Members>...
	{
	public:
		template <typename ...Tags>
		using find_members_type_t = typename reflection::find_member_template<type_operations::array<Members...>>::template by_tag<Tags...>::type;

		template <typename Result> struct member_type { using type = void; };
		template <typename Result> using member_type_t = typename member_type<Result>::type; 

		template <typename T> static constexpr bool has_type_v = !std::is_same_v<void, member_type_t<T>>;

	protected:
		template <typename ...Tags> using find_member_type_t = member_type_t<find_members_type_t<Tags...>>;
		template <typename ...Tags, typename ...Args> static decltype(auto) construct_member(tags::tags<Tags...>, Args&& ...args)
		{
			struct constructor 
				: generate_member::construct_arguments<Args&&...>
			{
				constructor(Args&& ...args) : generate_member::construct_arguments<Args&&...>{std::forward<Args>(args)...} {}
				using type = find_member_type_t<Tags...>;
			};
			return constructor{std::forward<Args>(args)...};
		}

		template <typename ...Args> 
		constexpr generate_class(Args&& ...args) : Args::type
			{ std::forward<Args>(args), std::make_index_sequence<std::tuple_size_v<decltype(args.values)>>{}}...
		{}
	public:
		template <typename Tag, typename Return = find_members_type_t<Tag, tags::excludes_member_specification<"const">>>
		requires (has_type_v<Return>) constexpr auto operator()(tags::tags<Tag>) -> member_type_t<Return>& {  return *this; }

		template <typename Tag, typename Results = find_members_type_t<Tag>, 
			typename NonConst = typename reflection::find_member_template<Results>::template by_tag<tags::excludes_member_specification<"const">>::type>
		requires (!has_type_v<NonConst>) constexpr auto operator()(tags::tags<Tag>) -> member_type_t<Results>&  { return *this; }

		template <typename Tag> constexpr auto operator()(tags::tags<Tag>) const
			-> const find_member_type_t<Tag, tags::includes_member_specification<false, "const">>& { return *this; }
	};

	template <typename This, typename... Members>
	template <typename T>
	struct generate_class<This, Members...>::member_type<type_operations::array<T>> { using type = generate_member::generator_t<This, T>; };
}
