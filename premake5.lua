workspace "HouseEngine"
   architecture "x64"
   startproject "Engine"

   configurations
   {
      "Debug",
      "Release"
   }

   outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
   VULKAN_SDK = os.getenv("VULKAN_SDK")

    IncludeDir = {}
    IncludeDir["GLFW"] =          "%{wks.location}/vendor/GLFW/include"
    IncludeDir["glm"] =           "%{wks.location}/vendor/glm/"
    IncludeDir["ImGui"] =         "%{wks.location}/vendor/imgui"
    IncludeDir["Vulkan"] = "%{VULKAN_SDK}/Include"

    Library = {}

    Library["Vulkan"] = "%{VULKAN_SDK}/Lib"
    Library["VulkanSDK"] = "%{VULKAN_SDK}/Lib/vulkan-1.lib"
    Library["ShaderC"] =   "%{VULKAN_SDK}/Lib/shaderc_shared.lib"
    Library["SpriVCore"] = "%{VULKAN_SDK}/Lib/spirv-cross-core.lib"
    Library["SpriVGLSL"] = "%{VULKAN_SDK}/Lib/spirv-cross-glsl.lib"
    Library["SpriVTOOLS"] ="%{VULKAN_SDK}/Lib/SPIRV-Tools.lib"

    group "Dependencies"
        include "vendor/imgui"
        include "vendor/GLFW"
    group ""

project "Engine"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"

   targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
   objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

   files {
       "Source/**.h",
       "Source/**.cpp",
       "Shaders/**",
       "vendor/stb/**.h",
       "vendor/stb/**.cpp",
       "vendor/glm/glm/**.hpp",
       "vendor/glm/glm/**.inl",
   }

   includedirs {
        "Source",
        "vendor",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.Vulkan}"
   }

   defines 
   {
       "_CRT_SECURE_NO_WARNINGS"
   }

   links {
        "GLFW",
        "ImGui",

        "%{Library.VulkanSDK}",
        "%{Library.ShaderC}",
        "%{Library.SpriVCore}",
        "%{Library.SpriVGLSL}",
        "%{Library.SpriVTOOLS}"
       }

   buildoptions "/utf-8"
   filter "system:windows"
       systemversion "latest"

   filter "configurations:Debug"
       runtime "Debug"
       symbols "on"

   filter "configurations:Release"
       runtime "Release"
       optimize "on"

