# class-generating

Generates class interface.

Currently implemented features:
  * Event (class_generating::event)
  * Method (class_generating::method)
  
  
### Event

Event has constructor and two methods:

Constructor forwards arguments to Callback Container constructor

``` C++
using Callback = void (*)(Argument);
constexpr void operator()(Callback callback, tags::tags<tags::name<"subscribe">> = {})
constexpr void operator()(Argument argument, tags::tags<tags::name<"notify">> = {}) const
```

Neccessary includes
``` C++
#include "class_generating/generate_class.hpp" //include class that generates interface from feature's template
#include "class_generating/event.hpp" //include feature related stuff, like template
```

Generating of class with interface
 
``` C++
class Person
	: public class_generating::generate_class
	<
		Person, //generate_class's generator uses CRTP pattern 
		class_generating::event<"birthday", std::size_t, std::vector, std::back_insert_iterator>, //event named "birthday"
		class_generating::event<"death", std::size_t, std::vector, std::back_insert_iterator> //event named "death"
	>
{
public:
	using generate_class::operator();//in case Person class has invoke operator
```

Constructing event
``` C++
Person() 
  : generate_class{ construct_member //accessing constructor of event
    ( class_generating::access_member_by_name<"birthday">, //get member(in this case event) by name,
                                                          //in features like "method" we may use other tags to resolve ambiguity
      2, &onBirthday)//"2" and callback - arguments for callback container(in this case std::vector)
		}
```
  
Event "Notify" workflow
``` C++
if (years < maxYears)
{
	using Event = find_member_type_t<class_generating::tags::name<"birthday">>;//getting type of generated event by name,
                                                              //since, we are inherited from it we accessing it method using type
  Event::operator()(++years, //event argument
      class_generating::access_member_by_name<"notify"> //specifying that it should be notify
                                                        //if callback type is different from argument specifying is not required
    );
}
```
 
Event "Subscribe" workflow
``` C++
auto person = Person{};
auto& event = person(class_generating::access_member_by_name<"birthday">);//geting person event by invoke operator
event(onBirthday, class_generating::access_member_by_name<"subscribe">);//subscribe on event with "onBirthday" callback
```
