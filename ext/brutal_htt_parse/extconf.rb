# frozen_string_literal: true

require "mkmf"

require "mini_portile2"
recipe = MiniPortileCMake.new("llhttp", "9.2.1")
recipe.files = %w[https://github.com/nodejs/llhttp/archive/refs/tags/release/v9.2.1.tar.gz]

recipe.cook
recipe.activate

dir_config("llhttp", recipe.include_path, recipe.lib_path)
$LDFLAGS << " -L#{recipe.lib_path} -Wl,-rpath,#{recipe.lib_path}"

have_header("llhttp.h") || abort("Missing llhttp")
have_library("llhttp") ||  abort("Missing llhttp")

# Makes all symbols private by default to avoid unintended conflict
# with other gems. To explicitly export symbols you can use RUBY_FUNC_EXPORTED
# selectively, or entirely remove this flag.
append_cflags("-fvisibility=hidden")
append_cflags("-g -O0") if ENV["DEBUG"] == "1"
create_makefile("brutal_htt_parse/brutal_htt_parse")
