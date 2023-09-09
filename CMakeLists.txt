cmake_minimum_required (VERSION 3.10)

project(AkaEditor)

# Include Aka
add_subdirectory(lib/Aka)

add_executable(AkaEditor
	"src/main.cpp"
	"src/Editor.cpp"

 	"src/Renderer/Renderer.hpp" 

 	"src/Importer/Importer.hpp"
	"src/Importer/AssimpImporter.cpp"

 	"src/Asset/Asset.cpp"
	"src/Asset/AssetLibrary.cpp"
	"src/Asset/AssetPath.cpp"
	"src/Asset/Archive/ArchiveGeometry.cpp" 
	"src/Asset/Archive/ArchiveMaterial.cpp" 
	"src/Asset/Archive/ArchiveBatch.cpp" 
	"src/Asset/Archive/ArchiveStaticMesh.cpp"
	"src/Asset/Archive/ArchiveImage.cpp"
	"src/Asset/Archive/ArchiveScene.cpp" 
	"src/Asset/Archive/Archive.cpp"
	"src/Asset/Resource/Scene.cpp"
	"src/Asset/Resource/StaticMesh.cpp" 
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