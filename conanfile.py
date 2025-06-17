from conan import ConanFile
from conan.tools.cmake import CMakeDeps, CMakeToolchain, cmake_layout


class OctaCoreConan(ConanFile):
    name = "octa-core"
    version = "1.0.0"
    
    # Package metadata
    description = "Octa-Core C++ game logic library"
    homepage = "https://github.com/robertocastro/octa-core-cpp"
    license = "MIT"
    
    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "tests/*", "bench/*", "cmake/*"

    # Options
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "thread_safe": [True, False],
        "enable_testing": [True, False],
        "enable_benchmarks": [True, False],
    }
    
    default_options = {
        "shared": False,
        "fPIC": True,
        "thread_safe": False,
        "enable_testing": True,
        "enable_benchmarks": False,
    }

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"

    def configure(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def requirements(self):
        # Core dependencies
        self.requires("spdlog/1.12.0")
        
        # Testing dependencies
        if self.options.enable_testing:
            self.test_requires("gtest/1.14.0")
        
        # Benchmarking dependencies  
        if self.options.enable_benchmarks:
            self.test_requires("benchmark/1.8.3")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        
        tc = CMakeToolchain(self)
        tc.variables["OCTA_THREAD_SAFE"] = self.options.thread_safe
        tc.variables["BUILD_TESTS"] = self.options.enable_testing
        tc.variables["BUILD_BENCHMARKS"] = self.options.enable_benchmarks
        tc.generate()

    def build(self):
        from conan.tools.cmake import CMake
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        from conan.tools.cmake import CMake
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["octa_core"]
        self.cpp_info.includedirs = ["include"]
        
        if self.options.thread_safe:
            self.cpp_info.defines.append("OCTA_THREAD_SAFE")
            
        if self.settings.os in ["Linux", "FreeBSD"]:
            self.cpp_info.system_libs.append("pthread") 