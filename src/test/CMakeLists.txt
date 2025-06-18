target_sources(lib2 PUBLIC
  FILE_SET lib2_test_modules TYPE CXX_MODULES FILES
	assert_exception.ixx
	assert.ixx
	test_result.ixx
	test_case.ixx
	test_suite.ixx
  	test.ixx
)