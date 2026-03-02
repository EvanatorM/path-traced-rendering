# Path Traced Rendering
This project aims to create a real-time path traced renderer. It is made using C++ and OpenGL. It will provide photo-realistic images with global illumination, containing both real-time rendering and the ability to render an image to a file.

## Building

<details>
<summary>Installing dependencies on Debian-based distros</summary>
<br>

Run this command to get the dependencies for building GLFW:

```sh
sudo apt install libwayland-dev libxkbcommon-dev xorg-dev cmake build-essential git
```

</details>

<details>
<summary>Installing dependencies on Windows</summary>
<br>

For Windows, you need the following:
- CMake
- Visual Studio or Visual Studio Build Tools with the 'Desktop Development with C++' workload
- Git

</details>

### Building with CMake

In the project root directory:
Create CMake files:

```sh
mkdir -p build
cd build
cmake ..
```

After that you can build the project using:

```sh
cmake --build .
```

If you are building from the root directory, use this command:

```sh
cmake --build build
```

Run the build command in the project root directory.

The final executable can be found at `(project root)/build/bin/(build type)`

#### Note for building with CMake

If you're running from a command line, make sure to run the executable in the same directory as it is located to ensure all resources are loaded properly.


## Plans
- Scene management
- Basic ray traced rendering
- OpenGL abstraction
- Lighting
- Path traced lighting
- Global illumination
- Interaction

## Dependencies
All dependencies are automatically downloaded using CMake. The dependencies are listed below.

- GLM (https://github.com/g-truc/glm)
- GLFW (https://www.glfw.org/)
- Glad (https://glad.dav1d.de/)