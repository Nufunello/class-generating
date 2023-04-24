#pragma once

#include "class_generating/fixed_string.hpp"
#include "class_generating/tags.hpp"
#include "class_generating/generate_member.hpp"
#include "class_generating/reflection.hpp"

namespace class_generating
{
	template <util::fixed_string Name, typename Argument, template <typename> typename Container, template <typename> typename InsertIterator>
	struct event{};

	namespace events::util
	{
		template <class_generating::util::fixed_string/*used to resolve ambiguity*/, 
			typename Argument, template <typename> typename Container, template <typename> typename InsertIterator> 
		class event
		{
		public:
			using Callback = void (*)(Argument);
			constexpr void operator()(Callback callback, tags::tag<tags::name<"subscribe">> = {})
			{
				InsertIterator<Container<Callback>>{callbacks} = std::move(callback);
			}
			template <typename ...Arguments>
			constexpr event(Arguments&& ...arguments)
				: callbacks(std::forward<Arguments>(arguments)...)
			{}
		protected:
			//copying event directly is not something we want to support
			event(event&&) = default;
			event(const event&) = default;
			constexpr void operator()(Argument argument, tags::tag<tags::name<"notify">> = {}) const
			{
				for (const auto& callback : callbacks)
				{
					callback(argument);
				}
			}
		private:
			Container<Callback> callbacks;
		};
	}

	namespace generate_member
	{
		template <util::fixed_string Name, typename Argument, template <typename> typename Container, template <typename> typename InsertIterator>
		struct generator<event<Name, Argument, Container, InsertIterator>>
		{
		public:
			using type = generate_member::generated_member<events::util::event<Name, Argument, Container, InsertIterator>>;
		};
	}

	namespace reflection
	{
		template <util::fixed_string Name, typename Argument, template <typename> typename Container, template <typename> typename InsertIterator>
		struct get_name<event<Name, Argument, Container, InsertIterator>>
		{
			static constexpr auto value = Name;
		};
	}
}
