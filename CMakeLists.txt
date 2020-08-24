## add_contract(bankbulls  ${CMAKE_CURRENT_SOURCE_DIR}/src/bankbulls.cpp)

target_include_directories(bankbulls
   PUBLIC
   ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(bankbulls
   PROPERTIES
   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")