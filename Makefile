.ONESHELL:
.PHONY: web qvm

web: web/web.c src/qas/assembler.c src/qas/as_lexer.c src/qvm/endian.c
	emcc -Isrc -Isrc/qas -Isrc/qvm -sEXPORTED_FUNCTIONS=_all_create,_all_destroy,_assemble,_get_assembled_bytes,_get_assembler_logs,_main -s MODULARIZE=1 -s EXPORT_NAME="Module" -sEXPORTED_RUNTIME_METHODS=ccall,cwrap -o web_dist/web.js web/web.c src/qas/assembler.c src/qas/as_lexer.c src/qvm/endian.c

qvm:
	mkdir build -p && cd build
	cmake .. && make
