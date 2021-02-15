project(xlsxLib)

file(GLOB_RECURSE hFile ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB SrcFile ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB includeFile ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(${PROJECT_NAME} SHARED
	${hFile}
	${SrcFile}
	)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
target_link_libraries(${PROJECT_NAME}
	PUBLIC
	Qt5::Core
	Qt5::Gui
	Qt5::Widgets
	PRIVATE
	Qt5::GuiPrivate
	)
		
 source_group("include" FILES ${includeFile})
 add_compile_definitions(QT_BUILD_XLSX_LIB)
 #SET_TARGET_PROPERTIES(Ts_Excel_Transform) 