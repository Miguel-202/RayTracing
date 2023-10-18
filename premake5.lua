-- premake5.lua
workspace "RayTracingRenderer"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "RayTracingRenderer"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
include "Walnut/WalnutExternal.lua"

include "RayTracingRenderer"