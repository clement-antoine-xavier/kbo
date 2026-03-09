# Windows-specific target configuration.
function(kbo_apply_platform_config target_name)
    target_compile_definitions(${target_name} PRIVATE
        KBO_PLATFORM_WINDOWS
        $<$<CONFIG:Debug>:DEBUG>
        $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
    )

    if(MSVC)
        target_compile_options(${target_name} PRIVATE
            /W4
            $<$<CONFIG:Debug>:/Od /Zi>
            $<$<CONFIG:Release>:/O2>
            $<$<CONFIG:RelWithDebInfo>:/O2 /Zi>
            $<$<CONFIG:MinSizeRel>:/O1>
        )

        target_link_options(${target_name} PRIVATE
            $<$<CONFIG:Debug>:/DEBUG>
            $<$<CONFIG:RelWithDebInfo>:/DEBUG>
        )
    elseif(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(${target_name} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            $<$<CONFIG:Debug>:-O0 -g3 -fno-omit-frame-pointer>
            $<$<CONFIG:Release>:-O3>
            $<$<CONFIG:RelWithDebInfo>:-O2 -g>
            $<$<CONFIG:MinSizeRel>:-Os>
        )

        target_link_options(${target_name} PRIVATE
            $<$<CONFIG:Debug>:-g3>
            $<$<CONFIG:RelWithDebInfo>:-g>
        )
    endif()
endfunction()
