from conan import ConanFile
from conan.tools.cmake import cmake_layout


class HCC(ConanFile):
    name = "HCC"
    version = "0.0.0"
    settings = ["os", "compiler", "build_type", "arch"]
    author = "HyperCPU Project"
    requires = [
        "gtest/1.14.0",
        "fmt/11.0.2",
        "flex/2.6.4",
        "bison/3.8.2",
        "eternal/1.0.1",
        "tl-expected/20190710"
    ]
    generators = ["CMakeToolchain", "CMakeDeps"]

    def build_requirements(self):
        self.tool_requires("flex/2.6.4")
        self.tool_requires("bison/3.8.2")

    def layout(self) -> None:
        cmake_layout(self)
