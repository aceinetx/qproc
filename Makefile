.ONESHELL:
.PHONY: web qvm

web: web/web.c
	emcc web/web.c src/qas/assembler.c src/qas/as_lexer.c src/qvm/endian.c -o web_dist/web.js -Isrc -Isrc/qas -Isrc/qvm

qvm:
	mkdir build -p && cd build
	cmake .. && make
