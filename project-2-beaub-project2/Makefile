
CLANG_FLAGS = -std=c++17 -Wall -O -g
GTEST_FLAGS = -lpthread -lgtest_main -lgtest  -lpthread

print_score: rubricscore gfxalgebra_rubric.json gfxalgebra_test.xml
	./rubricscore gfxalgebra_rubric.json gfxalgebra_test.xml

rubricscore: rubricscore.cpp
		clang++ ${CLANG_FLAGS} rubricscore.cpp -o rubricscore

gfxalgebra_test.xml: gfxalgebra_test
	# || true allows make to continue the build even if some tests fail
	./gfxalgebra_test --gtest_output=xml:./gfxalgebra_test.xml || true

gfxalgebra_test: headers libraries gfxalgebra_test.cpp
	clang++ ${CLANG_FLAGS} ${GTEST_FLAGS} gfxalgebra_test.cpp -o gfxalgebra_test

headers: gfxnumeric.hpp gfxalgebra.hpp

libraries: /usr/lib/libgtest.a

/usr/lib/libgtest.a:
	@echo -e "google test library not installed\n"
	@echo -e "Installing libgtest-dev. Please provide the password when asked\n"
	@sudo apt-get -y install libgtest-dev cmake
	@sudo apt-get install cmake # install cmake
	@echo -e "\nConfiguring libgtest-dev\n"
	@cd /usr/src/gtest; sudo cmake CMakeLists.txt; sudo make; sudo cp *.a /usr/lib
	@echo -e "Finished installing google test library\n"

clean:
		rm -f rubricscore gfxalgebra_test gfxalgebra_test.xml
