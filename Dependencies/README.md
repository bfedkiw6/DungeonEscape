# CS1950U Dependencies

A collection of common C++ dependencies for CS1950U projects, managed via CMake with toggleable options.

## Available Dependencies

| Library | CMake Option | Target Name | Description |
|---------|--------------|-------------|-------------|
| [GLFW](https://github.com/glfw/glfw) | `DEP_GLFW` | `glfw` | Window/context creation and input handling |
| [GLM](https://github.com/g-truc/glm) | `DEP_GLM` | `glm` | OpenGL Mathematics library |
| [GLEW](http://glew.sourceforge.net/) | `DEP_GLEW` | `glew_static` or `GLEW::GLEW` | OpenGL Extension Wrangler (static) |
| [FreeType](https://github.com/freetype/freetype) | `DEP_FREETYPE` | `freetype` | Font rendering library |
| [Dear ImGui](https://github.com/ocornut/imgui) | `DEP_IMGUI` | `imgui` | Immediate mode GUI |
| [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo) | `DEP_IMGUIZMO` | `ImGuizmo` | Gizmo tools for ImGui |
| [STB](https://github.com/nothings/stb) | `DEP_STB` | `stb` | Header-only image loading and utilities |
| [Assimp](https://github.com/assimp/assimp) | `DEP_ASSIMP` | `assimp` | 3D model import library |
| [raudio](https://github.com/raysan5/raudio) | `DEP_RAUDIO` | `raudio` | Simple audio library |

## Setup

### Adding to Your Project

Add this repository as a git submodule:

```bash
git submodule add https://github.com/BrownCSCI1950U/dependencies.git dependencies
git submodule update --init --recursive
```

### CMake Configuration

In your project's `CMakeLists.txt`, enable the dependencies you need **before** adding the subdirectory:

```cmake
# Enable desired dependencies
set(DEP_GLFW ON)
set(DEP_GLM ON)
set(DEP_GLEW ON)
set(DEP_IMGUI ON)

# Add the dependencies directory
add_subdirectory(dependencies)

# Link to your target
target_link_libraries(your_target PRIVATE
    glfw
    glm
    glew_static
    imgui
)
```

## ImGui Options

When `DEP_IMGUI` is enabled, the following backend options are available (all ON by default):

| Option | Default | Description |
|--------|---------|-------------|
| `DEP_IMGUI_BACKEND_GLFW` | `ON` | GLFW platform backend |
| `DEP_IMGUI_BACKEND_OPENGL3` | `ON` | OpenGL3 renderer backend |
| `DEP_IMGUI_FREETYPE` | `ON` | FreeType font rendering |

**Note:** ImGui backends have dependencies:
- `DEP_IMGUI_BACKEND_GLFW` requires `DEP_GLFW=ON`
- `DEP_IMGUI_FREETYPE` requires `DEP_FREETYPE=ON`

### ImGuizmo

ImGuizmo requires ImGui. Enable both:

```cmake
set(DEP_IMGUI ON)
set(DEP_IMGUIZMO ON)
```

## Example: Full Graphics Project

```cmake
cmake_minimum_required(VERSION 3.16)
project(MyGame)

# Enable dependencies
set(DEP_GLFW ON)
set(DEP_GLM ON)
set(DEP_GLEW ON)
set(DEP_FREETYPE ON)
set(DEP_IMGUI ON)
set(DEP_STB ON)
set(DEP_ASSIMP ON)

add_subdirectory(dependencies)

add_executable(MyGame main.cpp)
target_link_libraries(MyGame PRIVATE
    glfw
    glm
    glew_static
    freetype
    imgui
    stb
    assimp
)
```

## Customizing Source Paths

By default, dependencies are expected in subdirectories of this folder. Override paths if needed:

```cmake
set(GLFW_SOURCE_DIR "/path/to/custom/glfw")
set(DEP_GLFW ON)
add_subdirectory(dependencies)
```

## License

See individual library directories for their respective licenses.
