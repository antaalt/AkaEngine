IF NOT EXIST ".\build\" mkdir .\build

:: Default shaders
%VULKAN_SDK%/bin/glslangValidator.exe -V shader.vert -o build/shader.vert.spv
%VULKAN_SDK%/bin/glslangValidator.exe -V shader.frag -o build/shader.frag.spv

pause
