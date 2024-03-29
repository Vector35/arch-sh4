# arch-sh4

This is an experimental SH4 architecture plugin for Binary Ninja.

## What's What

- [arch_sh4.cpp](./arch_sh4.cpp) implements the Architecture class
- [disassembler/*](./disassembler/) is the disassembler
- [disassembler/disasm_test.py](./disassembler/disasm_test.py) tests disassembler in isolation
- [test_disasm.py](./test_disasm.py) tests disassembler through the binja/architecture plugin
- [sh4test.py](./sh4test.py) tests some lifting

## Testing

- [disassembler/disasm_test.py](./disassembler/disasm_test.py) implements a very basic "disassemble to string and compare" set of unit tests

Personal Binary Ninja users can test via the built in console:

```
>>> sys.path.append('C:/users/x/documents/binja/arch-sh4') # Path directory containing sh4test.py
>>> from importlib import reload
>>> import sh4test
success!
>>> # Add or fix any testcases
>>> reload(sh4test)
success!
```

And, of course, you can open a test binary in Binary Ninja with this architecture built and activated to see if results are as expected.

## Pull Requests

Please follow whatever formatting conventions are present in the file you edit. Pay attention to curly brackets, spacing, tabs vs. spaces, etc.

If you're making an architecture or lifter change, add a test case to [sh4test.py](./sh4test.py) that fails before your change and succeeds after your change.

If you're making a disassembler change, add a test case to [disassembler/test.py](./disassembler/test.py) that fails before your change and succeeds after your change.

When you submit your first PR to one of Vector 35's repositories, you'll receive a notice from [CLA Assistant](https://cla-assistant.io/) that allows you to sign our [Contribution License Agreement](https://binary.ninja/cla.pdf) online. 


## Building

Building the architecture plugin requires `cmake` 3.9 or above. You will also need the
[Binary Ninja API source](https://github.com/Vector35/binaryninja-api).

First, set the `BN_API_PATH` environment variable to the path containing the
Binary Ninja API source tree.

Run `cmake <path>`. This can be done either from a separate build directory or from the source
directory. If your app is installed in a non-default location, set BN_INSTALL_DIR in your
cmake invocation, like `cmake -DBN_INSTALL_DIR=/Applications/Binary\ Ninja\ DEV.app/ <path>`.
Once that is complete, run `make` in the build directory to compile the plugin.

The plugin can be found in the root of the build directory as `libarch_sh4.so`,
`libarch_sh4.dylib` or `arch_sh4.dll` depending on your platform.

To install the plugin, first launch Binary Ninja and uncheck the "SH4 architecture plugin"
option in the "Core Plugins" section. This will cause Binary Ninja to stop loading the
bundled plugin so that its replacement can be loaded. Once this is complete, you can copy
the plugin into the user plugins directory (you can locate this by using the "Open Plugin Folder"
option in the Binary Ninja UI).

**Do not replace the architecture plugin in the Binary Ninja install directory. This will
be overwritten every time there is a Binary Ninja update. Use the above process to ensure that
updates do not automatically uninstall your custom build.**

## Build Example

### acquire repositories

```
mkdir ~/repos/vector35
cd ~/repos/vector35
git clone git@github.com:Vector35/binaryninja-api.git
git clone git@github.com:Vector35/arch-sh4.git
```

### environment variables

`export BN_API_PATH=~/repos/vector35/binaryninja-api`

### cmake, make

```
cd arch-sh4
cmake -DBN_INSTALL_DIR=/Applications/Binary\ Ninja\ DEV.app/ .
make
```

## Build Troubleshooting

### example

    CMake Error at CMakeLists.txt:8 (message):
      Provide path to Binary Ninja API source in BN_API_PATH
    resolution:
    ensure BN_API_PATH is in your environment

### example

    CMake Error at /Users/andrewl/repos/vector35/binaryninja-api/CMakeLists.txt:53 (message):
      Binary Ninja Core Not Found
    resolution:
    ensure BN_INSTALL_DIR is supplied at command line invocation of cmake
    ensure some bad directory is not cached in CMakeCache.txt

### example

    cmake seems to ignore your setting of BN_INSTALL_DIR and other cmake variables
    resolution:
    rm CMakeCache.txt

### example

    undefined symbols at link time, like:
    Undefined symbols for architecture x86_64:
      "_BNClearUserVariableValue", referenced from:
      BinaryNinja::Function::ClearUserVariableValue(BinaryNinja::Variable const&, unsigned long long) in libbinaryninjaapi.a(function.cpp.o)
    resolution:
    ensure that your api repo is on the same channel and at the same commit as the libbinaryninjacore you're linking against
    eg: binaryninja is on dev update channel and is up-to-date and binaryninja-api repo is on branch dev with latest pulled

