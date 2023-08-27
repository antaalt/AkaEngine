cmake_minimum_required (VERSION 3.10)

project(AkaEditor)

# Include Aka
add_subdirectory(lib/Aka)

add_executable(AkaEditor
	"src/main.cpp"
	"src/Editor.cpp"
 	"src/Renderer/Renderer.hpp" 
 	"src/Importer/Importer.hpp"
 	"src/Asset/Asset.hpp"
	"src/Asset/Archive/ArchiveGeometry.cpp" 
	"src/Asset/Archive/ArchiveMaterial.cpp" 
	"src/Asset/Archive/ArchiveBatch.cpp" 
	"src/Asset/Archive/ArchiveStaticMesh.cpp"
	"src/Asset/Resource/StaticMesh.cpp" 
	"src/Asset/Archive/ArchiveImage.cpp"
)
#add_definitions(AKA_USE_IMGUI_LAYER)

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