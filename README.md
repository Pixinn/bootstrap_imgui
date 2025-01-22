
# What is it?

This is a cross-platform *bootstrap* project to get started with **Dear Imgui**, along with **SDL2** and **OpenGL3.3+**.  
It also provides some OpenGl helpers and ImGui Widgets.

A demo program using the helpers and the widget is also included.

![Screenshot of the demo running on Windows](demo.png?raw=true "Demo running on Windows")

# Prerequisites

## conan.io

This project requires *Dear Imgui*, *sdl2* and *glew*.  
Those dependencies are handled by [*conan*](https://conan.io/).

Install and configure *conan*:

```bash
> pip install 'conan'
> conan profile detect 
```
  
All paths required to use *glm* will also be set-up.

## cmake

The build chain is handled by *cmake*.

# How to build

From the project root dir:

```bash
> conan install . -s build_type=Debug --build=missing
> conan install . -s build_type=Release --build=missing
> cmake --preset conan-default
```

The project and all have been generated in the *build* directory

```bash
> cd build
> cmake --build . --config=release -j
> cmake --build . --config=debug -j
```

The executable are in the *build/Release* and *build/Debug* directories along with all the required resource files.
  
**Please note** that you must be consistent between your conan 32/64 bit configuration and the build. Otherwise you'll get link errors. If you build using *Visual Studio* you may have to use the **Win64** generator.
