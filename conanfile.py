# pylint: disable=line-too-long,missing-module-docstring,missing-class-docstring,missing-function-docstring,trailing-whitespace

import os, shutil

from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeConfigDeps
from conan.tools.files import copy

class saRecipe(ConanFile):
    name = "gta-reversed"
    version = "1.0"

    settings = ("os", "compiler", "build_type", "arch")
    exports_sources = "CMakeLists.txt", "source/*"

    tool_requires = (
        "cmake/[>=4.2.0]", # We need CMake 4 to avoid compatibility issues
    )

    options = {
        # Enable or disable script command hooks
        "with_script_command_hooks": [True, False],

        # Use SDL3 or DInput
        "use_sdl3": [True, False],
  
        # Use Unity build mode
        "unity_build": [True, False],

        # Standalone build mode
        "standalone": [False, 'dump_hooks_only'], # There's no `True` yet, as we can't build a fully standalone version yet
    }

    default_options = {
        "with_script_command_hooks": False,
        "use_sdl3": True,
        "standalone": False,
        "unity_build": False,
    }
    
    def requirements(self):
        # Required for our custom stuff, and optionally in vanilla with SDL3
        if self.options.use_sdl3:
            self.requires("sdl/3.4.14")

        # Required for our custom stuff
        self.requires("nlohmann_json/3.12.0")
        self.requires("spdlog/1.17.0", options={"use_std_fmt": True})
        self.requires("tracy/cci.20220130")
        self.requires("imgui/1.92.9b-docking")
        
        # Required for vanilla build
        self.requires("ogg/1.3.5")
        self.requires("vorbis/1.3.7")
        self.requires("libjpeg-turbo/3.2.0")

    def layout(self):
        cmake_layout(self)
    
    def generate(self):
        deps = CMakeConfigDeps(self)
        deps.generate()
        
        tc = CMakeToolchain(self) 
        tc.user_presets_path = 'ConanPresets.json'
        tc.cache_variables["GTASA_STANDALONE_DUMP_HOOKS_ONLY"] = self.options.standalone == 'dump_hooks_only'
        tc.cache_variables["GTASA_STANDALONE"] = bool(self.options.standalone)
        tc.cache_variables["GTASA_WITH_SCRIPT_COMMAND_HOOKS"] = self.options.with_script_command_hooks
        tc.cache_variables["GTASA_USE_SDL3"] = self.options.use_sdl3
        tc.cache_variables["GTASA_UNITY_BUILD"] = self.options.unity_build
        tc.generate()

        # Copy ImGui bindings, and misc stuff
        imgui_libs_folder = os.path.join(self.source_folder, "source", "libs", "imgui")
        try:
            shutil.rmtree(imgui_libs_folder)
        except FileNotFoundError:
            pass
        def copy_imgui_bindings(pattern):
            copy(
                self,
                pattern,
                os.path.join(self.dependencies["imgui"].package_folder, "res", "bindings"),
                os.path.join(imgui_libs_folder, "bindings")
            )
        copy_imgui_bindings("*imgui_impl_sdl3*" if self.options['use_sdl3'] else "*imgui_impl_win32*")
        copy_imgui_bindings("*imgui_impl_dx9*")
        copy(
            self,
            "*",
            os.path.join(self.dependencies["imgui"].package_folder, "res", "misc", "cpp"),
            os.path.join(imgui_libs_folder, "misc", "cpp")
        )

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
