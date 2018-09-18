from conans import ConanFile, CMake, tools


class LibossiaConan(ConanFile):
    name = "libossia"
    version = "1.0.0"
    license = "CeCILL-C"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "A modern C++, cross-environment distributed object model for creative coding and interactive art."
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = "shared=False"
    generators = "cmake"

    def source(self):
        self.run("git clone https://github.com/OSSIA/libossia.git")
        self.run("cd libossia")
        
    def configure_cmake(self):
        cmake = CMake(self)
        cmake.definitions["SOME_DEFINITION"] = True
        cmake.configure()
    
        return cmake

    def build(self):
        cmake = self.configure_cmake()
        cmake.build()


    def package(self):
        cmake = self.configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["libossia"]

