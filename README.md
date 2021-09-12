# GHEngine

A multi platform game engine C++.

* See LICENSE.txt for license info.
* For further info email dev@goldenhammersoftware.com.

Use getall.sh to pull with dependencies.

This is the Golden Hammer Software engine used in Trick Shot Bowling 2.  It is an evolution of the engine used in Big Mountain Snowboarding.  It has support for rendering in dx11, metal, or opengl es with VR support for Oculus and OpenVR.  This code has been used to create games and prototypes for windows, uwp, ios, android, blackberry, oculus, and steam.

It is currently not expected that this is useful outside of Golden Hammer Software projects.  Efforts to make this code more broadly useful to others are ongoing but are not guaranteed.  As such you may find project setup, tools, and documentation lacking.  We recommend starting with UE5 or Unity instead but this code can be used as reference.

## Additional Dependencies
Several external libraries are required to exist in ../contrib or ../contrib-windows.  Before this can be compiled we will need to add an automated way to populate these directories.  We store these files in a separate repo but the licensing issues of sharing that are unclear.

# Project Descriptions and Dependencies

### GHString
String utility functions.
Dependencies: None.

### GHMath
Math utility functions.
Dependencies: None.

### GHPlatform
Cross platform OS interface utilities.
Dependencies: GHString.

### GHUtils
Useful utilities that have not been moved to a separate repo.
Dependencies: GHString, GHPlatform

### Base
The root project for any app.  This is not specific to making games and can be used for console applications.
Dependencies: GHString, GHPlatform, GHUtils.

### Render
The base library for platform-independent drawing code.
Dependencies: Base

### GUI
Our GUI system.
Dependencies: Render

### Sim
Physics interfaces and useful code for simulations.  This project exists to remove dependencies on Game and Render.
Dependencies: Base

### GHBullet
Physics interface implementation using Bullet.
Dependencies: Sim, Bullet3(external)

### Game
The root project for platform independent game code.
Dependencies: Sim, Render.

# Data Directories

### DX11Shaders
These are our cannon shaders used to generate the shaders for other platforms by using Tools/ShaderConverter which uses dxc and spirv-cross.  This is a new development and we still have hand generated versions of the shaders for other languages until we finish the transition.

### Shaders
Our glsl shaders.  This directory is deprecated.

### iOSShaders
Our glsl es shaders.  This directory is deprecated.

### shared
Common optional data that might be useful to any app.