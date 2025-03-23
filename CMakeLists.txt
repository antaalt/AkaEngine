cmake_minimum_required (VERSION 3.28)

project(AkaEditor)

# Enable hot reload on Visual studio
if (MSVC AND WIN32 AND NOT MSVC_VERSION VERSION_LESS 142)
    add_link_options($<$<CONFIG:Debug>:/INCREMENTAL>)
    add_compile_options($<$<CONFIG:Debug>:/ZI>)
endif()

# Include Aka
add_subdirectory(lib/Aka)

add_executable(AkaEditor
	"src/main.cpp"
	"src/Editor.cpp"

	"src/Importer/AssimpImporter.cpp"
	
	"src/Editor/SceneEditorLayer.cpp" 
	"src/Editor/AssetBrowserEditorLayer.cpp" 
	"src/Editor/InfoEditorLayer.cpp" 
	"src/Editor/PerformanceEditorLayer.cpp"
	"src/Editor/AssetViewerEditorLayer.cpp"
	"src/Editor/AssetViewer/AssetViewer.cpp" 
	"src/Editor/AssetViewer/StaticMeshViewer.cpp" 
	"src/Editor/AssetViewer/SceneViewer.cpp"
	"src/Editor/AssetViewer/TextureViewer.cpp"
	"src/Editor/EditorLayer.cpp"
	"src/Editor/Modal/FileModal.cpp" 
	"src/Editor/Modal/ImportModal.cpp"
	
	"src/Component/CustomComponent.cpp"
	"src/Component/RotatorComponent.cpp"
)

# ASSIMP
set(ZLIB_LIBRARIES zlibstatic)
set(ENABLE_BOOST_WORKAROUND ON)
set(BUILD_STATIC_LIB ON)
set(BUILD_ASSIMP_TOOLS OFF)
set(BUILD_ASSIMP_VIEW OFF)
set(ASSIMP_BUILD_STATIC_LIB ON)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_ZLIB ON)
add_subdirectory(lib/assimp)
target_include_directories(AkaEditor PUBLIC lib/assimp lib/IconCppHeaders)
target_link_libraries(AkaEditor assimp)

# Aka
target_link_libraries(AkaEditor Aka)