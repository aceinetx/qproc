.ONESHELL:
.PHONY: web qvm

web:
	cd web
	emcc web.c -o ../web_dist/web.js

qvm:
	mkdir build -p && cd build
	cmake .. && make
