
# ignore warnings
add_definitions(-w)

set(
    sources
    AccountManager.cpp AccountManager.h
    main.cpp
    MinerAux.cpp MinerAux.h
    ubre_callback.cpp ubre_callback.h
    cli_driver.cpp cli_driver.h
)

set(LIBUBRE_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/libubre")
include_directories(${LIBUBRE_INCLUDE_DIR})
include_directories(${LIBUBRE_INCLUDE_DIR}/lib/include)
link_directories(${LIBUBRE_INCLUDE_DIR}/lib/lib)

add_executable(aleth ${sources})
target_link_libraries(
    aleth
    PRIVATE ethereum ethashseal evm web3jsonrpc webthree devcore Boost::program_options nbre_rt glog
    ff_net pthread
)

add_executable(cli
  cli.cpp
  cli_driver.cpp cli_driver.h
  ubre_callback.cpp ubre_callback.h
  )
target_link_libraries(cli PRIVATE Boost::program_options ff_net pthread nbre_rt glog)

target_include_directories(aleth PRIVATE ../utils)

if(MINIUPNPC)
    target_compile_definitions(aleth PRIVATE ETH_MINIUPNPC)
endif()

install(TARGETS aleth DESTINATION bin)
