set(NAME glm_dummy)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

if(CMAKE_COMPILER_IS_GNUCXX)
	#add_definitions(-S)
	#add_definitions(-s)
	add_definitions(-m64)
	add_definitions(-O2)

	#add_definitions(-fprofile-arcs -ftest-coverage) gcov
	#ctest_enable_coverage()
endif()

file(GLOB ROOT_SOURCE *.cpp)
file(GLOB ROOT_INLINE *.inl)
file(GLOB ROOT_HEADER *.hpp)
file(GLOB ROOT_TEXT ../*.txt)
file(GLOB ROOT_NAT ../util/glm.natvis)

file(GLOB_RECURSE CORE_SOURCE ./detail/*.cpp)
file(GLOB_RECURSE CORE_INLINE ./detail/*.inl)
file(GLOB_RECURSE CORE_HEADER ./detail/*.hpp)

file(GLOB_RECURSE GTC_SOURCE ./gtc/*.cpp)
file(GLOB_RECURSE GTC_INLINE ./gtc/*.inl)
file(GLOB_RECURSE GTC_HEADER ./gtc/*.hpp)

file(GLOB_RECURSE GTX_SOURCE ./gtx/*.cpp)
file(GLOB_RECURSE GTX_INLINE ./gtx/*.inl)
file(GLOB_RECURSE GTX_HEADER ./gtx/*.hpp)

source_group("Text Files" FILES ${ROOT_TEXT})
source_group("Core Files" FILES ${CORE_SOURCE})
source_group("Core Files" FILES ${CORE_INLINE})
source_group("Core Files" FILES ${CORE_HEADER})
source_group("GTC Files" FILES ${GTC_SOURCE})
source_group("GTC Files" FILES ${GTC_INLINE})
source_group("GTC Files" FILES ${GTC_HEADER})
source_group("GTX Files" FILES ${GTX_SOURCE})
source_group("GTX Files" FILES ${GTX_INLINE})
source_group("GTX Files" FILES ${GTX_HEADER})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

add_library(glm STATIC ${ROOT_TEXT} ${ROOT_NAT}
                               		${ROOT_SOURCE}    ${ROOT_INLINE}    ${ROOT_HEADER}
                               		${CORE_SOURCE}    ${CORE_INLINE}    ${CORE_HEADER}
                               		${GTC_SOURCE}     ${GTC_INLINE}     ${GTC_HEADER}
                               		${GTX_SOURCE}     ${GTX_INLINE}     ${GTX_HEADER})
#add_library(glm_shared SHARED glmh.cpp)
