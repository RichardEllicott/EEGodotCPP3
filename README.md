# EEGodotCPP3
# Godot GDExtension C++ boilerplate library

## Instructions
- install the latest (stable) [python](https://www.python.org/downloads/) to your platform, try to ensure the path variales are set upon install (ensuring you can use the python and pips commands at the terminal)
- open the terminal and install scons:
```
pip install scons
```
- get the [c++ bindings](https://github.com/godotengine/godot-cpp/tree/4.3) and put them in the root directory
- open the terminal and navigate to the project's root directory
- compile the c++ bindings by running the command scons in the directory
```
cd .\godot-cpp\
scons
```
- if you get any compile problems refer to the Godot docs, you may need to install a compiler
- setup an ide like [vs.code](https://code.visualstudio.com/)
- to compile the project run scons in the root directory, check out the demo folder for a Godot project
- source code to edit is in the src subfolder
- the file register_types.cpp is set up to link all the modules to compile
- i have custom macros in macros.h to reduce code for adding @export properties
- two templates demonstrate basic functionality, the second template is a single file template bypassing the normal two file convention... it may be useful for beginners and fast code editing
