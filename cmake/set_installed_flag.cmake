function(set_installed_flag)
	set(oneValueArgs INSTALLED_FILE SUBST)
	cmake_parse_arguments(PARSE_ARGV 0 arg
		"${options}" "${oneValueArgs}" "${multiValueArgs}"
	)
	
	cmake_path(GET arg_INSTALLED_FILE FILENAME INSTALLED_FILE_NAME)
    install(CODE "
		message(\"-- Setting installed flag on: ${arg_INSTALLED_FILE}\")
		file(MAKE_DIRECTORY \"${CMAKE_CURRENT_BINARY_DIR}/install_tmp\")
		file(SIZE \"${arg_INSTALLED_FILE}\" DATA_SIZE)
		execute_process(COMMAND sh -c
		\"LC_ALL=C sed '${arg_SUBST}' < '${arg_INSTALLED_FILE}' | head -c \${DATA_SIZE} /dev/stdin > '${CMAKE_CURRENT_BINARY_DIR}/install_tmp/${INSTALLED_FILE_NAME}'\")
		file(RENAME \"${CMAKE_CURRENT_BINARY_DIR}/install_tmp/${INSTALLED_FILE_NAME}\" \"${arg_INSTALLED_FILE}\")
	")
endfunction()