#pragma once

#include "chaos/preprocessor.h"

#include "class_generating/fixed_string.hpp"
#include "class_generating/tags.hpp"

#include "class_generating/generate_member.hpp"
#include "class_generating/member_specification.hpp"

#include "class_generating/reflection.hpp"
#include "class_generating/reflection/name.hpp"
#include "class_generating/reflection/required_options.hpp"

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
		template <typename RequiredOptions, class_generating::util::fixed_string ...Options>
		static constexpr bool method_has_only_required_options =
			reflection::has_required_options_v<
				method<"", function_signature<void>, method_options<Options...>>, 
				RequiredOptions,
				true>;

		template <typename This, class_generating::util::fixed_string Name, typename Signature, typename Options> class method_implementation;

#define VALUE_EMPTY(value) CHAOS_PP_ARRAY_IS_NIL(value)
#define VALUE_NOT_EMPTY(_, value, __) CHAOS_PP_NOT(VALUE_EMPTY(value)) 
#define GET_VALUE(value) CHAOS_PP_IF(VALUE_EMPTY(value))( , CHAOS_PP_SEQ_CONCAT(CHAOS_PP_ARRAY_TO_SEQ(value)))
#define STRINGIZE(s, value, _) CHAOS_PP_STRINGIZE(GET_VALUE(value))
#define GET_REFERENCE_OR_DEFAULT(value) CHAOS_PP_IF(VALUE_NOT_EMPTY(,value,))(GET_VALUE(value), &)

#define METHOD_IMPLEMENTATION(QUALIFIERS)\
		template <typename This, class_generating::util::fixed_string Name, typename Return, typename ...Args,\
			class_generating::util::fixed_string ...Options>\
			requires (method_has_only_required_options\
				<\
					class_generating::util::member_specification\
					<\
						CHAOS_PP_SEQ_ENUMERATE(CHAOS_PP_SEQ_TRANSFORM(STRINGIZE, CHAOS_PP_SEQ_FILTER(VALUE_NOT_EMPTY, QUALIFIERS, ), ))\
					>,\
					Options...\
				>)\
		class method_implementation<This, Name, function_signature<Return, Args...>, method_options<Options...>>\
		{\
CHAOS_PP_IF(VALUE_EMPTY(CHAOS_PP_SEQ_ELEM(4, QUALIFIERS)))\
			(Return (*code)(\
				GET_VALUE(CHAOS_PP_SEQ_ELEM(1, QUALIFIERS)) GET_VALUE(CHAOS_PP_SEQ_ELEM(2, QUALIFIERS))/*cv*/\
				This GET_REFERENCE_OR_DEFAULT(CHAOS_PP_SEQ_ELEM(3, QUALIFIERS)), Args...);\
			, CHAOS_PP_EMPTY())\
		protected:\
			CHAOS_PP_IF(VALUE_EMPTY(CHAOS_PP_SEQ_ELEM(4, QUALIFIERS)))\
			(template <typename ...T> constexpr method_implementation(T&& ...args): code{std::forward<T>(args)...} {}\
			, method_implementation() = default;)\
		public:\
			GET_VALUE(CHAOS_PP_SEQ_ELEM(0, QUALIFIERS)) Return operator()(Args ...args, tags::name<Name> = {})\
				GET_VALUE(CHAOS_PP_SEQ_ELEM(1, QUALIFIERS)) GET_VALUE(CHAOS_PP_SEQ_ELEM(2, QUALIFIERS))/*cv*/\
				GET_VALUE(CHAOS_PP_SEQ_ELEM(3, QUALIFIERS))/*reference*/\
				GET_VALUE(CHAOS_PP_SEQ_ELEM(4, QUALIFIERS))/*pure virtual*/\
CHAOS_PP_IF(VALUE_EMPTY(CHAOS_PP_SEQ_ELEM(4, QUALIFIERS)))\
			({\
				return code(static_cast<\
					GET_VALUE(CHAOS_PP_SEQ_ELEM(1, QUALIFIERS)) GET_VALUE(CHAOS_PP_SEQ_ELEM(2, QUALIFIERS))/*cv*/\
					This GET_REFERENCE_OR_DEFAULT(CHAOS_PP_SEQ_ELEM(3, QUALIFIERS))>(*this), std::forward<Args>(args)...);\
			}, ;)\
		};

#define FOR_METHOD_IMPLEMENTATION(s, elem, data) METHOD_IMPLEMENTATION(CHAOS_PP_SEQ_INSERT(CHAOS_PP_SEQ_SIZE(data), data, elem))
#define FOR_PURE_VIRTUAL(s, elem, data) CHAOS_PP_EXPR_S(s)(CHAOS_PP_SEQ_FOR_EACH_S(s, FOR_METHOD_IMPLEMENTATION, (CHAOS_PP_ARRAY_NIL())((1, (= 0))), CHAOS_PP_SEQ_INSERT(CHAOS_PP_SEQ_SIZE(data), data, elem)))
#define FOR_REFERENCE(s, elem, data) CHAOS_PP_EXPR_S(s)(CHAOS_PP_SEQ_FOR_EACH_S(s, FOR_PURE_VIRTUAL, (CHAOS_PP_ARRAY_NIL())((1, (&)))((1, (&&))), CHAOS_PP_SEQ_INSERT(CHAOS_PP_SEQ_SIZE(data), data, elem)))
#define FOR_VOLATILE(s, elem, data) CHAOS_PP_EXPR_S(s)(CHAOS_PP_SEQ_FOR_EACH_S(s, FOR_REFERENCE, (CHAOS_PP_ARRAY_NIL())((1, (volatile))), CHAOS_PP_SEQ_INSERT(CHAOS_PP_SEQ_SIZE(data), data, elem)))
#define FOR_CONST(s, elem, data) CHAOS_PP_EXPR_S(s)(CHAOS_PP_SEQ_FOR_EACH_S(s, FOR_VOLATILE, (CHAOS_PP_ARRAY_NIL())((1, (const))), (elem)))
		CHAOS_PP_EXPR(CHAOS_PP_SEQ_FOR_EACH(FOR_CONST, (CHAOS_PP_ARRAY_NIL())((1, (virtual))), ))

#undef VALUE_EMPTY
#undef VALUE_NOT_EMPTY
#undef GET_VALUE
#undef STRINGIZE
#undef GET_REFERENCE_OR_DEFAULT
#undef METHOD_IMPLEMENTATION
#undef FOR_METHOD_IMPLEMENTATION
#undef FOR_PURE_VIRTUAL
#undef FOR_REFERENCE
#undef FOR_VOLATILE
#undef FOR_CONST

		template <typename This, class_generating::util::fixed_string Name, typename Signature, typename Options>
		class method
			: public method_implementation<This, Name, Signature, Options>
		{
		public:
			template <typename ...Args>
			method(Args&& ...args) : method_implementation<This, Name, Signature, Options>{std::forward<Args>(args)...} {}
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
