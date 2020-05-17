
CLANG_FLAGS = -std=c++17 -Wall -O -g
PNG_FLAGS = `libpng-config --cflags --ldflags`
GTEST_FLAGS = -lpthread -lgtest_main -lgtest  -lpthread

print_score: rubricscore rasterize_rubric.json rasterize_test.xml
	./rubricscore rasterize_rubric.json rasterize_test.xml

rubricscore: rubricscore.cpp
		clang++ ${CLANG_FLAGS} rubricscore.cpp -o rubricscore

rasterize_test.xml: rasterize_test
	# || true allows make to continue the build even if some tests fail
	./rasterize_test --gtest_output=xml:./rasterize_test.xml || true

rasterize_test: headers libraries rasterize_test.cpp
	clang++ ${CLANG_FLAGS} ${PNG_FLAGS} ${GTEST_FLAGS} rasterize_test.cpp -o rasterize_test

headers: gfxnumeric.hpp gfximage.hpp gfxpng.hpp gfxrasterize.hpp

libraries: /usr/lib/libgtest.a /usr/include/png++/png.hpp

/usr/lib/libgtest.a:
	@echo -e "google test library not installed\n"
	@echo -e "Installing libgtest-dev. Please provide the password when asked\n"
	@sudo apt-get -y install libgtest-dev cmake
	@sudo apt-get install cmake # install cmake
	@echo -e "\nConfiguring libgtest-dev\n"
	@cd /usr/src/gtest; sudo cmake CMakeLists.txt; sudo make; sudo cp *.a /usr/lib
	@echo -e "Finished installing google test library\n"

/usr/include/png++/png.hpp:
	@echo -e "libpng++ library not installed\n"
	@echo -e "Installing libpng++-dev. Please provide the password when asked\n"
	@sudo apt-get -y install libpng++-dev

make_images: headers libraries make_images.cpp
	clang++ ${CLANG_FLAGS} ${PNG_FLAGS} make_images.cpp -o make_images

images: make_images
	./make_images

clean:
		rm -f rubricscore rasterize_test test.png rasterize_test.xml make_images got*png
