# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\DescoDeco_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\DescoDeco_autogen.dir\\ParseCache.txt"
  "DescoDeco_autogen"
  )
endif()
