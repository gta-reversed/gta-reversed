import os
import shutil

from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.files import copy

USE_SDL3 = True # TODO: Add an CLi option to toggle this

class saRecipe(ConanFile):
    name = "gta-reversed"
    version = "0.0"

    settings = "os", "compiler", "build_type", "arch"
    exports_sources = "CMakeLists.txt", "source/*"

    requires = [
        "ogg/1.3.5",
        "nlohmann_json/3.11.3",
        "spdlog/1.15.0",
        "tracy/cci.20220130",
        "vorbis/1.3.7",
        "imgui/1.91.5-docking",
        "sdl/3.2.6"   
    ]

    def layout(self):
        cmake_layout(self)
    
    def generate(self):
        IMGUI_LIBS_FOLDER = os.path.join(self.source_folder, "source", "libs", "imgui")

        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        if USE_SDL3:
            tc.preprocessor_definitions['NOTSA_USE_SDL3'] = 1
        tc.user_presets_path = 'ConanPresets.json'
        tc.generate()

        # Copy ImGui bindings
        shutil.rmtree(IMGUI_LIBS_FOLDER)
        def copy_imgui_bindings(pattern):
            copy(
                self,
                pattern,
                os.path.join(self.dependencies["imgui"].package_folder, "res", "bindings"),
                os.path.join(IMGUI_LIBS_FOLDER, "bindings")
            )
        copy_imgui_bindings("*imgui_impl_sdl3*" if USE_SDL3 else "*imgui_impl_win32*")
        copy_imgui_bindings("*imgui_impl_dx9*")

        # Copy ImGui misc stuff
        copy(
            self,
            "*",
            os.path.join(self.dependencies["imgui"].package_folder, "res", "misc", "cpp"),
            os.path.join(IMGUI_LIBS_FOLDER, "misc", "cpp")
        )
