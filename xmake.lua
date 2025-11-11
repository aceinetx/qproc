add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "build"}) -- generate compile commands
set_languages("c90")
set_warnings("all")

add_includedirs("include/qvm", "include/qas", "include/qcc", "include")

target("qproc_core")
    set_kind("static")
    add_files("src/arena.c", "src/util.c", "src/qvm/endian.c")

target("qvm")
    set_kind("binary")
    add_files("src/qvm/*.c")
    add_deps("qproc_core")

target("qcc")
    set_kind("binary")
    add_files("src/qcc/*.c")
    add_deps("qproc_core")

target("qas")
    set_kind("binary")
    add_files("src/qas/*.c")
    add_deps("qproc_core")
