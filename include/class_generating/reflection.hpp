#pragma once

namespace class_generating::reflection
{
	template <typename GeneratedClass>
	struct find_member_template;

	template <template <typename...> typename GeneratedClass, typename ...Members>
	struct find_member_template<GeneratedClass<Members...>>
	{
		template <typename Tag>
		struct by_tag;
	};

	template <typename Template>
	struct get_name;

	template <typename Template>
	static constexpr auto get_name_v = get_name<Template>::value;
}
