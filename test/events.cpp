#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <functional>

#include "class_generating/events.hpp"

namespace
{
	void onBirthday(std::size_t years)
	{
		std::cout << "Person is now " << years << std::endl;
	}

	void onDie(std::size_t years)
	{
		std::cout << "Person died at " << years << std::endl;
	}

	constexpr static std::size_t MAX_YEARS_MAX = 100;
	class Person
		: public class_generating::generate_class
		<
			class_generating::event<"birthday", std::size_t, std::vector, std::back_insert_iterator>,
			class_generating::event<"death", std::size_t, std::vector, std::back_insert_iterator>
		>
	{
		std::size_t years;
		std::size_t maxYears;
	public:
		Person()
			: construct_members
				{
					construct_member<"birthday">{2, &onBirthday},
					construct_member<"death">{1, &onDie},
				}
			, years{0}
			, maxYears{ rand() % MAX_YEARS_MAX }
		{}

		using generated_members::operator();

		void operator()()
		{
			if (years < maxYears)
			{
				access_member<"birthday">::operator()(class_generating::tag<"notify">, ++years);
			}
			else
			{
				if (years == maxYears)
				{
					access_member<"death">::operator()(class_generating::tag<"notify">, ++years);
				}
			}
		}

	};
}

int main()
{
	srand((unsigned) time(NULL));
	auto person = Person{};
	person(class_generating::access_member<"birthday">)(class_generating::tag<"subscribe">, onBirthday);
	person(class_generating::access_member<"death">)(class_generating::tag<"subscribe">, onDie);
	for (std::size_t years = 0; years < MAX_YEARS_MAX; ++years)
	{
		person();
	}
}
