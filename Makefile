

build-emscripten:
	emcc mmultiplication.cpp openglprogram.cpp -std=c++11 -s WASM=0 -s USE_SDL=2 -s USE_WEBGL2=1 -s FULL_ES3=1 -s USE_GLFW=3 -s ALLOW_MEMORY_GROWTH=1 -o mmultiplication.js

# build-osx:
#	g++ sdl.cpp -lSDL2 -lglew -framework OpenGLv2 -framework GLUT -std=c++11  -I /usr/local/Cellar/emscripten/1.38.42_1/libexec/system/include/ -L /usr/local/Cellar/emscripten/1.38.42_1/libexec/system/lib

build:
	g++ mmultiplication.cpp openglprogram.cpp -lSDL2 -lGLESv2 -lGLEW -std=c++11 -o mmultiplication

# npm http-server
http-server:
	http-server
