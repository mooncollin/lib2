add_library(lib2_scan OBJECT)

target_sources(lib2_scan
	PUBLIC FILE_SET CXX_MODULES FILES
		scan_error.ixx
		scanner.ixx
		basic_scan_parse_context.ixx
		basic_scan_arg.ixx
		basic_scan_context.ixx
		scan.ixx
)

target_link_libraries(lib2_scan PUBLIC
	lib2_concepts
	lib2_utility
	lib2_iterator
	lib2_string
)

install(TARGETS lib2_scan
    EXPORT lib2Targets
    FILE_SET CXX_MODULES DESTINATION ${INCLUDE_INSTALL_DIR}/scan
    FILE_SET CXX_MODULES DESTINATION ${INCLUDE_INSTALL_DIR}/scan
)