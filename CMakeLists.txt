cmake_minimum_required(VERSION 3.2)

project("Class Generating" VERSION 1.0)

set(features "events")

foreach(feature IN LISTS features)
	add_executable("${feature}" "test/${feature}.cpp")
	set_target_properties("${feature}" PROPERTIES OUTPUT_NAME "${feature}")
	target_include_directories("${feature}" PUBLIC "include")
	set_property(TARGET "${feature}" PROPERTY CXX_STANDARD 23)
endforeach()
