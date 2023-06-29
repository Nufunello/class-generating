#pragma once

#include "class_generating/fixed_string.hpp"
#include "class_generating/tags.hpp"

#include "class_generating/reflection.hpp"
#include "class_generating/reflection/name.hpp"

namespace class_generating
{
	namespace enumeration::util
	{
		struct no_value{};
		constexpr static auto no_value_v = no_value{};
	}

	namespace enumeration
	{
		template <class_generating::util::fixed_string Name, auto Value> struct field{};
		template <class_generating::util::fixed_string Name> using empty_field = field<Name, enumeration::util::no_value_v>;
	}

	namespace enumeration::util
	{
		template <typename T, typename Cur, typename Args = type_operations::array<>> struct iterate_transform;
	}

	template <util::fixed_string Name, typename T, typename ...Fields>
	using generate_enumeration = typename decltype((enumeration::util::iterate_transform<T, Fields>{} + ...))::type<Name>;

	namespace tags::enumeration
	{
		template<typename T, T> struct value{};
	}

	namespace reflection::enumeration
	{
		template <typename Enum> struct get_fields;
		template <typename Enum> using get_fields_t = typename get_fields<Enum>::type;

		template <typename Enum> struct get_value_type;
		template <typename Enum> using get_value_type_t = typename get_value_type<Enum>::type;

		template <typename T> struct get_value;
		template <typename T> constexpr auto get_value_v = get_value<T>::value;

		template <typename T, typename Tag> struct get_field;
		template <typename T, typename Tag> using get_field_t = typename get_field<T, Tag>::type;

		template <typename T, typename Tag> struct get_previous_field;
		template <typename T, typename Tag> using get_previous_field_t = typename get_previous_field<T, Tag>::type;
	}

	namespace enumeration::util
	{
		template <class_generating::util::fixed_string Name, typename T, typename ...Fields>
		class enumeration_value : private Fields...
		{
			T _;
		public:
			constexpr enumeration_value(T value) : _{std::move(value)} {}
			constexpr operator const T&() const & { return (_); }
			constexpr operator T() && { return std::move(_); }
			template <class_generating::util::fixed_string FieldName>
			static constexpr auto value = enumeration_value
			{
				reflection::enumeration::get_value_v
				<
					reflection::enumeration::get_field_t<enumeration_value, tags::name<FieldName>>
				>
			};
		};

		template <typename T, typename Field>
		struct transform_first
		{
			using type = Field;
		};
		template <typename T, class_generating::util::fixed_string Name>
		struct transform_first<T, enumeration::empty_field<Name>>
		{
			using type = enumeration::field<Name, T{}>;
		};

		template <typename, typename Current>
		struct transform_with_previous
		{
			using type = Current;
		};
		template <class_generating::util::fixed_string PrevName, auto PrevValue, class_generating::util::fixed_string Name>
		struct transform_with_previous<enumeration::field<PrevName, PrevValue>, enumeration::empty_field<Name>>
		{
			using type = enumeration::field<Name, PrevValue + 1>;
		};

		template <typename T, typename Cur, typename... Args>
		struct iterate_transform<T, Cur, type_operations::array<Args...>>
		{
			template <class_generating::util::fixed_string Name> using type = enumeration_value<Name, T, Args..., Cur>;
			template <typename Next> constexpr auto operator+(const iterate_transform<T, Next, type_operations::array<>>&) const
			{
				using current = std::conditional_t<sizeof...(Args) == 0, typename transform_first<T, Cur>::type, Cur>;
				return iterate_transform
				<
					T,
					typename transform_with_previous<current, Next>::type,
					type_operations::array<Args..., current>
				>{};
			}
		};
	}

	namespace reflection
	{
		template <util::fixed_string Name, typename ...T>
		struct get_name<class_generating::enumeration::util::enumeration_value<Name, T...>>
		{
			static constexpr auto value = Name;
		};
		template <util::fixed_string Name, auto _>
		struct get_name<class_generating::enumeration::field<Name, _>>
		{
			static constexpr auto value = Name;
		};
		template <typename T, T Value, class_generating::util::fixed_string _, auto FieldValue>
		struct condition_for_tag<tags::enumeration::value<T, Value>, class_generating::enumeration::field<_, FieldValue>>
		{
			static constexpr bool value = false;
		};
		template <typename T, T Value, class_generating::util::fixed_string _>
		struct condition_for_tag<tags::enumeration::value<T, Value>, class_generating::enumeration::field<_, Value>>
		{
			static constexpr bool value = true;
		};
	}

	namespace reflection::enumeration
	{
		template <class_generating::util::fixed_string Name, typename T, typename ...Fields>
		struct get_fields<class_generating::enumeration::util::enumeration_value<Name, T, Fields...>> 
		{
			using type = type_operations::array<Fields...>;
		};

		template <class_generating::util::fixed_string Name, typename T, typename ...Fields>
		struct get_value_type<class_generating::enumeration::util::enumeration_value<Name, T, Fields...>> 
		{
			using type = T;
		};

		template <class_generating::util::fixed_string Name, auto Value>
		struct get_value<class_generating::enumeration::field<Name, Value>>
		{
			static constexpr auto value = Value;
		};

		template <class_generating::util::fixed_string EnumName, typename T, typename ...Fields, typename Tag>
		struct get_field<class_generating::enumeration::util::enumeration_value<EnumName, T, Fields...>, Tag>
		{
			using type = std::invoke_result_t<decltype([]{
				using enumeration = class_generating::enumeration::util::enumeration_value<EnumName, T, Fields...>;
				using fields = typename reflection::find_member_template<get_fields_t<enumeration>>
					::template by_tag<Tag>
					::type;
				return std::invoke_result_t
				<
					decltype([]<typename Field>(type_operations::array<Field>) { return Field{}; }), 
					fields
				>{};
			})>;
		};
	}
}