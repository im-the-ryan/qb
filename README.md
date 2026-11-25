# qb - quickbuild

This is a very basic build tool inspired by Tsoding's [nob.h](https://github.com/tsoding/nob.h). It differs from other header file build tools by instead using qb.h as the build commands and qb.c as the actual build tool.

The project is NOT recommended for any of your own projects at the moment. It currently lacks any features and probably has a bunch of bugs.

## Usage
Clone qb.c and qb.h into your project. Configure qb.h (see [Configuration](##Configuration)). Then run:
```
gcc qb.c -o qb
./qb
```

## Configuration
qb currently supports 7 different build options.

First define your target (compulsory)
```
#define QB_TARGET "project name here"
```

Define include directory. Currently only supports one include directory.
```
#define QB_INCLUDE "include directory here"
```

Define all source files. List all source directories relative to where you are compile qb. If multiple directories are used, seperate them with a space. For example ```"src/main.c src/utils.c"```.
```
#define QB_SRC_FILES "source files here"
```

Define C flags. Flags should be written like normal flags, beginning with a ```-``` and separated with a space (```"-flag1 -flag2 -flag3"```). These flags will be used for all C sources.
```
#define QB_CFLAGS "c flags"
```

Define C++ flags. Flags should be written like normal flags, beginning with a ```-``` and separated with a space (```"-flag1 -flag2 -flag3"```). These flags will be used for all C++ sources.
```
#define QB_CXXFLAGS "c++ flags"
```

Define the build directory where the object files will be created. It is important to note that this directory must be created before running ```./qb``` otherwise it will raise an error. Automatic build directory creation will be implemented soon.
```
#define QB_BUILD_DIR "build directory"
```

Define linking flags. Flags should be written like normal flags, beginning with a ```-``` and separated with a space (```"-flag1 -flag2 -flag3"```).
```
#define QB_LDFLAGS "linking flags"
```