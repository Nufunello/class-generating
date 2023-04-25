#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <functional>

#include "class_generating/generate_class.hpp"
#include "class_generating/event.hpp"

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
			Person,
			class_generating::event<"birthday", std::size_t, std::vector, std::back_insert_iterator>,
			class_generating::event<"death", std::size_t, std::vector, std::back_insert_iterator>
		>
	{
		std::size_t years;
		std::size_t maxYears;
	public:
		using generate_class::operator();

		Person()
			: generate_class
				{
					construct_member{class_generating::access_member_by_name<"birthday">, 2, &onBirthday},
					construct_member{class_generating::access_member_by_name<"death">, 1, &onDie},
				}
			, years{0}
			, maxYears{ rand() % MAX_YEARS_MAX }
		{}

		void operator()()
		{
			if (years < maxYears)
			{
				find_member_type_t<class_generating::tags::name<"birthday">>::operator()(++years);
			}
			else
			{
				if (years == maxYears)
				{
					find_member_type_t<class_generating::tags::name<"death">>::operator()(++years);
				}
			}
		}

	};
}

int main()
{
	srand((unsigned) time(NULL));
	auto person = Person{};
	person(class_generating::access_member_by_name<"birthday">)(onBirthday);
	person(class_generating::access_member_by_name<"death">)(onDie);
	for (std::size_t years = 0; years < MAX_YEARS_MAX; ++years)
	{
		person();
	}
}
