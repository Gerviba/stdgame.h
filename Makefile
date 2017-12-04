all:
	echo "Make arguments: compile, gendocs, countlines"
	
compile:
	mkdir -p build
	mkdir -p build/src
	cd build; \
	gcc -I/usr/include/GL -I/usr/include/SOIL -I/usr/include/GLFW -O0 -g3 -Wall -c -fmessage-length=0 -Wimplicit-function-declaration -MMD -MP -MF"src/stdgame.d" -MT"src/stdgame.o" -o "src/stdgame.o" "../src/stdgame.c"; \
	gcc -I/usr/include/GL -I/usr/include/SOIL -I/usr/include/GLFW -O0 -g3 -Wall -c -fmessage-length=0 -Wimplicit-function-declaration -MMD -MP -MF"src/components.d" -MT"src/components.o" -o "src/components.o" "../src/components.c"; \
	gcc -I/usr/include/GL -I/usr/include/SOIL -I/usr/include/GLFW -O0 -g3 -Wall -c -fmessage-length=0 -Wimplicit-function-declaration -MMD -MP -MF"src/events.d" -MT"src/events.o" -o "src/events.o" "../src/events.c"; \
	gcc -I/usr/include/GL -I/usr/include/SOIL -I/usr/include/GLFW -O0 -g3 -Wall -c -fmessage-length=0 -Wimplicit-function-declaration -MMD -MP -MF"src/font.d" -MT"src/font.o" -o "src/font.o" "../src/font.c"; \
	gcc -I/usr/include/GL -I/usr/include/SOIL -I/usr/include/GLFW -O0 -g3 -Wall -c -fmessage-length=0 -Wimplicit-function-declaration -MMD -MP -MF"src/game.d" -MT"src/game.o" -o "src/game.o" "../src/game.c"; \
	gcc -I/usr/include/GL -I/usr/include/SOIL -I/usr/include/GLFW -O0 -g3 -Wall -c -fmessage-length=0 -Wimplicit-function-declaration -MMD -MP -MF"src/linkedlist.d" -MT"src/linkedlist.o" -o "src/linkedlist.o" "../src/linkedlist.c"; \
	gcc -I/usr/include/GL -I/usr/include/SOIL -I/usr/include/GLFW -O0 -g3 -Wall -c -fmessage-length=0 -Wimplicit-function-declaration -MMD -MP -MF"src/map.d" -MT"src/map.o" -o "src/map.o" "../src/map.c"; \
	gcc -I/usr/include/GL -I/usr/include/SOIL -I/usr/include/GLFW -O0 -g3 -Wall -c -fmessage-length=0 -Wimplicit-function-declaration -MMD -MP -MF"src/menu.d" -MT"src/menu.o" -o "src/menu.o" "../src/menu.c"; \
	gcc -I/usr/include/GL -I/usr/include/SOIL -I/usr/include/GLFW -O0 -g3 -Wall -c -fmessage-length=0 -Wimplicit-function-declaration -MMD -MP -MF"src/object.d" -MT"src/object.o" -o "src/object.o" "../src/object.c"; \
	gcc -I/usr/include/GL -I/usr/include/SOIL -I/usr/include/GLFW -O0 -g3 -Wall -c -fmessage-length=0 -Wimplicit-function-declaration -MMD -MP -MF"src/player.d" -MT"src/player.o" -o "src/player.o" "../src/player.c"; \
	gcc -I/usr/include/GL -I/usr/include/SOIL -I/usr/include/GLFW -O0 -g3 -Wall -c -fmessage-length=0 -Wimplicit-function-declaration -MMD -MP -MF"src/shader.d" -MT"src/shader.o" -o "src/shader.o" "../src/shader.c"; \
	gcc -Wimplicit-function-declaration -o "stdgame"  ./src/components.o ./src/events.o ./src/font.o ./src/game.o ./src/linkedlist.o ./src/map.o ./src/menu.o ./src/object.o ./src/player.o ./src/shader.o ./src/stdgame.o   -lGL -lSOIL -lX11 -lXrandr -lXinerama -lXi -lXxf86vm -lXcursor -ldl -lm -lpthread -lglfw -lglfw3

gendocs:
	doxygen doxygen.cfg
	
countlines:
	find src -name '*.*' | xargs wc -l

	