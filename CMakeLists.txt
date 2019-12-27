cmake_minimum_required(VERSION 3.7.0)

project(CPP-CAN-Parser)


include(CTest)

set(CPPPARSER_INCLUDE_DIRECTORY 
	${CMAKE_CURRENT_LIST_DIR}/include
)

set(CPPPARSER_SRC_FILES
	src/models/CANDatabase.cpp
	src/models/CANFrame.cpp
  src/models/CANSignal.cpp
	src/parsing/DBCParser.cpp
	src/parsing/Tokenizer.cpp)

add_library(cpp_can_parser STATIC ${CPPPARSER_SRC_FILES})
target_include_directories(cpp_can_parser 
	PUBLIC ${CPPPARSER_INCLUDE_DIRECTORY})

add_executable(dbcLoadFileExec 
	tests/dbcLoadFileTest.cpp)
target_link_libraries(dbcLoadFileExec cpp_can_parser)

add_test(NAME dbcLoadFileTest 
		 COMMAND dbcLoadFileExec)