#pragma once

#include "class_generating/generate_member.hpp"
#include "class_generating/tags.hpp"
#include "class_generating/member_specification.hpp"
#include "class_generating/reflection.hpp"
#include "class_generating/fixed_string.hpp"

namespace class_generating
{
	template <util::fixed_string Name, typename Signature, typename Options>
	struct method;

	template <typename Return, typename ...Args> struct function_signature {};
	template <util::fixed_string ...Options> using method_options = util::member_specification<Options...>;

	template <util::fixed_string Name, typename Return, typename ...Args, util::fixed_string ...Options>
	struct method<Name, function_signature<Return, Args...>, method_options<Options...>> {};

	namespace methods::util
	{
		template <class_generating::util::fixed_string ToTranslate>
		static constexpr auto unify_member_specification = ToTranslate;

		static constexpr auto const_qualifier = class_generating::util::fixed_string{"const"};
		static constexpr auto volatile_qualifier = class_generating::util::fixed_string{"volatile"};

		static constexpr auto lvalue_specifier = class_generating::util::fixed_string{"&"};
		static constexpr auto rvalue_specifier = class_generating::util::fixed_string{"&&"};

		static constexpr auto virtual_specifier = class_generating::util::fixed_string{"virtual"};

		static constexpr auto pure_virtual_specifier = class_generating::util::fixed_string{"=0"};
		template <> constexpr auto unify_member_specification<"= 0"> = pure_virtual_specifier;
		template <> constexpr auto unify_member_specification<" = 0"> = pure_virtual_specifier;
		template <> constexpr auto unify_member_specification<"abstract"> = pure_virtual_specifier;
		template <> constexpr auto unify_member_specification<"pure virtual"> = pure_virtual_specifier;

		template <bool Only, typename RequiredOptions, class_generating::util::fixed_string ...Options>
		static constexpr bool method_has_required_options =
			reflection::has_required_options_v<
				method<"", function_signature<void>, method_options<unify_member_specification<Options>...>>, 
				RequiredOptions,
				Only>;

		template <typename RequiredOptions, class_generating::util::fixed_string ...Options>
		static constexpr bool method_has_not_required_options =
			reflection::has_not_required_options_v<
				method<"", function_signature<void>, method_options<unify_member_specification<Options>...>>, 
				RequiredOptions>;

		template <typename This, class_generating::util::fixed_string Name, typename Signature, typename Options> class method_implementation;

		template <typename This, class_generating::util::fixed_string Name, typename Return, typename ...Args,
			class_generating::util::fixed_string ...Options> 
			requires (method_has_not_required_options<class_generating::util::member_specification<virtual_specifier>, Options...>)
		class method_implementation<This, Name, function_signature<Return, Args...>, method_options<Options...>>
		{
			Return (*code)(This&, Args...);
		protected:
			template <typename T>
			constexpr method_implementation(T&& t) : code{std::forward<T>(t)} {}
		public:
			Return operator()(Args ...args) 
				requires (method_has_required_options<true, class_generating::util::member_specification<>, Options...>)
			{
				return code(static_cast<This&>(*this), std::forward<Args>(args)...);
			}
			Return operator()(Args ...args) const
				requires (method_has_required_options<true, class_generating::util::member_specification<const_qualifier>, Options...>)
			{
				return code(static_cast<This&>(*this), std::forward<Args>(args)...);
			}
			Return operator()(Args ...args) volatile
				requires (method_has_required_options<true, class_generating::util::member_specification<volatile_qualifier>, Options...>)
			{
				return code(static_cast<This&>(*this), std::forward<Args>(args)...);
			}
		};
		template <typename This, class_generating::util::fixed_string Name, typename Return, typename ...Args,
			class_generating::util::fixed_string ...Options>
			requires (method_has_required_options<true, class_generating::util::member_specification<virtual_specifier>, Options...>)
		class method_implementation<This, Name, function_signature<Return, Args...>, method_options<Options...>>
		{
			Return (*code)(This&, Args...);
		protected:
			template <typename T>
			constexpr method_implementation(T&& t) : code{std::forward<T>(t)} {}
		public:
			virtual Return operator()(Args ...args, tags::name<Name> = {})
			{
				return code(static_cast<This&>(*this), std::forward<Args>(args)...);
			}
		};

		template <typename This, typename Options> struct add_cv_qualifier;
		template <typename This, class_generating::util::fixed_string ...Options>
		struct add_cv_qualifier<This, method_options<Options...>>
		{
			using type = std::conditional_t
			<
				method_has_required_options<false, class_generating::util::member_specification<Options...>, const_qualifier>,
				std::conditional_t
				<
					method_has_required_options<false, class_generating::util::member_specification<Options...>, volatile_qualifier>,
					std::add_cv_t<This>,
					std::add_const_t<This>
				>,
				std::conditional_t
				<
					method_has_required_options<false, class_generating::util::member_specification<Options...>, volatile_qualifier>,
					std::add_volatile_t<This>,
					This
				>
			>;
		};

		template <typename This, typename Options> struct add_ref_qualifier;
		template <typename This, class_generating::util::fixed_string ...Options>
		struct add_ref_qualifier<This, method_options<Options...>>
		{
			using type = std::conditional_t
			<
				method_has_required_options<false, class_generating::util::member_specification<Options...>, lvalue_specifier>,
				std::add_lvalue_reference_t<This>,
				std::conditional_t
				<
					method_has_required_options<false, class_generating::util::member_specification<Options...>, rvalue_specifier>,
					std::add_rvalue_reference_t<This>,
					This
				>
			>;
		};

		template <typename This, typename Options> using add_cv_ref_qualifiers_t = typename
			add_ref_qualifier<typename add_cv_qualifier<This, Options>::type, Options>::type;

		template <typename This, class_generating::util::fixed_string Name, typename Signature, typename Options>
		class method
			: public method_implementation<add_cv_ref_qualifiers_t<This, Options>, Name, Signature, Options>
		{
		public:
			template <typename T>
			constexpr method(T&& t) : method_implementation<add_cv_ref_qualifiers_t<This, Options>, Name, Signature, Options>{std::forward<T>(t)} {}
		};
	}

	namespace generate_member
	{
		template <typename This, util::fixed_string Name, typename Signature, typename Options>
		class generator<This, method<Name, Signature, Options>>
		{
		public:
			using type = generated_member<methods::util::method<This, Name, Signature, Options>>;
		};
	}

	namespace reflection
	{
		template <util::fixed_string Name, typename Signature, util::fixed_string ...Options, util::fixed_string ...RequiredOptions, bool Only>
		struct has_required_options<method<Name, Signature, method_options<Options...>>, util::member_specification<RequiredOptions...>, Only>
			: has_required_options<util::member_specification<Options...>, util::member_specification<RequiredOptions...>, Only>
		{};
		template <util::fixed_string Name, typename Signature, util::fixed_string ...Options, util::fixed_string ...RequiredOptions>
		struct has_not_required_options<method<Name, Signature, method_options<Options...>>, util::member_specification<RequiredOptions...>>
			: has_not_required_options<util::member_specification<Options...>, util::member_specification<RequiredOptions...>>
		{};
		template <util::fixed_string Name, typename Signature, typename Options>
		struct get_name<method<Name, Signature, Options>>
		{
			static constexpr auto value = Name;
		};
	}
}
