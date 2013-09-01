LIBS = -framework Cocoa -framework OpenGL -lglfw -lBulletDynamics -lBulletCollision -lLinearMath
INCLUDES = -I/usr/local/include/bullet -Iinclude
CFLAGS = -g -std=c++11 -stdlib=libc++

all: engine samples

engine: include/engine/*.h src/*.cpp include/engine/components/*.h src/components/*.cpp include/engine/systems/*.h src/systems/*.cpp include/engine/utils/*.h src/utils/*.cpp
	clang++ -c $(CFLAGS) $(INCLUDES) $(LIBS) src/*.cpp src/components/*.cpp src/systems/*.cpp src/utils/*.cpp
	ar -r engine.a *.o

samples: monkey

monkey: engine samples/monkey/Monkey.cpp
	clang++ $(CFLAGS) $(INCLUDES) $(LIBS) -o monkeys engine.a samples/monkey/Monkey.cpp

clean:
	rm -f engine.a *.o monkeys