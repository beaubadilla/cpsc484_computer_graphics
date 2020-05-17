
CFLAGS = -std=c++17 `pkg-config --cflags glew glfw3 glm`
LDFLAGS = `pkg-config --libs glew glfw3 glm`

rasterdemo: libraries rasterdemo.cpp shader.o
	clang++ $(CFLAGS) rasterdemo.cpp shader.o -o rasterdemo $(LDFLAGS)

shader.o: libraries shader.hpp shader.cpp
	clang++ $(CFLAGS) -c shader.cpp -o shader.o

clean:
	rm -f rasterdemo shader.o 

libraries: /usr/include/GL/gl.h /usr/include/GL/glew.h /usr/include/GLFW/glfw3.h /usr/include/glm/glm.hpp

/usr/include/GL/gl.h /usr/include/GL/glew.h /usr/include/GLFW/glfw3.h /usr/include/glm/glm.hpp:
	@echo "OpenGL libraries not installed, installing. Please provide the password when asked\n"
	sudo apt-get -y install mesa-common-dev libglew-dev libglfw3-dev libglm-dev
