cmake_minimum_required(VERSION 3.3)

project( endhiran VERSION 1.0 LANGUAGES CXX)

enable_testing()

set (CMAKE_CXX_STANDARD 14)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(Boost REQUIRED)
find_package(Eigen3 REQUIRED)

set (EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build)
set (LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})
set(LIBRARY_NAME endhiran)

if ( Boost_FOUND AND EIGEN3_FOUND )

  include_directories(${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})
  include_directories(${EIGEN3_INCLUDE_DIR})

  include_directories(include)
  include_directories(src)


  file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
  file(GLOB_RECURSE INC_ALL ${PROJECT_SOURCE_DIR}/src/*.h)

  add_library (${LIBRARY_NAME} SHARED ${SRC_FILES} ${INC_ALL})
  add_executable (${LIBRARY_NAME}_uci src/uci_main.cxx ${SRC_FILES} ${INC_ALL})
  add_executable (${LIBRARY_NAME}_cli src/cli_main.cxx ${SRC_FILES} ${INC_ALL})
  add_executable (${LIBRARY_NAME}_perft src/perft_main.cxx ${SRC_FILES} ${INC_ALL})

  target_link_libraries (${LIBRARY_NAME}_uci  ${Boost_LIBRARIES} Eigen3::Eigen)
  target_link_libraries (${LIBRARY_NAME}_cli  ${Boost_LIBRARIES} Eigen3::Eigen)
  target_link_libraries (${LIBRARY_NAME}_perft  ${Boost_LIBRARIES} Eigen3::Eigen)


  target_include_directories(${LIBRARY_NAME}
      PUBLIC
          $<INSTALL_INTERFACE:include>
          $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      PRIVATE
          ${PROJECT_SOURCE_DIR}/src
  )

  add_subdirectory(python)

  ## install
  include(GNUInstallDirs)

  set(EXPORT_NAME endhiran)

  set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${EXPORT_NAME})


  set_target_properties(${LIBRARY_NAME} PROPERTIES EXPORT_NAME ${EXPORT_NAME})

  install(TARGETS ${LIBRARY_NAME}
          EXPORT ${EXPORT_NAME}Targets
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          )


  install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

  install(EXPORT ${EXPORT_NAME}Targets
          FILE ${EXPORT_NAME}Targets.cmake
          DESTINATION ${INSTALL_CONFIGDIR}
           )

  include(CMakePackageConfigHelpers)
  write_basic_package_version_file(
     ${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_NAME}ConfigVersion.cmake
     VERSION ${PROJECT_VERSION}
     COMPATIBILITY AnyNewerVersion
     )

   configure_package_config_file(
       ${CMAKE_CURRENT_LIST_DIR}/cmake/${EXPORT_NAME}Config.cmake.in
       ${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_NAME}Config.cmake
       INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
   )

  install(
      FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_NAME}ConfigVersion.cmake
      DESTINATION ${INSTALL_CONFIGDIR}
  )

  export(EXPORT ${EXPORT_NAME}Targets FILE ${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_NAME}Targets.cmake)
  export(PACKAGE ${EXPORT_NAME})


  # Test cases
  add_subdirectory(test)

else()

    message(STATUS "This program requires Boost and eigen libraries, and will not be compiled.")

endif()
