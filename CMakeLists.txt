cmake_minimum_required(VERSION 3.9)
project(bankbulls)

set(EOSIO_CDT_INSTALL_DIR /usr/local/eosio.cdt )
include_directories(include)
include_directories(/usr/local/eosio.cdt/include/eosiolib/contracts)
include_directories(/usr/local/eosio.cdt/include/eosiolib/core)
include_directories(${EOSIO_CDT_INSTALL_DIR}/include)
include_directories(${EOSIO_CDT_INSTALL_DIR}/include/libc)
include_directories(${EOSIO_CDT_INSTALL_DIR}/include/libcxx)

set(COMMON_FILES /usr/local/eosio.cdt)
set(HPP_FILES )
set(CPP_FILES src/bankbulls.cpp)


add_library(link SHARED
       ${COMMON_FILES}
        ${CPP_FILES}
        ${HPP_JSON}
        ${HPP_FILES})

target_link_libraries(link
       ${EOSIO_CDT_INSTALL_DIR}/lib/libc.a
       ${EOSIO_CDT_INSTALL_DIR}/lib/libc++.a
      ${EOSIO_CDT_INSTALL_DIR}/lib/eosiolib.a)