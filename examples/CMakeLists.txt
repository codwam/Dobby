add_executable(socket_example
  main.cc
  socket_example.cc
  )

target_link_libraries(socket_example
  dobby
  logging
  )


add_library(socket_example_lib SHARED
  socket_example.cc
  )

target_link_libraries(socket_example_lib
  dobby
  )

# ---
if(NOT TARGET_NAME)
  set(TARGET_NAME "feature_example")
  # message(STATUS "Using default target: ${TARGET_NAME}")
endif(NOT TARGET_NAME)

# set(executable_name ${TARGET_NAME})
# message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
set(BIN_PATH "${CMAKE_BINARY_DIR}/build/Debug")
set(DEVELOPMENT_TEAM_ID "L8W42QLGJ8") # test

add_executable(${TARGET_NAME}
  feature_example.cc
  )

target_link_libraries(${TARGET_NAME}
  dobby
  logging
  )

# RUNTIME ERROR: The application bundle does not contain a valid identifier. It appears when xcode(14.3.1) sets the package name.
# Solution: Empty enter OK and re-enter to compile.
set_xcode_property(${TARGET_NAME} PRODUCT_BUNDLE_IDENTIFIER "com.codwam.test" "All")
set_xcode_property(${TARGET_NAME} LD_RUNPATH_SEARCH_PATHS "@executable_path/Frameworks ${BIN_PATH}" "All") # ‘$(inherited)’ add automatically
set_xcode_property(${TARGET_NAME} CODE_SIGN_IDENTITY "iPhone Developer" "All")
set_xcode_property(${TARGET_NAME} DEVELOPMENT_TEAM ${DEVELOPMENT_TEAM_ID} "All")
# dependency handle
set_xcode_property("dobby" CODE_SIGN_IDENTITY "iPhone Developer" "All")
set_xcode_property("dobby" DEVELOPMENT_TEAM ${DEVELOPMENT_TEAM_ID} "All")
set_target_properties(${TARGET_NAME} PROPERTIES
    XCODE_EMBED_FRAMEWORKS dobby
    XCODE_EMBED_FRAMEWORKS_CODE_SIGN_ON_COPY NO
)
