
# What is it?

This is a *bootstrap* project to get started with **Dear Imgui**, along with **SDL2** and **OpenGL3.3+**.

As a test, it will draw an orange rectangle and an "Hello World" box in a SDL2 window. This test is borrows from this [OpenGl tutorial](https://learnopengl.com/Getting-started/Hello-Triangle) and the [Dear Imgui example](https://github.com/ocornut/imgui/tree/master/examples/example_sdl_opengl3)

# Prerequisites

## conan.io

This project requires *Dear Imgui*, *sdl2* and *glew*. Also all paths required to use *glm* will be set-up.
Those dependencies are handled by [*conan*](https://conan.io/).

Install *conan* and add the *bincrafters* public repository:

```bash
> pip install conan  
> conan remote add bincrafters https://bincrafters.jfrog.io/artifactory/api/conan/public-conan
> conan config set general.revisions_enabled=1
```
  
## cmake

The build chain is handled by *cmake*.

# How to build

```bash
> mkdir build && cd build
> conan install ..
> cmake .. -DCMAKE_BUILD_TYPE=Release
> cmake --build . --config Release
```
  
**Please note** that you must be consistent between your conan 32/64 bit configuration and the build. Otherwise you'll get link errors. If you build using *Visual Studio* you may have to use the **Win64** generator.
