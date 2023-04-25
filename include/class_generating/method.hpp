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
		template <typename Options, class_generating::util::fixed_string ...RequiredOptions>
		static constexpr bool method_has_only_required_options =
			reflection::has_required_options_v<method<"", function_signature<void>, Options>, method_options<RequiredOptions...>, true>;

		template <typename This, class_generating::util::fixed_string Name, typename Signature, typename Options> class method_implementation;

		template <typename This, class_generating::util::fixed_string Name, typename Return, typename ...Args, typename Options> 
			requires (method_has_only_required_options<Options>)
		class method_implementation<This, Name, function_signature<Return, Args...>, Options>
		{
			Return (*code)(This&, Args...);
		protected:
			template <typename T>
			constexpr method_implementation(T&& t) : code{std::forward<T>(t)} {}
		public:
			Return operator()(Args ...args)
			{
				return code(static_cast<This&>(*this), args...);
			}
		};
		template <typename This, class_generating::util::fixed_string Name, typename Return, typename ...Args, typename Options>
			requires (method_has_only_required_options<Options, "const">)
		class method_implementation<This, Name, function_signature<Return, Args...>, Options>
		{
			Return (*code)(const This&, Args...);
		protected:
			template <typename T>
			constexpr method_implementation(T&& t) : code{std::forward<T>(t)} {}
		public:
			Return operator()(Args ...args) const
			{
				return code(static_cast<const This&>(*this), args...);
			}
		};
		template <typename This, class_generating::util::fixed_string Name, typename Return, typename ...Args, typename Options>
			requires (method_has_only_required_options<Options, "virtual">)
		class method_implementation<This, Name, function_signature<Return, Args...>, Options>
		{
			Return (*code)(This&, Args...);
		protected:
			template <typename T>
			constexpr method_implementation(T&& t) : code{std::forward<T>(t)} {}
		public:
			virtual Return operator()(Args ...args, tags::name<Name> = {})
			{
				return code(static_cast<This&>(*this), args...);
			}
		};

		template <typename This, class_generating::util::fixed_string Name, typename Signature, typename Options>
		class method
			: public method_implementation<This, Name, Signature, Options> 
		{
		public:
			template <typename T>
			constexpr method(T&& t) : method_implementation<This, Name, Signature, Options>{std::forward<T>(t)} {}
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
