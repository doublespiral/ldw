all:
	python3 build.py


plugins:
install-plugins: plugins
	if [ ! -d "$$HOME/.cache/ldw/plugins" ]; then \
		mkdir "$$HOME/.cache/ldw/plugins"; \
	fi
	cp -r ${BIN}/plugins $$HOME/.cache/ldw/


install:
	install ldw        '/usr/local/bin'
	install ldw-daemon '/usr/local/bin'

uninstall:
	rm -f '/usr/local/bin/ldw'
	rm -f '/usr/local/bin/ldw-daemon'


clean:
	rm -f ldw-daemon
	rm -f ldw


#install-local:
#	mkdir "$$HOME/.ldw"
#	install ldw "$$HOME/.ldw"
#
#uninstall-local:
#	rm -rf "$$HOME/.ldw"







# old one
#INCLUDE = -Iinclude
#CFLAGS = ${INCLUDE} -std=c99 -ggdb -Wall -Wextra
#LINKMA = -lX11 -lGL
##-lGLEW
#
#all: xwindow main
#	cc -o main bin/xwindow.o bin/main.o ${LINKMA} ${CFLAGS}
#
#
#main: source/main.c include/ldw.h
#	cc -o bin/main.o -c source/main.c ${CFLAGS}
#
#xwindow: source/xwindow.c source/xwindow.h include/ldw.h
#	cc -o bin/xwindow.o -c source/xwindow.c ${CFLAGS}
#
#
##IMGUI_REPO := https://github.com/ocornut/imgui.git
##IMGUI_DIR := third_party/imgui
##
##all: imgui YourProjectName
##
##imgui:
##    git clone $(IMGUI_REPO) $(IMGUI_DIR)
##
##YourProjectName: main.o
##    $(CC) -o YourProjectName main.o -L$(IMGUI_DIR) -limgui
##
##main.o: main.c
##    $(CC) }