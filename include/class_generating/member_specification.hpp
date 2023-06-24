#pragma once

#include "class_generating/fixed_string.hpp"
#include "class_generating/type_operations.hpp"

#include "class_generating/reflection.hpp"
#include "class_generating/reflection/required_options.hpp"

namespace class_generating
{
	namespace util
	{
		template <fixed_string ...Options> struct member_specification {};
	}

	namespace tags
	{
		template <bool Only, util::fixed_string ...MemberSpecification> struct includes_member_specification{};
		template <util::fixed_string ...MemberSpecification> struct excludes_member_specification{};
	}

	namespace reflection
	{
		template <util::fixed_string ...Options, util::fixed_string ...RequiredOptions, bool Only>
		struct has_required_options<util::member_specification<Options...>, util::member_specification<RequiredOptions...>, Only>
		{
			static constexpr bool value = (
				(Only ? (sizeof...(Options) == sizeof...(RequiredOptions)) : (sizeof...(Options) >= sizeof...(RequiredOptions))) && 
					... && []<util::fixed_string Option>(util::member_specification<Option>)
					{ return (std::is_same_v<util::member_specification<Option>, util::member_specification<RequiredOptions>> || ...); }
					(util::member_specification<Options>{}));
		};

		template <util::fixed_string ...MemberSpecification, typename Member, bool Only>
		struct condition_for_tag<tags::includes_member_specification<Only, MemberSpecification...>, Member>
		{
			static constexpr bool value = has_required_options_v<Member, util::member_specification<MemberSpecification...>, Only>;
		};

		template <util::fixed_string ...Options, util::fixed_string ...RequiredOptions>
		struct has_not_required_options<util::member_specification<Options...>, util::member_specification<RequiredOptions...>>
		{
			static constexpr bool value = (true && ... && []<util::fixed_string Option>(util::member_specification<Option>)
					{ return (!std::is_same_v<util::member_specification<Option>, util::member_specification<RequiredOptions>> && ...); }
				(util::member_specification<Options>{}));
		};

		template <util::fixed_string ...MemberSpecification, typename Member>
		struct condition_for_tag<tags::excludes_member_specification<MemberSpecification...>, Member>
		{
			static constexpr bool value = has_not_required_options_v<Member, util::member_specification<MemberSpecification...>>;
		};
	}
}
