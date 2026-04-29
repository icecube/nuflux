cmake_minimum_required(VERSION 3.30)

cmake_policy(SET CMP0167 NEW)
if(POLICY CMP0177)
	cmake_policy(SET CMP0177 NEW)
endif()

message("CMake version: ${CMAKE_VERSION}")
message("CMake build directory: ${CMAKE_BINARY_DIR}")

find_package(Git)
if(GIT_EXECUTABLE)
	execute_process(
		COMMAND ${GIT_EXECUTABLE} describe --tags --dirty --always
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		OUTPUT_VARIABLE GIT_PROJECT_VERSION
		OUTPUT_STRIP_TRAILING_WHITESPACE
		ERROR_QUIET
	)
	if(GIT_PROJECT_VERSION)
		set(NUFLUX_VERSION "${GIT_PROJECT_VERSION}")
	endif(GIT_PROJECT_VERSION)
endif(GIT_EXECUTABLE)

if(NOT NUFLUX_VERSION)
	set(NUFLUX_VERSION "0.0.0")
endif(NOT NUFLUX_VERSION)

string(REGEX REPLACE "^v?([0-9]+(\\.[0-9]+)?(\\.[0-9]+)?)(-.*)?" "\\1" NUFLUX_NUMERIC_VERSION "${NUFLUX_VERSION}")
string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.([0-9]+)"
       NUFLUX_NUMERIC_VERSION_DECOMP ${NUFLUX_NUMERIC_VERSION})
set(NUFLUX_VERSION_MAJOR ${CMAKE_MATCH_1})
set(NUFLUX_VERSION_MINOR ${CMAKE_MATCH_2})
set(NUFLUX_VERSION_PATCH ${CMAKE_MATCH_3})
message("Package version: ${NUFLUX_VERSION} (${NUFLUX_NUMERIC_VERSION})")

project(Nuflux LANGUAGES CXX VERSION "${NUFLUX_NUMERIC_VERSION}")

set(CMAKE_CXX_STANDARD 17)

#option(DATA_PATH "Override the directory path to which data files are installed, instead of the default of $PREFIX/share/nuflux")
option(INSTALL_DATA "If off/false, data files will not be installed" ON)
option(PYTHON_BINDINGS "Build bindings for python" ON)
option(PYTHON_ONLY "Install only the python module. The resulting installation is not usable from any other language." OFF)

find_package(Boost COMPONENTS headers REQUIRED)
find_package(Photospline)
if(Photospline_FOUND)
	message("-- Found photospline; will build spline-based models")
else(Photospline_FOUND)
	message("-- Did not find photospline; will not build spline-based models")
endif(Photospline_FOUND)

include(GNUInstallDirs)

set(BUILD_TIME_DATA_PATH "${CMAKE_SOURCE_DIR}/resources/data")

# Figure out where data will be installed.
# This is used during the installation step, but we also want to know to bake it into the code.
if(PYTHON_ONLY)
	# set NUFLUX_DATA_INSTALL_DIR later
	set(INSTALLED_DATA_PATH "getBasePathRuntime()")
else(PYTHON_ONLY)
	set(NUFLUX_DATA_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/nuflux")
	set(INSTALLED_DATA_PATH "\"${NUFLUX_DATA_INSTALL_DIR}/data\"")
endif(PYTHON_ONLY)

configure_file("${CMAKE_SOURCE_DIR}/include/nuflux/version.h.in" "${CMAKE_BINARY_DIR}/version.h")

set(INSTALL_FLAG_PREFIX "NUFLUX_INSTALL_FLAG_PREFIX_")
set(INSTALL_FLAG_SUFFIX "_NUFLUX_INSTALL_FLAG_SUFFIX")
set(INSTALL_FLAG_OFF "A")
set(INSTALL_FLAG_ON "B")
configure_file("${CMAKE_SOURCE_DIR}/src/nuflux/detail.cpp" "${CMAKE_BINARY_DIR}/detail.cpp")

set(LIBRARY_SOURCES
	"${CMAKE_SOURCE_DIR}/src/nuflux/FluxFunction.cpp"
	"${CMAKE_SOURCE_DIR}/src/nuflux/IPLEFlux.cpp"
	"${CMAKE_SOURCE_DIR}/src/nuflux/LegacyConventionalFlux.cpp"
	"${CMAKE_SOURCE_DIR}/src/nuflux/LegacyPromptFlux.cpp"
	"${CMAKE_BINARY_DIR}/detail.cpp"
)
if(Photospline_FOUND)
	list(APPEND LIBRARY_SOURCES "${CMAKE_SOURCE_DIR}/src/nuflux/ANFlux.cpp")
	list(APPEND LIBRARY_SOURCES "${CMAKE_SOURCE_DIR}/src/nuflux/SplineFlux.cpp")
	list(APPEND LIBRARY_SOURCES "${CMAKE_SOURCE_DIR}/src/nuflux/SplineFlux2.cpp")
endif(Photospline_FOUND)

if(PYTHON_ONLY)
	list(APPEND LIBRARY_SOURCES "${CMAKE_SOURCE_DIR}/src/pybindings/detail.cxx")
	message("-- Python-only build enabled")
	if(NOT PYTHON_BINDINGS)
		message("-- Python bindings are required for python-module-only build, overriding to enable")
	endif(NOT PYTHON_BINDINGS)
	set(PYTHON_BINDINGS ON)
endif(PYTHON_ONLY)

add_library(nuflux_lib OBJECT ${LIBRARY_SOURCES})
set_property(TARGET nuflux_lib PROPERTY POSITION_INDEPENDENT_CODE 1)
target_include_directories(nuflux_lib BEFORE PRIVATE
                           $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
                           $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
)
target_include_directories(nuflux_lib PUBLIC $<INSTALL_INTERFACE:include>)
target_compile_definitions(nuflux_lib PRIVATE
	NUFLUX_VERSION="${NUFLUX_VERSION}"
	DATA_DIR="${CMAKE_SOURCE_DIR}/resources/data"
)
target_link_libraries(nuflux_lib PUBLIC Boost::headers)

if(NOT PYTHON_ONLY)
	add_library(nuflux_static STATIC $<TARGET_OBJECTS:nuflux_lib>)
	add_library(nuflux_shared SHARED $<TARGET_OBJECTS:nuflux_lib>)
	set_target_properties(nuflux_static PROPERTIES OUTPUT_NAME nuflux)
	set_target_properties(nuflux_shared PROPERTIES OUTPUT_NAME nuflux)
	target_include_directories(nuflux_static PUBLIC $<INSTALL_INTERFACE:include>)
	target_include_directories(nuflux_shared PUBLIC $<INSTALL_INTERFACE:include>)
	if(Photospline_FOUND)
		target_link_libraries(nuflux_static INTERFACE photospline)
		target_link_libraries(nuflux_shared PUBLIC photospline)
	endif(Photospline_FOUND)
	add_library(nuflux INTERFACE)
	target_link_libraries(nuflux INTERFACE nuflux_shared Boost::headers)
endif(NOT PYTHON_ONLY)

set(PYBINDINGS_SOURCES
	"${CMAKE_SOURCE_DIR}/src/pybindings/module.cxx"
)

if(PYTHON_BINDINGS)
	find_package(Python3 COMPONENTS Interpreter Development.Module NumPy)
	if(NOT Python3_FOUND)
		message(WARNING "Python with NumPy not found; searching for Python without NumPy")
		find_package(Python3 COMPONENTS Interpreter Development.Module REQUIRED)
		if(Python3_FOUND)
			message("-- Found Python without NumPy; NumPy support will be disabled")
		endif(Python3_FOUND)
	else(NOT Python3_FOUND)
		message(" + Will build with NumPy support")
		set(HAVE_NUMPY ON)
	endif(NOT Python3_FOUND)
	find_package(Boost COMPONENTS python REQUIRED)
	
	if(PYTHON_ONLY)
		target_link_libraries(nuflux_lib PRIVATE Boost::python Python3::Module)
		if(HAVE_NUMPY)
			target_link_libraries(nuflux_lib PRIVATE Python3::NumPy)
		endif(HAVE_NUMPY)
	endif(PYTHON_ONLY)
	
	file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/nuflux")
	configure_file("${CMAKE_SOURCE_DIR}/src/pybindings/__init__.py" "${CMAKE_BINARY_DIR}/nuflux/__init__.py")
	add_library(nuflux_python SHARED ${PYBINDINGS_SOURCES})
	set_target_properties(nuflux_python PROPERTIES OUTPUT_NAME "_nuflux" PREFIX "" SUFFIX ".so"
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/nuflux")
	target_include_directories(nuflux_python BEFORE PRIVATE "${CMAKE_SOURCE_DIR}/include" "${CMAKE_BINARY_DIR}")
	target_link_libraries(nuflux_python PRIVATE Boost::headers Boost::python Python3::Module nuflux_lib)
	if(Photospline_FOUND)
		target_link_libraries(nuflux_python PRIVATE photospline)
	else(Photospline_FOUND)
		target_compile_definitions(nuflux_python PRIVATE NO_PHOTOSPLINE)
	endif(Photospline_FOUND)
	if(HAVE_NUMPY)
		target_link_libraries(nuflux_python PRIVATE Python3::NumPy)
	endif(HAVE_NUMPY)
endif(PYTHON_BINDINGS)

# ==== Documentation ====

find_program(DOXYGEN_FOUND doxygen)
if(DOXYGEN_FOUND)
	set(DOXYGEN_HTML_DIR "${CMAKE_BINARY_DIR}/doxygen_html")
	set(DOXYGEN_XML_DIR "${CMAKE_BINARY_DIR}/doxygen_xml")
	configure_file("${CMAKE_SOURCE_DIR}/docs/Doxyfile.in" "Doxyfile")
	
	add_custom_target(doxygen doxygen WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
	
	add_custom_target(docs DEPENDS doxygen)
	
	execute_process(COMMAND "${Python3_EXECUTABLE}" -c "import sphinx, breathe"
		RESULT_VARIABLE HAVE_SPHINX
		ERROR_VARIABLE HAVE_SPHINX_ERR)
	if(NOT (HAVE_SPHINX EQUAL 0))
		configure_file("${CMAKE_SOURCE_DIR}/docs/conf.py.in" "${CMAKE_BINARY_DIR}/sphinx/conf.py")
		add_custom_target(sphinx "${Python3_EXECUTABLE}" -m sphinx -c "${CMAKE_BINARY_DIR}/sphinx/" "${CMAKE_SOURCE_DIR}/docs" "${CMAKE_BINARY_DIR}/sphinx" DEPENDS doxygen)
		add_dependencies(docs sphinx)
	endif(NOT (HAVE_SPHINX EQUAL 0))
endif(DOXYGEN_FOUND)

# ==== Tests ====

enable_testing()
add_executable(test-basic
	"${CMAKE_SOURCE_DIR}/tests/test_basic.cxx"
)
target_include_directories(test-basic BEFORE PRIVATE "${CMAKE_SOURCE_DIR}/include" "${CMAKE_BINARY_DIR}")
target_link_libraries(test-basic PRIVATE Boost::headers nuflux_lib)
if(Photospline_FOUND)
	target_link_libraries(test-basic PRIVATE photospline)
endif(Photospline_FOUND)

add_test(test-basic COMMAND "${CMAKE_BINARY_DIR}/test-basic")

if(PYTHON_BINDINGS)
	add_test(test-python COMMAND "${Python3_EXECUTABLE}" -m pytest "${CMAKE_SOURCE_DIR}/tests")
	
	execute_process(COMMAND "${Python3_EXECUTABLE}" -c "import pytest"
		RESULT_VARIABLE HAVE_PYTEST
		ERROR_VARIABLE HAVE_PYTEST_ERR)
	if(NOT (HAVE_PYTEST EQUAL 0))
		set_property(TEST test-python PROPERTY DISABLED ON)
	endif(NOT (HAVE_PYTEST EQUAL 0))
	if(NOT Photospline_FOUND)
		set_property(TEST test-python PROPERTY ENVIRONMENT "NO_PHOTOSPLINE=1;")
	endif(NOT Photospline_FOUND)
endif(PYTHON_BINDINGS)

# ==== Installation ====

if(APPLE)
	# See: https://gitlab.kitware.com/cmake/community/-/wikis/doc/cmake/RPATH-handling
	set(CMAKE_MACOSX_RPATH TRUE)
	set(CMAKE_SKIP_BUILD_RPATH FALSE)
	set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
	set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
	set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif(APPLE)

include("${CMAKE_SOURCE_DIR}/cmake/set_installed_flag.cmake")

set(REPLACE_INSTALL_FLAG 
	"s|${INSTALL_FLAG_PREFIX}${INSTALL_FLAG_OFF}${INSTALL_FLAG_SUFFIX}|${INSTALL_FLAG_PREFIX}${INSTALL_FLAG_ON}${INSTALL_FLAG_SUFFIX}|g")

if(NOT PYTHON_ONLY)
	install(TARGETS nuflux_static nuflux_shared nuflux EXPORT ${PROJECT_NAME}Targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
	set_installed_flag(INSTALLED_FILE "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}nuflux${CMAKE_STATIC_LIBRARY_SUFFIX}" SUBST "${REPLACE_INSTALL_FLAG}")
	set_installed_flag(INSTALLED_FILE "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}nuflux${CMAKE_SHARED_LIBRARY_SUFFIX}" SUBST "${REPLACE_INSTALL_FLAG}")
	install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} FILES_MATCHING PATTERN "*.h")
	install(FILES "${CMAKE_BINARY_DIR}/version.h" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/nuflux")
	
	include(CMakePackageConfigHelpers)
	
	set(_config_dir "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}")
	
	write_basic_package_version_file(
		"${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
		VERSION ${PROJECT_VERSION}
		COMPATIBILITY SameMajorVersion
	)
	export(EXPORT ${PROJECT_NAME}Targets FILE ${PROJECT_NAME}Targets.cmake)
	configure_package_config_file(
		"${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
		"${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
		INSTALL_DESTINATION "${_config_dir}"
	)
	
	install(FILES
		"${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
		"${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake"
		"${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
		DESTINATION "${_config_dir}"
	)
	install(EXPORT ${PROJECT_NAME}Targets DESTINATION "${_config_dir}")
endif(NOT PYTHON_ONLY)

if(PYTHON_BINDINGS)
	if(SKBUILD_PLATLIB_DIR)
		message("Base directory: ${SKBUILD_DATA_DIR}")
		message("Module directory: ${SKBUILD_PLATLIB_DIR}")
		set(PYTHON_MODULE_DIR "${SKBUILD_PLATLIB_DIR}")
	else(SKBUILD_PLATLIB_DIR)
		execute_process(COMMAND "${Python3_EXECUTABLE}" -c
"from sys import version_info;
if version_info.major < 3 or (version_info.major == 3 and version_info.minor < 10):
 from distutils.sysconfig import get_python_lib; print(get_python_lib(prefix='')); exit(0);
from sysconfig import get_path, get_preferred_scheme;
from pathlib import PurePath;
ps = get_preferred_scheme('prefix');
d  = PurePath(get_path('data', ps));
pl = PurePath(get_path('platlib', ps));
print(pl.relative_to(d))"
			OUTPUT_VARIABLE PYTHON_MODULE_DIR
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)
		set(PYTHON_MODULE_DIR "${CMAKE_INSTALL_PREFIX}/${PYTHON_MODULE_DIR}")
	endif(SKBUILD_PLATLIB_DIR)
	install(TARGETS nuflux_python DESTINATION "${PYTHON_MODULE_DIR}/nuflux")
	set_installed_flag(INSTALLED_FILE "${PYTHON_MODULE_DIR}/nuflux/_nuflux.so" SUBST "${REPLACE_INSTALL_FLAG}")
	install(FILES "${CMAKE_BINARY_DIR}/nuflux/__init__.py" DESTINATION "${PYTHON_MODULE_DIR}/nuflux")
endif(PYTHON_BINDINGS)

# If building in python-only mode, shove the data files inside the python package itself.
# Python build systems are perfectly capable of installing data in a more standard location,
# but the python runtime is too broken to support any reliable means for a module to find the
# data directory associated with its own installed location.
if(PYTHON_ONLY)
	if(SKBUILD_PLATLIB_DIR)
		set(NUFLUX_DATA_INSTALL_DIR "${SKBUILD_PLATLIB_DIR}/nuflux")
	else(SKBUILD_PLATLIB_DIR)
		set(NUFLUX_DATA_INSTALL_DIR "${PYTHON_MODULE_DIR}/nuflux")
	endif(SKBUILD_PLATLIB_DIR)
endif(PYTHON_ONLY)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/resources/data DESTINATION "${NUFLUX_DATA_INSTALL_DIR}")

# ==== Uninstallation ====

configure_file("${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in" 
	"${CMAKE_BINARY_DIR}/cmake_uninstall_cmake"
	IMMEDIATE @ONLY)
ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_BINARY_DIR}/cmake_uninstall_cmake")
