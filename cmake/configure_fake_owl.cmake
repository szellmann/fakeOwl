macro(fake_owl_compile_and_embed output_var file)

    set(targetName ${file})

    while (TARGET ${targetName})
        message("Oupssss")
        set(targetName ${targetName}X)
    endwhile()
        message(${targetName})

    add_library(${targetName} SHARED ${file})
    set_target_properties(${targetName} PROPERTIES LINKER_LANGUAGE "CXX")
    set_source_files_properties(${targetName} PROPERTIES LANGUAGE "CXX")
    target_link_libraries(${targetName} ${fakeOwl_LIBRARY})
    set_property(TARGET ${targetName} PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
    set_property(TARGET ${targetName} PROPERTY COMPILE_FLAGS "-lto")

    set(embedded_file ${CMAKE_CURRENT_BINARY_DIR}/fakeOwl_${targetName}_embedded.c)

    file(
        WRITE ${embedded_file}
        "#ifdef __cplusplus
        extern \"C\" {
        #endif

        const unsigned char ${output_var}[] = \"${CMAKE_CURRENT_BINARY_DIR}/lib${targetName}.dylib\";

        #ifdef __cplusplus
        }
        #endif"
    )

    set(${output_var} ${embedded_file})

endmacro()
