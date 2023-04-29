cmake_minimum_required(VERSION 3.2)

project("Class Generating" VERSION 1.0)

set(features "events" "methods")

foreach(feature IN LISTS features)
	add_executable("${feature}" "test/${feature}.cpp")
	set_target_properties("${feature}" PROPERTIES OUTPUT_NAME "${feature}")
	target_include_directories("${feature}" PUBLIC "third-party/chaos-pp")
	target_include_directories("${feature}" PUBLIC "include")
	target_compile_features("${feature}" PUBLIC cxx_std_20)
endforeach()
