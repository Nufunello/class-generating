# class-generating - header only library

Generates class interface. May be used to produce classes (meta class)

Currently implemented features:
  * Event (class_generating::event)
  * Method (class_generating::method)
  
  
## Event

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


## Method

Method has Name, Signature(Return type and arguments), Options(member specification - array of C-style strings)

Options work as conditional noexcept for other specification(only): virtual, const, volatile, &, &&, pure specifier(= 0)

Neccessary includes
``` C++
#include "class_generating/generate_class.hpp" //include class that generates interface from feature's template
#include "class_generating/method.hpp" //include feature related stuff, like template
```

``` C++
class Functions : public class_generating::generate_class
<
	Functions, //generate_class's generator uses CRTP pattern 
	class_generating::method<"foo", class_generating::function_signature<void, int>, class_generating::method_options<>>, //method foo
	class_generating::method<"foo", class_generating::function_signature<void, std::string>, class_generating::method_options<"const">> //const version of "foo"
>
{
```

Constructing methods with lambdas
``` C++
Functions() : generate_class
{
	construct_member //constructing non const version
	(
		class_generating::tags::tags
		<
			class_generating::tags::name<"foo">, //specifying name
			class_generating::tags::excludes_member_specification<"const"> //specifying non-const 
		>{},
		[](Functions& functions, int value)//implementation should be function pointer or lambda with no lambda-capture
		{ 
			std::cout << "Non-const function" << std::endl
			<< value << std::endl;
		}
	),
	construct_member
	(
		class_generating::tags::tags
		<
			class_generating::tags::name<"foo">,
			class_generating::tags::includes_member_specification<false, "const"> //specifying const
		>{},
		[](const Functions& functions, std::string value) 
		{ 
			std::cout << "Const function" << std::endl
			<< value << std::endl;
		}
	)
```

Calling methods
``` C++
auto functions = Functions{};
functions(class_generating::access_member_by_name<"foo">)(1);//non const will be called
const auto& cFunctions = functions;
cFunctions(class_generating::access_member_by_name<"foo">)("2");//const will be called
```

Pure virtual function (abstract)

``` C++
class PureVirtualFunction : public class_generating::generate_class
<
	PureVirtualFunction,
	class_generating::method<"foo", class_generating::function_signature<int, int>, class_generating::method_options<"virtual", "= 0">>
>
{};

class PureVirtualFunctionOverride : public PureVirtualFunction
{
public:
	PureVirtualFunctionOverride() = default;
	using generate_class::operator();
	int operator()(int value, 
		class_generating::tags::name<"foo"> = {}//specifying function name to override
		) override 
	{
		return value * 4;
	}
};

PureVirtualFunctionOverride vfo;
PureVirtualFunction& vf = vfo;

std::cout << "Expects 4, actual " << vf(class_generating::access_member_by_name<"foo">)(1) << std::endl;
std::cout << "Expects 8, actual " << vfo(class_generating::access_member_by_name<"foo">)(2) << std::endl;
```
