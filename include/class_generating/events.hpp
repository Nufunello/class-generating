#pragma once

#include "class_generating/generate_class.hpp"

namespace class_generating
{
	template <util::fixed_string name, typename Argument, template <typename> typename Container, template <typename> typename InsertIterator>
	struct event {};

	namespace events::util
	{
		template <class_generating::util::fixed_string/*used to resolve ambiguity*/, 
			typename Argument, template <typename> typename Container, template <typename> typename InsertIterator> 
		class event
		{
		public:
			using Callback = void (*)(Argument);
			constexpr void operator()(class_generating::util::tag<"subscribe">, Callback callback)
			{
				InsertIterator<Container<Callback>>{callbacks} = std::move(callback);
			}
			template <typename ...Arguments>
			constexpr event(Arguments&& ...arguments)
				: callbacks(std::forward<Arguments>(arguments)...)
			{}
			~event() = default;
		protected:
			constexpr void operator()(class_generating::util::tag<"notify">, Argument argument) const
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

	namespace util
	{
		template <util::fixed_string name, typename Argument, template <typename> typename Container, template <typename> typename InsertIterator>
		class generate_member<event<name, Argument, Container, InsertIterator>>
		{
		public:
			using generate = class_generating::util::generated_member<name, events::util::event<name, Argument, Container, InsertIterator>>;
		};
	}

}
