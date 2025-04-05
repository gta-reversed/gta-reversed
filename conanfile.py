import os

from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.files import copy

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
        "libjpeg-turbo/3.1.0",
    ]

    default_options = {
        "with_command_hooks": False,
    }

    options = {
        "with_command_hooks": [True, False],
    }

    def layout(self):
        cmake_layout(self)
    
    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.user_presets_path = 'ConanPresets.json'
        if self.options['with_command_hooks']:
            tc.cache_variables["GTASA_WITH_SCRIPT_COMMAND_HOOKS"] = "ON"
        tc.generate()

        copy(self, "*win32*", os.path.join(self.dependencies["imgui"].package_folder,
            "res", "bindings"), os.path.join(self.source_folder, "source", "app"))
        copy(self, "*dx9*", os.path.join(self.dependencies["imgui"].package_folder,
            "res", "bindings"), os.path.join(self.source_folder, "source", "app"))
        
        copy(self, "imgui_stdlib.*", os.path.join(self.dependencies["imgui"].package_folder,
            "res", "misc", "cpp"), os.path.join(self.source_folder, "source", "app"))

