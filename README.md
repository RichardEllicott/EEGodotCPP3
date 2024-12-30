# EEGodotCPP3

# Godot GDExtension C++ boilerplate library

## Instructions
- get Godot 4.3
- install the latest (stable) [python](https://www.python.org/downloads/) to your platform, try to ensure the path variables are set upon install (ensuring you can use the python and pip commands at the terminal)
- open the terminal and install scons:

```
pip install scons
```

simply navigate to the directory and run scons:

```
scons
```

my scons file will automaticly download the [godot-cpp repo](https://github.com/godotengine/godot-cpp/tree/4.3), this can take some time so if you already have it you could copy it over before running scons

i reccomend using [vs.code](https://code.visualstudio.com/) and installing microsoft's C++ tools, my ".clang-format" file enforces oldschool bracket style for the automayic formatting
