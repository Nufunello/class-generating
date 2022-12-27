#pragma once

#include <cstddef>
#include <utility>

namespace class_generating
{
	namespace util
	{
		template <std::size_t Length, typename Indexes>
		struct fixed_string;
		
		template <std::size_t Length, std::size_t ...Indexes>
		struct fixed_string<Length, std::index_sequence<Indexes...>>
		{
			char string[Length];
			constexpr fixed_string(const char (&string)[Length])
				: string{string[Indexes]...}
			{}
		};

		template <std::size_t Length>
		fixed_string(const char (&string)[Length]) -> fixed_string<Length, std::make_index_sequence<Length>>;

		template <typename Member>
		class generate_member;

		template <fixed_string, typename Type>
		class generated_member : public Type 
		{
		public:
			template <typename ...Args>
			constexpr generated_member(Args&& ...args)
				: Type(std::forward<Args>(args)...)
			{}
			using type = Type;
		};

		template <util::fixed_string Name>
		using tag = std::integral_constant<decltype(Name), Name>;
		
		template <util::fixed_string Name>
		using access_member = std::pair<tag<"access member">, tag<Name>>;
	}

	template <util::fixed_string Name>
	constexpr static auto tag = util::tag<Name>{};

	template <util::fixed_string Name>
	constexpr static auto access_member = util::access_member<Name>{};

	template <typename... Members>
	class generate_class
		: public util::generate_member<Members>::generate...
	{
	private:
		template <util::fixed_string Name>
		struct generated_member
		{
			using type = std::decay_t<std::invoke_result_t<
				decltype
				(
					[]<typename Type>(const util::generated_member<Name, Type>& generated_member)
					{
						return (generated_member);
					}
				),
				generate_class
			>>;
		};

	protected:
		using generated_members = generate_class;
		using construct_members = generate_class;

		template <util::fixed_string Name>
		using construct_member = generated_member<Name>::type;

		template <util::fixed_string Name>
		using access_member = generated_member<Name>::type::type;

	public:
		template <util::fixed_string Name>
		constexpr auto operator()(util::access_member<Name>)
			-> access_member<Name>&
		{
			return (*this);
		}
		template <util::fixed_string Name>
		constexpr auto operator()(util::access_member<Name>) const
			-> const access_member<Name>&
		{
			return (*this);
		}
	};
}
