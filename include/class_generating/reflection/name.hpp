#pragma once

namespace class_generating::reflection
{
	template <typename Template> struct get_name;
	template <typename Template> static constexpr auto get_name_v = get_name<Template>::value;
	
	template <util::fixed_string Name, typename Member>
	struct condition_for_tag<tags::name<Name>, Member>
	{
		static constexpr bool value = std::is_same_v<tags::name<Name>, tags::name<reflection::get_name_v<Member>>>;
	};
}