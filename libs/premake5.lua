project "ogg"
    language "C++"
    kind "StaticLib"
    targetname "ogg"
    warnings "Off"
    
    vpaths {
        ["Headers/*"] = {"ogg/**.h",},
        ["Sources/*"] = {"ogg/**.c",},
        ["*"] = {"premake5.lua", "CMakeLists.txt"}
    }

    files {
        "ogg/**.h",
        "ogg/**.c"
    }

    includedirs { 
        "vorbis/include", 
        "ogg/include", 
        "ogg/include" 
    }

project "vorbis"
    language "C++"
    kind "StaticLib"
    targetname "vorbis"
    warnings "Off"

    vpaths {
        ["Headers/*"] = {"vorbis/**.h",},
        ["Sources/*"] = {"vorbis/**.c",},
        ["*"] = {"premake5.lua", "CMakeLists.txt"}
    }

    files { 
        "vorbis/win32/vorbis.def", 
        "vorbis/lib/**.*"
    }

    removefiles {
        "vorbis/lib/psytune.c",
        "vorbis/lib/tone.c",
        "vorbis/lib/misc.c",
        "vorbis/lib/psy.h",
    }

    includedirs { 
        "vorbis/include", 
        "ogg/include", 
        "%{cfg.targetdir}" 
    }

project "vorbisenc"
    language "C++"
    kind "StaticLib"
    targetname "vorbisenc"
    warnings "Off"
    
    vpaths {
        ["Sources/*"] = {"vorbis/**.c",},
        ["*"] = {"premake5.lua", "CMakeLists.txt"}
    }

    files { 
        "vorbis/lib/vorbisenc.c", 
        "/vorbis/win32/vorbisenc.def" 
    }

    includedirs { 
        "vorbis/include", 
        "ogg/include", 
        "%{cfg.targetdir}" 
    }


project "vorbisfile"
    language "C++"
    kind "StaticLib"
    targetname "vorbisfile"   
    warnings "Off"

    vpaths {
        ["Sources/*"] = {"vorbis/**.c",},
        ["*"] = {"premake5.lua", "CMakeLists.txt"}
    }

    files { 
        "vorbis/lib/vorbisfile.c", 
        "/vorbis/win32/vorbisfile.def" 
    }

    includedirs { 
        "vorbis/include", 
        "ogg/include", 
        "%{cfg.targetdir}" 
    }

project "imgui"
    language "C++"
    kind "StaticLib"
    targetname "imgui" 
    warnings "Off"

    vpaths {
        ["Headers/*"] = {"imgui/**.h*",},
        ["Sources/*"] = {"imgui/**.c*",},
        ["*"] = {"premake5.lua", "CMakeLists.txt"}
    }

    files {
        "imgui/backends/imgui_impl_win32.h",
        "imgui/backends/imgui_impl_win32.cpp",
        "imgui/backends/imgui_impl_dx9.h",
        "imgui/backends/imgui_impl_dx9.cpp",
        "imgui/misc/cpp/*.*",
        "imgui/*.*",
    }

    includedirs { 
        "imgui", 
        "imgui/backends", 
        "imgui/misc/cpp" 
    }

project "spdlog"
    cppdialect "C++20"
    kind "StaticLib"
    targetname "spdlog"
    warnings "Off"

    vpaths {
        ["Headers/*"] = {"spdlog/include/spdlog/**.*",},
        ["Sources/*"] = {"spdlog/src/**.c*",},
        ["*"] = {"premake5.lua", "CMakeLists.txt"}
    }

    files {
        "spdlog/src/spdlog.cpp", 
        "spdlog/src/stdout_sinks.cpp", 
        "spdlog/src/color_sinks.cpp", 
        "spdlog/src/file_sinks.cpp", 
        "spdlog/src/async.cpp", 
        "spdlog/src/cfg.cpp",
        "spdlog/include/**"
    }

    includedirs {
        "spdlog/include"
    }

    defines {
        "SPDLOG_COMPILED_LIB"
    }
    
project "tracy"
    language "C++"
    kind "StaticLib"
    targetname "tracy"
    warnings "Off"

    vpaths {
        ["Headers/*"] = {"tracy/**.h*",},
        ["Sources/*"] = {"tracy/**.cpp",},
        ["*"] = {"premake5.lua", "CMakeLists.txt"}
    }

    includedirs {
        "tracy/public/",
    }

    files {
        "tracy/public/TracyClient.cpp",
        
        -- tracy_includes
        "tracy/public/tracy/TracyC.h",
        "tracy/public/tracy/Tracy.hpp",
        "tracy/public/tracy/TracyD3D11.hpp",
        "tracy/public/tracy/TracyD3D12.hpp",
        "tracy/public/tracy/TracyLua.hpp",
        "tracy/public/tracy/TracyOpenCL.hpp",
        "tracy/public/tracy/TracyOpenGL.hpp",
        "tracy/public/tracy/TracyVulkan.hpp",

        -- client_includes
        "tracy/public/client/tracy_concurrentqueue.h",
        "tracy/public/client/tracy_rpmalloc.hpp",
        "tracy/public/client/tracy_SPSCQueue.h",
        "tracy/public/client/TracyArmCpuTable.hpp",
        "tracy/public/client/TracyCallstack.h",
        "tracy/public/client/TracyCallstack.hpp",
        "tracy/public/client/TracyCpuid.hpp",
        "tracy/public/client/TracyDebug.hpp",
        "tracy/public/client/TracyDxt1.hpp",
        "tracy/public/client/TracyFastVector.hpp",
        "tracy/public/client/TracyLock.hpp",
        "tracy/public/client/TracyProfiler.hpp",
        "tracy/public/client/TracyRingBuffer.hpp",
        "tracy/public/client/TracyScoped.hpp",
        "tracy/public/client/TracyStringHelpers.hpp",
        "tracy/public/client/TracySysPower.hpp",
        "tracy/public/client/TracySysTime.hpp",
        "tracy/public/client/TracySysTrace.hpp",
        "tracy/public/client/TracyThread.hpp",

        -- common_includes
        "tracy/public/common/tracy_lz4.hpp",
        "tracy/public/common/tracy_lz4hc.hpp",
        "tracy/public/common/TracyAlign.hpp",
        "tracy/public/common/TracyAlloc.hpp",
        "tracy/public/common/TracyApi.h",
        "tracy/public/common/TracyColor.hpp",
        "tracy/public/common/TracyForceInline.hpp",
        "tracy/public/common/TracyMutex.hpp",
        "tracy/public/common/TracyProtocol.hpp",
        "tracy/public/common/TracyQueue.hpp",
        "tracy/public/common/TracySocket.hpp",
        "tracy/public/common/TracyStackFrames.hpp",
        "tracy/public/common/TracySystem.hpp",
        "tracy/public/common/TracyUwp.hpp",
        "tracy/public/common/TracyYield.hpp",
    }

    defines {
        "TRACY_ENABLE",
        "TRACY_CALLSTACK",
        "TRACY_ON_DEMAND",
        --"TRACY_NO_CODE_TRANSFER" -- Uncomment if you want callstacks to be working
    }

project "json"
    language "C++"
    kind "None"
    targetname "json"
    warnings "Off"

    vpaths {
        ["Headers/*"] = {"json/include/**.hpp",},
        ["Sources/*"] = {"json/include/**.c*",},
        ["*"] = {"premake5.lua", "CMakeLists.txt"}
    }

    files {
        "json/include/*.*",
    }

    includedirs { 
        "json/include",
    }
