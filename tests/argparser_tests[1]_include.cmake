if(EXISTS "/Users/hom1c1d3/github-classroom/is-itmo-c-23/labwork4-antoha124/tests/argparser_tests[1]_tests.cmake")
  include("/Users/hom1c1d3/github-classroom/is-itmo-c-23/labwork4-antoha124/tests/argparser_tests[1]_tests.cmake")
else()
  add_test(argparser_tests_NOT_BUILT argparser_tests_NOT_BUILT)
endif()
