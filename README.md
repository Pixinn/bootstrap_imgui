
# What is it?

This is a cross-platform *bootstrap* project to get started with **Dear Imgui**, along with **SDL2** and **OpenGL3.3+**.  
It also provides some OpenGl helpers and ImGui Widgets.

A demo program using the helpers and the widget is also included.

![Screenshot of the demo running on Windows](demo.png?raw=true "Demo running on Windows")

# Prerequisites

## conan.io

This project requires *Dear Imgui*, *sdl2* and *glew*.  
Those dependencies are handled by [*conan*](https://conan.io/).

Install *conan 1.x*:

```bash
> pip install 'conan<2.0'  
> conan remote add bincrafters https://bincrafters.jfrog.io/artifactory/api/conan/public-conan
> conan config set general.revisions_enabled=1
```
  
All paths required to use *glm* will also be set-up.

## cmake

The build chain is handled by *cmake*.

# How to build

```bash
> mkdir build && cd build
> conan install .. --build=missing
> cmake .. -DCMAKE_BUILD_TYPE=Release
> cmake --build . --config Release
```
  
**Please note** that you must be consistent between your conan 32/64 bit configuration and the build. Otherwise you'll get link errors. If you build using *Visual Studio* you may have to use the **Win64** generator.
