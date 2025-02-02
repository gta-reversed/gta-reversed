--[[
    GTA_reversed Build Script
    Use 'premake5 --help' for help
--]]


--[[
    Options and Actions
--]]

newoption {
    trigger     = "script-tracing",
    description = "Enable script command trace logging (Slow!)"
}
newoption {
    trigger     = "outdir",
    value       = "path",
    description = "Output directory for the build files"
}
newoption {
    trigger     = "allow-script-cmd-hooks",
    description = "Whenever script command hooks should be generated (Slows down (full) build by a lot)"
}

if not _OPTIONS["outdir"] then
    _OPTIONS["outdir"] = "build"
end


--[[
    The Solution
--]]

workspace "gta_reversed"
    startproject "gta_sa_modern"
    configurations { "Release", "Debug" }

    location(_OPTIONS["outdir"])
    targetprefix "" -- no 'lib' prefix on gcc
    targetdir("bin/%{cfg.buildcfg}")
    
    filter "configurations:Debug*"
        staticruntime "off"
        symbols "On"
        runtime "Debug"
        vectorextensions "AVX2"
        defines { "NOTSA_DEBUG" }

    filter "configurations:Release*"
        staticruntime "off"
        symbols "On"
        runtime "Release"
        vectorextensions "AVX2"
        defines { "NDEBUG" }
        optimize "Full"
        flags { "LinkTimeOptimization" }

    filter "action:vs*"
        flags { "MultiProcessorCompile" }
        linkoptions   { "/ignore:4099,4251,4275" }
        buildoptions { "/EHsc", "/Zc:preprocessor", "/bigobj", "/utf-8" }
        disablewarnings { 26812, 26495, 4275, 4251, 4200, 4099 }

    filter "files:libs/**"
        warnings "Off"

    filter {}

    characterset "MBCS"  -- Fix strings
    staticruntime "On" 
    rtti "Off"

    flags {
        "NoImportLib",
        --"NoBufferSecurityCheck"
        "FatalWarnings",
    }

    include "source/"
    local gta_exe = os.getenv("GTA_SA_EXE")
    if gta_exe ~= nil and os.isfile(gta_exe) then
        debugcommand "$(GTA_SA_EXE)"
    else
        debugcommand "$(GTA_SA_DIR)/gta_sa.exe"
    end
    debugdir "$(GTA_SA_DIR)"

    group "Dependencies"
        defines { "WIN32", "_WINDOWS" }
        include "libs/"
