cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
include(local_conf.cmake OPTIONAL)
project(Statistical_models VERSION 0.1 DESCRIPTION "Application example to reproduce results of Effective dimension" LANGUAGES CXX)


set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
	set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
		STRING "Choose the type of build." FORCE)
	# Set the possible values of build type for cmake-gui
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
		"Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_executable(fisher_app Fisher_app.cpp)
####################################CONFIG-VARIABLES#####################################
if(NOT  MODEL)
	set(MODEL "DNN_abbas" CACHE STRING "")
	message(STATUS "MODEL not set using default ${MODEL} from options")
endif(NOT MODEL)
target_compile_definitions(fisher_app PRIVATE MODEL=${MODEL})
option(USE_YAML "On or off" ON)
#########################################################################################


include(FetchContent)
find_package(Torch REQUIRED)

target_link_libraries(fisher_app PRIVATE  "${TORCH_LIBRARIES}")
if(USE_YAML)
	if(NOT TARGET yaml-cpp)
		find_package(YAML-CPP CONFIG QUIET)
		if(NOT YAML-CPP_FOUND)
			message(STATUS "YAML-CPP was not found. Downloading")
			FetchContent_Declare(
				yaml-cpp
				GIT_REPOSITORY git@github.com:jbeder/yaml-cpp.git
				GIT_TAG yaml-cpp-0.7.0
				)
			FetchContent_MakeAvailable(yaml-cpp)
		else()
			message(STATUS "YAML FOUND in ${YAML-CPP_DIR}")
		endif(NOT YAML-CPP_FOUND)
	endif(NOT TARGET yaml-cpp)
endif(USE_YAML)

target_link_libraries(fisher_app PRIVATE  yaml-cpp)

FetchContent_Declare(
	custom_modules
	GIT_REPOSITORY git@github.com:EddyTheCo/custom_modules.git
	GIT_TAG main
	)
FetchContent_MakeAvailable(custom_modules)


target_link_libraries(fisher_app PRIVATE  custom_modules)


FetchContent_Declare(
	Fisher_Matrix	
	GIT_REPOSITORY git@github.com:EddyTheCo/Fisher_Matrix.git
	GIT_TAG main
	)
FetchContent_MakeAvailable(Fisher_Matrix)


target_link_libraries(fisher_app PRIVATE Fisher_Matrix)



FetchContent_Declare(
	yaml-torch
	GIT_REPOSITORY git@github.com:EddyTheCo/yaml-torch.git
	GIT_TAG main
	)
FetchContent_MakeAvailable(yaml-torch)
target_link_libraries(fisher_app PRIVATE  yaml-torch)

target_compile_features(fisher_app PUBLIC cxx_std_17)



install(TARGETS fisher_app DESTINATION bin)
install(DIRECTORY INPUT DESTINATION .)
install(DIRECTORY gnuplot DESTINATION .)
execute_process(COMMAND "git" "log" "--format='%H'" "-n 1"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        OUTPUT_VARIABLE  commit)
execute_process(COMMAND "git" "rev-parse" "--abbrev-ref" "HEAD" 
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        OUTPUT_VARIABLE  branch)

FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/.${PROJECT_NAME}.txt  ${PROJECT_NAME}:\n)
FILE(APPEND ${CMAKE_CURRENT_BINARY_DIR}/.${PROJECT_NAME}.txt  \t commit=${commit})
FILE(APPEND ${CMAKE_CURRENT_BINARY_DIR}/.${PROJECT_NAME}.txt  \t branch=${branch}\n)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/.${PROJECT_NAME}.txt DESTINATION .conf)
