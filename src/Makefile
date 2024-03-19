WWW = -Wall -Werror -Wextra -std=c11
LC=-lcheck
GCOV=-fprofile-arcs -ftest-coverage
UNIX = -lsubunit -lrt -lpthread -lm
SRC=back/back.c test.c
OS=$(shell uname -s)
TARGET=3DviewerTK

ifeq ($(OS),Linux)
	OPEN_CMD = xdg-open
endif
ifeq ($(OS),Darwin)
	OPEN_CMD = open
endif

all: clean install

install: 
	mkdir -p ../build
	cd front && qmake && make && make clean && rm -rf Makefile && mv $(TARGET).app ../../build/$(TARGET).app
	cp -R ../build/$(TARGET).app ~/Desktop/$(TARGET).app

uninstall:
	rm -rf ~/Desktop/$(TARGET).app ../build

dvi: 
	open ../README.md 

dist: 
	mkdir -p ../dist
	cp -R ../build/$(TARGET).app ../dist
	cd ../dist && tar cvzf $(TARGET)_v1.0.tgz *
	cd ../dist && rm -rf $(TARGET).app

test: 
ifeq ($(OS), Darwin)
	gcc $(WWW) $(SRC) -o test.out $(LC) -lm
else
	gcc $(WWW) $(SRC) -o test.out $(LC) $(UNIX)
endif
	./test.out

gcov_report: clean
ifeq ($(OS), Darwin)
	gcc $(WWW) $(GCOV) $(SRC) -o test.out $(LC)
else
	gcc $(WWW) $(GCOV) $(SRC) -o test.out $(LC) $(UNIX)
endif
	./test.out
	lcov -t "test" -o test.info -c -d .
	genhtml -o report test.info
	$(OPEN_CMD) ./report/index.html
	make clean_test

style:
	clang-format -style=Google -n **/*.c **/*.h **/*.cpp
	
clean: clean_test
	rm -rf *.o back/*.o *.a report ../build ../dist

clean_test:
	rm -rf *.gcda *.gcno test.out *.info
