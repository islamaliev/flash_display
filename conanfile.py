from conans import ConanFile, CMake


class FlashDisplayConan(ConanFile):
    name = "flash_display"
    version = "0.1"
    # license = "<Put the package license here>"
    # url = "<Package recipe repository url here"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = "shared=False"
    generators = "cmake"
    exports_sources = "*"

    #def source(self):
        #self.run("git clone https://github.com/islamaliev/flash_display.git")
        #self.run("cd flash_display && git checkout master")

    def requirements(self):
        self.requires("flash_math/0.1@islamaliev/testing")
        self.requires("glew/2.1.0@bincrafters/stable")
        self.requires("glfw/3.2.1@bincrafters/stable")
        self.requires("libpng/1.6.34@bincrafters/stable")
        self.requires("libjpeg/9c@bincrafters/stable")

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
        libs = ['flash_display']

        self.cpp_info.libs = []
        for lib in libs:
            if self.settings.os == "Windows" and self.settings.build_type == "Debug":
                suffix = "d"
            else:
                suffix = ""
            self.cpp_info.libs += ["%s%s" % (lib, suffix)]
