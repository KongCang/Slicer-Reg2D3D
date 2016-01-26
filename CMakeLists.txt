cmake_minimum_required(VERSION 2.8.9)

project(Slicer-Reg2D3D)

#-----------------------------------------------------------------------------
# Extension meta-information
set(EXTENSION_HOMEPAGE "http://www.example.com/Slicer/Extensions/Slicer-Reg2D3D")
set(EXTENSION_CATEGORY "Examples")
set(EXTENSION_CONTRIBUTORS "John Doe (AnyWare Corp.)")
set(EXTENSION_DESCRIPTION "This is an example of a simple extension")
set(EXTENSION_ICONURL "http://www.example.com/Slicer/Extensions/Slicer-Reg2D3D.png")
set(EXTENSION_SCREENSHOTURLS "http://www.example.com/Slicer/Extensions/Slicer-Reg2D3D/Screenshots/1.png")

#-----------------------------------------------------------------------------
# Extension dependencies
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})

#-----------------------------------------------------------------------------
# Extension modules
add_subdirectory(Reg2D3D)
## NEXT_MODULE

#-----------------------------------------------------------------------------
include(${Slicer_EXTENSION_CPACK})
