cmake_minimum_required (VERSION 3.13)

project(AkaEditor)

# Include Aka
add_subdirectory(lib/Aka)

add_executable(AkaEditor
	"src/main.cpp"
	"src/Editor.cpp"

 	"src/Renderer/Renderer.hpp" 

	"src/Importer/AssimpImporter.cpp"
	
	"src/Editor/SceneEditorLayer.cpp" 
	"src/Editor/AssetEditorLayer.cpp" 
	"src/Editor/InfoEditorLayer.cpp" 
	"src/Editor/AssetViewer/AssetViewerManager.cpp"
	"src/Editor/AssetViewer/AssetViewer.cpp" 
	"src/Editor/AssetViewer/StaticMeshViewer.cpp" 
	"src/Editor/AssetViewer/SceneViewer.cpp"
	"src/Editor/AssetViewer/TextureViewer.cpp"
)

# ASSIMP
set(ZLIB_LIBRARIES zlibstatic)
set(ENABLE_BOOST_WORKAROUND ON)
set(BUILD_STATIC_LIB ON)
set(BUILD_ASSIMP_TOOLS OFF)
set(BUILD_ASSIMP_VIEW OFF)
set(ASSIMP_BUILD_STATIC_LIB ON)
set(ASSIMP_BUILD_TESTS OFF)
add_subdirectory(lib/assimp)
target_include_directories(AkaEditor PUBLIC lib/assimp lib/IconCppHeaders)
target_link_libraries(AkaEditor assimp)


target_link_libraries(AkaEditor Aka)

#add_custom_command(
#	TARGET AkaGame POST_BUILD
#	COMMAND ${CMAKE_COMMAND} -E copy_if_different
#	"${PROJECT_SOURCE_DIR}/lib/Aka/lib/glew/bin/glew32.dll"
#	$<TARGET_FILE_DIR:AkaGame>
#)
#add_custom_command(
#	TARGET AkaGame POST_BUILD
#	COMMAND ${CMAKE_COMMAND} -E copy_directory
#	${PROJECT_SOURCE_DIR}/asset
#	$<TARGET_FILE_DIR:AkaGame>/asset
#)