# point this to wherever your app is installed, probably /Applications/BinaryNinja.app
PATH_BINJA_APP = $(HOME)/repos/vector35/binaryninja/ui/binaryninja.app/Contents/MacOS

# point this to wherever you cloned https://github.com/Vector35/binaryninja-api and built
PATH_API = $(HOME)/repos/vector35/binaryninja/api

# point this to your plugin path
PATH_PLUGINS = $(HOME)/Library/Application Support/Binary Ninja/plugins

all: arch_sh4.dylib

arch_sh4.dylib: arch_sh4.cpp
	g++ -std=gnu++11 -I$(PATH_API) arch_sh4.cpp $(PATH_API)/libbinaryninjaapi.a $(PATH_BINJA_APP)/libbinaryninjacore.dylib -fPIC -shared -o arch_sh4.dylib

clean:
	rm -f arch_sh4.dylib

install:
	cp arch_sh4.dylib "$(PATH_PLUGINS)"

uninstall:
	rm "$(PATH_PLUGINS)/arch_sh4.dylib"
