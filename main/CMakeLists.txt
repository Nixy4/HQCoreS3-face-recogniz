set(src_dirs        .)
set(include_dirs    .)
set(embed_files "www/index_ov2640.html.gz"
                "www/index_ov3660.html.gz"
                "www/index_ov5640.html.gz"
                "www/monitor.html.gz")

idf_component_register(
  SRC_DIRS ${src_dirs} 
  INCLUDE_DIRS ${include_dirs}
  EMBED_FILES ${embed_files}
  )

component_compile_options(-ffast-math -O3)

# -ffast-math：
# -ffast-math 是一个 GCC 编译器选项，用于启用一组优化选项，以提高浮点运算的性能。
# 启用 -ffast-math 后，编译器会进行一些不严格遵循 IEEE 754 标准的优化，例如假设浮点运算没有异常、允许重新排序浮点运算等。
# 这些优化可以提高浮点运算的性能，但可能会导致结果的精度降低或行为不符合标准。

# -O3：
# -O3 是一个 GCC 编译器选项，用于启用最高级别的优化。
# 启用 -O3 后，编译器会尝试进行各种优化，以生成运行速度最快的代码。这些优化包括循环展开、函数内联、向量化等。
# -O3 优化级别可能会增加编译时间和生成的代码大小，但通常可以显著提高代码的运行性能。
# 注意事项
# 性能与精度权衡：使用 -ffast-math 选项时，需要注意性能与精度之间的权衡。如果应用对浮点运算的精度要求较高，可能需要谨慎使用该选项。
# 代码大小：-O3 优化级别可能会增加生成的代码大小。如果代码大小是一个重要的考虑因素，可以考虑使用较低的优化级别（如 -O2）。