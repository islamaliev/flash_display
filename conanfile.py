from conans import ConanFile, CMake


class FlashmathConan(ConanFile):
    name = "flash_display"
    version = "0.1"
    # license = "<Put the package license here>"
    # url = "<Package recipe repository url here"
    settings = "os", "compiler", "build_type", "arch"
    #requirements = [
        #"flash_math/0.1@islamaliev/testing",
        #"glew/2.0.0@lasote/vcpkg",
        #"libpng/1.6.23@lasote/stable"
    #]
    options = {"shared": [True, False]}
    default_options = "shared=False"
    generators = "cmake"
    exports_sources = "*"

    #def source(self):
        #self.run("git clone https://github.com/islamaliev/flash_display.git")
        #self.run("cd flash_display && git checkout master")

    def requirements(self):
        self.requires("flash_math/0.1@islamaliev/testing")
        self.requires("glew/2.0.0@lasote/vcpkg")
        self.requires("glfw3/3.2.1@lasote/vcpkg")
        self.requires("libpng/1.6.23@lasote/stable")
        self.requires("libjpeg/9a@Kaosumaru/stable")

    def build(self):
        cmake = CMake(self)
        #shared = "-DBUILD_SHARED_LIBS=ON" if self.options.shared else ""
        #self.run('cmake flash_display %s %s' % (cmake.command_line, shared))
        #self.run("cmake --build . %s" % cmake.build_config)
        cmake.configure()
        cmake.build()

    def package(self):
        self.copy("*.h", dst="include", src="include")
        self.copy("*.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.dylib", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["flash_display"]
