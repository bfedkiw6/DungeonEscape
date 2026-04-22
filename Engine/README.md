<!--Created by Marcus Winter -->

# Stencil Reference doc
This stencil was crafted to be simple to understand and robust to build upon for a large scale project.
In its base format, it supports 3D geometry loading with textures from almost every file format (that [Assimp](https://github.com/assimp/assimp/blob/master/doc/Fileformats.md) supports)
and basic OpenGL drawing capabilities, along with Audio and ImGUI integration. We strongly recommend spending some time familiarizing yourself with the stencil (and
OpenGL if it has been a while).

As a rule, all code in this stencil is designed to be fairly simple and easy to understand. There are many places where 
you can expand functionality and optimize performance.
#### **YOU MAY EDIT ANYTHING AND EVERYTHING**  
There is no single approach to a game engine and such you may do whatever you wish with this stencil, including writing
your own renderer from scratch. The libraries provided and the OpenGL implementation serves as a basic
starting point.


## Quick reference guide:

### Namespaces:
- All OpenGL related code is in the `gl::` namespace
- Debug functions are in the `debug::` namespace and File utilities in the `file::` namespace

### CMake and Dependencies
All dependencies are built by the CMakeLists.txt file.  We also provide some toggleable options for various dependencies
if you so decide not to use them, however you may need to remove or update our stencil implementations to suit.
If you are unfamiliar with CMake, check out [this tutorial](https://cmake.org/cmake/help/book/mastering-cmake/cmake/Help/guide/tutorial/)
to get started. AI tools also can be very helpful.

### Building source files
Any ```.h/.cpp``` files added in the ```Engine/src/``` directory will be built and linked when CMake is reloaded. If you add or remove any
files, you must **reload your CMake project** for the files to be correctly built.

Headers are located in ```Engine/include/Engine/``` and can be included using ```#include <Engine/header-name.h>```.

---

# Detailed File Overview

## `include/Engine/`
###  ```window.h```
> Responsible for initializing the top level Window using GLFW and passing various input callbacks to your Game class.
Also initializes the Graphics file.

### ```game.h```
> The base class for your game. Inherit from `Game` and override the virtual methods to handle updates, drawing,
and input events. Pass your game instance to `Window::initialize()` to run it.

### ```debug.h```
> Includes helpful debug functions for printing various messages in terminal. Supports variable arguments with each '{}'
> in the message string being replaced by the next argument in the function.
> - `debug::error("Message: {}", err)` - Macro which prints an error message and includes the file and line number which it was called at.
> - `debug::print("Message: {} {}", var1, var2)` - only prints when CMake Debug Profile is active. Setting to 'Release' mutes these messages.
> - See also `debug::info()`, `debug::warn()`, and `debug::log()`
> - Also supports `glm::vec` as arguments. See file comments for more info on how to support displaying types in your own format.

### ```file.h```
> Includes helpful utility functions for managing file paths using std::filesystem. Useful for any file loading to ensure
> proper compatibility regardless of system and build directory.
> - **All files should be given as a path from the content root.** e.g "Resources/Shaders/phong.glsl"
> - See file for more details and function definitions

### ```transform.h```
> Represents the model transform of an object. Used by `graphics.h` for draw functions. We strongly recommend adding any
> extra functionality you deem necessary to this class.

---

## `include/Engine/Graphics/`

### ```graphics.h```
> Responsible for all OpenGL drawing functionality. Contains methods for drawing meshes, shapes and text. Also contains
> any loaded shaders and methods for setting uniforms such as camera and lights.
> - Methods are all static and can be accessed using `gl::Graphics::methodName()`
> - Before any draw calls, you must call `use__Shader()` to set the appropriate active shader program.
> - Remember to use `setCamera()`, `setLights()` and `setAmbientLight()` with the appropriate shader bound.
> - Most methods are self-explanatory. See file comments for more details.

### ```mesh.h```
> This class is responsible for loading 3D geometry from files or shape data and binding to OpenGL VAO, VBO and EBO.
> - It contains various structs which represent different drawable objects, with the load method storing the object
> internally and returning a pointer which can be used with `Graphics`'s `draw()` methods.
> - Once a mesh has been loaded, it can be retrieved using Mesh::get___Mesh() methods. Use the file name for meshes that were loaded from files.
> - _**NOTE**_ Along with VAO's and VBO's, we use [Element Buffer Object](https://opentk.net/learn/chapter1/3-element-buffer-objects.html)
> (EBO's) to achieve indexed drawing. This is a more efficient and modern implementation of OpenGL rendering.

### ```shaders.h```
> This class is responsible for initializing a ShaderProgram.
> - ShaderProgram class can be used to activate its stored shader with the `use()` function.
> - It also includes various `setUniform()` methods, e.g. `setVec3()`. These are mostly for code readability.

### ```camera.h```
> This class represents the Camera. We recommend adding any methods which may be helpful to allow for easier camera
> control.
> - Used by `Graphics` for all of its 3D draw functions

### ```light.h```
> Minimal struct for containing light data. Currently `Directional` and `Point` are implemented in Graphics and the phong shader.
> Expand this if adding more light types.

### ```Shapes/*.h```
> Contains 5 basic drawing shapes: cone, cube, cylinder, quad and sphere. These can be loaded by `Mesh::loadPrimitive()`
> and then drawn using `Graphics::drawShape()`

### ```Text/font.h```
> Implements FreeType's library for loading fonts and creating characters as OpenGL textures.
> - Also contains methods for getting the size of the text in pixels

### ```Text/text-renderer.h```
> Implementation of text rendering used by `Graphics`. Also supports loading fonts and stores them internally in a
> `std::unordered_map<string, font>` for easy access using the font's name.
> - Use Graphics::drawText() to draw text at a given position (pixels) on the screen. Remember to call `gl::Graphics::useTextShader()` beforehand.
> - AnchorPosition determine where the alignment of the rendered text.

### ```Skinned/skeleton.h```
> This file contains a skeleton implementation for Skinned Meshes. We cover this topic in greater depth later in the course
> and for Lab 5.
> - The Skeleton is constructed using `Mesh::loadSkinned()` method which loads a skinned mesh file (.fbx, .dae, etc.)
> - The bones are stored in a vector, with a "bone_map" which maps each index to the name of the bone
> - Each bone contains various transform data and its parent and child bone indexes

### ```Skinned/animation.h```
> Contains Animation data for the Skeleton. Will also be explored further in Lab 5.
> - Contains AnimationChannels which each link to a single bone
> - Each AnimationChannel contains vectors of Position, Scale and Rotation keys which determine the transforms of the
> - bones at a given time while the animation is running.

---

## `include/Engine/Audio/`
### ```audio-engine.h```
> Implements raudio library to enable audio loading and playback. Supports basic positional sound which can be
> integrated into your game engine structure.
> - Usage: `initialize()` initializes the AudioEngine.
> - Use `loadSound()` and `loadMusic()` after initializing to store sounds/music for later playback
> - Call `update()` in the Window or Game update loop.
> - Use `playSound()` / `playMusic()` at any point

---

## `include/Engine/Imgui/`
### ```ui.h```
> Provides some basic setup code for using ImGUI. This class is to be used however much you want for helping with
> debugging as ImGUI makes displaying almost anything quick and easy. Sliders, CheckBoxes, Buttons.
> - ImGUI methods are all static and can be accessed by using ImGUI::methodName()