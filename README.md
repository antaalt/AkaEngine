# AkaEngine

## Next steps

- Solid hot reload of both assets & shaders
    - Use multithread jobs for asset loading.
    - asset indexer check whole asset folder & potentially import into library
        Check all assets, ask for import, if one was updated & is already imported, import it in background & reload it in engine.
    - asset indexer can also spot shaders & generate shader database.
        - shader db store entry points & potentially macros ?
        We can retrieve them using ShaderPath

Once asset management is solid, can work on required tools for game, 
-   aseprite importer
-   Ogmo ? or custom level editor with quad tree.
-   blender ? custom loading of blend file ? assimp ?
-   Test gameplay & level creation.