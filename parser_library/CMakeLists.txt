cmake_minimum_required (VERSION 3.10)
INCLUDE(GenerateExportHeader)
include(GoogleTest)
project(parser_library)

SET (CMAKE_CXX_STANDARD 17)


# set ANTLR jar location
set(ANTLR_JAR_LOCATION 
	${ANTLR4CPP_EXTERNAL_ROOT}/src/antlr4cpp/tool/target/antlr4-4.7.1-complete.jar)

# generated grammar source files
set(GENERATED_SRC
   ${PROJECT_SOURCE_DIR}/src/generated/hlasmparser.cpp
   ${PROJECT_SOURCE_DIR}/src/generated/hlasmparserBaseListener.cpp
   ${PROJECT_SOURCE_DIR}/src/generated/hlasmparserBaseVisitor.cpp
   ${PROJECT_SOURCE_DIR}/src/generated/hlasmparserListener.cpp
   ${PROJECT_SOURCE_DIR}/src/generated/hlasmparserVisitor.cpp
 )

foreach( src_file ${GENERATED_SRC} )
      set_source_files_properties(
          ${src_file}
          PROPERTIES
          GENERATED TRUE
          )
endforeach( src_file ${GENERATED_SRC} )

add_custom_target(GenerateParser DEPENDS ${GENERATED_SRC})
add_custom_command(OUTPUT ${GENERATED_SRC}
   COMMAND 
   ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/src/generated/
   COMMAND
   "${Java_JAVA_EXECUTABLE}" -jar ${ANTLR_JAR_LOCATION} -Werror -Dlanguage=Cpp -lib ${PROJECT_SOURCE_DIR}/src/grammar/ -listener -visitor -o ${PROJECT_SOURCE_DIR}/src/generated/ -package hlasm_plugin::parser_library::generated hlasmparser.g4
   WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/src/grammar/"
   DEPENDS antlr4jar ${PROJECT_SOURCE_DIR}/src/grammar/hlasmparser.g4 ${PROJECT_SOURCE_DIR}/src/grammar/lex.tokens
   )

set(LIB_NOT_EXPORTED_SRC    
    ${PROJECT_SOURCE_DIR}/src/context/common_types.cpp
    ${PROJECT_SOURCE_DIR}/src/context/hlasm_context.cpp
    ${PROJECT_SOURCE_DIR}/src/context/macro.cpp
    ${PROJECT_SOURCE_DIR}/src/context/macro_param_data.cpp
    ${PROJECT_SOURCE_DIR}/src/context/variable.cpp
	${PROJECT_SOURCE_DIR}/src/parser_tools.cpp
    ${PROJECT_SOURCE_DIR}/src/context/keyword_expression.cpp
    ${PROJECT_SOURCE_DIR}/src/context/arithmetic_expression.cpp
    ${PROJECT_SOURCE_DIR}/src/context/logic_expression.cpp
    ${PROJECT_SOURCE_DIR}/src/context/character_expression.cpp
    ${PROJECT_SOURCE_DIR}/src/context/ebcdic_encoding.cpp
    ${PROJECT_SOURCE_DIR}/src/context/expression.cpp
    ${PROJECT_SOURCE_DIR}/src/workspace.cpp
    ${PROJECT_SOURCE_DIR}/src/file_manager_impl.cpp
    ${PROJECT_SOURCE_DIR}/src/file_impl.cpp
    ${PROJECT_SOURCE_DIR}/src/library.cpp
)

#Generate the shared library from the library sources
add_library(parser_library SHARED
    ${PROJECT_SOURCE_DIR}/src/parser_library.cpp
    ${PROJECT_SOURCE_DIR}/src/lexer.cpp
    ${PROJECT_SOURCE_DIR}/src/token.cpp
    ${PROJECT_SOURCE_DIR}/src/token_factory.cpp
    ${PROJECT_SOURCE_DIR}/src/input_source.cpp
    ${PROJECT_SOURCE_DIR}/src/workspace_manager.cpp
	${LIB_NOT_EXPORTED_SRC}
    ${GENERATED_SRC}
)

generate_export_header(parser_library
			EXPORT_FILE_NAME ${PROJECT_SOURCE_DIR}/src/generated/parser_library_export.h)

target_link_libraries(parser_library antlr4-runtime)

target_include_directories(parser_library
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
		${PROJECT_SOURCE_DIR}/src/generated/
)

ADD_CUSTOM_COMMAND (TARGET parser_library POST_BUILD      # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_directory  # which executes "cmake - E copy_if_different..."
        "${ANTLR4CPP_LIBS}"      # <--this is in-file
        "${PROJECT_BINARY_DIR}/../bin/" )  

add_dependencies(parser_library antlr4jar GenerateParser)
add_dependencies(parser_library jsonrpcppext)



if(BUILD_TESTING)
	add_executable(library_test test/test.cpp
    ${LIB_NOT_EXPORTED_SRC}
	${GENERATED_SRC})
	add_custom_target(library_tests_copy
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${PROJECT_SOURCE_DIR}/test/res ${CMAKE_BINARY_DIR}/bin/test/library)

	target_link_libraries(library_test gmock_main)
	target_link_libraries(library_test antlr4-runtime)
	target_link_libraries(library_test parser_library)
	add_dependencies(library_test library_tests_copy)
	add_dependencies(library_test antlr4jar)
	add_dependencies(library_test parser_library)
    gtest_discover_tests(library_test
        TIMEOUT 10000
        DISCOVERY_TIMEOUT 10000)
endif()