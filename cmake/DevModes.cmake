string(TOUPPER ${PROJECT_NAME} UC_PROJECT_NAME)

set(GNU_CXX_WARNINGS
    -Wall
    -pedantic
    -pedantic-errors
    -Wextra
    -Wcast-align
    -Wcast-qual
    -Wconversion
    -Wdisabled-optimization
    -Werror
    -Wfloat-equal
    -Wformat=2
    -Wformat-nonliteral
    -Wformat-security
    -Wformat-y2k
    -Wimport
    -Winit-self
    -Winline
    -Winvalid-pch
    -Wmissing-field-initializers
    -Wmissing-format-attribute
    -Wmissing-include-dirs
    -Wmissing-noreturn
    -Wpacked
    -Wpointer-arith
    -Wredundant-decls
    -Wshadow
    -Wstack-protector
    -Wstrict-aliasing=2
    -Wswitch-default
    -Wswitch-enum
    -Wunreachable-code
    -Wunused
    -Wvariadic-macros
    -Wwrite-strings

    # Disabled warnings
    -Wno-unused-parameter
)
list(JOIN GNU_CXX_WARNINGS " " DEVMODES_GNU_CXX_WARNINGS)
set(${UC_PROJECT_NAME}_DEVMODES_GNU_CXX_WARNINGS ${DEVMODES_GNU_CXX_WARNINGS}
    CACHE STRING "g++ warnings to use in developer mode builds"
)

set(CLANG_CXX_WARNINGS
    -Wall
    -pedantic
    -pedantic-errors
    -Wextra
    -Wcast-align
    -Wcast-qual
    -Wconversion
    -Wdisabled-optimization
    -Werror
    -Wfloat-equal
    -Wformat=2
    -Wformat-nonliteral
    -Wformat-security
    -Wformat-y2k
    -Wimport
    -Winit-self
    -Winline
    -Winvalid-pch
    -Wmissing-field-initializers
    -Wmissing-format-attribute
    -Wmissing-include-dirs
    -Wmissing-noreturn
    -Wpacked
    -Wpointer-arith
    -Wredundant-decls
    -Wshadow
    -Wstack-protector
    -Wstrict-aliasing=2
    -Wswitch-default
    -Wswitch-enum
    -Wunreachable-code
    -Wunused
    -Wvariadic-macros
    -Wwrite-strings

    # Disabled warnings
    -Wno-unused-parameter
)
list(JOIN CLANG_CXX_WARNINGS " " DEVMODES_CLANG_CXX_WARNINGS)
set(${UC_PROJECT_NAME}_DEVMODES_CLANG_CXX_WARNINGS ${DEVMODES_CLANG_CXX_WARNINGS}
    CACHE STRING "clang++ warnings to use in developer mode builds"
)

set(DEV_MODES DevWithASAN DevWithCoverage)
get_property(IS_MULTI_CONFIG GLOBAL
    PROPERTY GENERATOR_IS_MULTI_CONFIG
)
if (IS_MULTI_CONFIG)
    foreach(MODE ${DEV_MODES})
        if(NOT MODE IN_LIST CMAKE_CONFIGURATION_TYPES)
            list(APPEND CMAKE_CONFIGURATION_TYPES "${MODE}")
        endif()
    endforeach()
else()
    set(ALLOWED_BUILD_TYPES Debug Release RelWithDebInfo MinSizeRel)
    list(APPEND ALLOWED_BUILD_TYPES ${DEV_MODES})
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY
        STRINGS "${ALLOWED_BUILD_TYPES}"
    )
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "" FORCE)
    elseif(NOT CMAKE_BUILD_TYPE IN_LIST ALLOWED_BUILD_TYPES)
        message(FATAL_ERROR "Unknown build type: ${CMAKE_BUILD_TYPE}")
    endif()

    if (CMAKE_BUILD_TYPE IN_LIST DEV_MODES)
        set(${UC_PROJECT_NAME}_ENABLE_TESTING ON)
        set(BUILD_SHARED_LIBS ON) # We want to catch symbol visibility errors during development
    endif()
endif()

set(USING_GNU $<CXX_COMPILER_ID:GNU>)
set(USING_CLANG $<CXX_COMPILER_ID:Clang>)
set(USING_APPLE_CLANG $<CXX_COMPILER_ID:AppleClang>)
if (USING_GNU)
    set(CMAKE_CXX_FLAGS_DEV "${CMAKE_CXX_FLAGS_DEBUG} ${${UC_PROJECT_NAME}_DEVMODES_GNU_CXX_WARNINGS} -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS_DEVWITHASAN "${CMAKE_CXX_FLAGS_DEV} -fsanitize=address")
    set(CMAKE_CXX_FLAGS_DEVWITHCOVERAGE "${CMAKE_CXX_FLAGS_DEV} --coverage")
    set(CMAKE_EXE_LINKER_FLAGS_DEVWITHCOVERAGE "${CMAKE_EXE_LINKER_FLAGS_DEBUG} --coverage")
elseif(USING_CLANG)
    set(CMAKE_CXX_FLAGS_DEV "${CMAKE_CXX_FLAGS_DEBUG} ${${UC_PROJECT_NAME}_DEVMODES_CLANG_CXX_WARNINGS} -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS_DEVWITHSANITIZERS "${CMAKE_CXX_FLAGS_DEV} -fsanitize=address")
    set(CMAKE_CXX_FLAGS_DEVWITHCOVERAGE "${CMAKE_CXX_FLAGS_DEV} --coverage")
    set(CMAKE_EXE_LINKER_FLAGS_DEVWITHCOVERAGE "${CMAKE_EXE_LINKER_FLAGS_DEBUG} --coverage")
elseif(USING_APPLE_CLANG)
    set(CMAKE_CXX_FLAGS_DEV "${CMAKE_CXX_FLAGS_DEBUG} ${${UC_PROJECT_NAME}_DEVMODES_CLANG_CXX_WARNINGS} -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS_DEVWITHSANITIZERS "${CMAKE_CXX_FLAGS_DEV} -fsanitize=address")
    set(CMAKE_CXX_FLAGS_DEVWITHCOVERAGE "${CMAKE_CXX_FLAGS_DEV} --coverage")
    set(CMAKE_EXE_LINKER_FLAGS_DEVWITHCOVERAGE "${CMAKE_EXE_LINKER_FLAGS_DEBUG} --coverage")
else()
    set(CMAKE_EXE_LINKER_FLAGS_DEV ${CMAKE_EXE_LINKER_FLAGS})
    set(CMAKE_EXE_LINKER_FLAGS_DEVWITHASAN ${CMAKE_EXE_LINKER_FLAGS})
    set(CMAKE_EXE_LINKER_FLAGS_DEVWITHCOVERAGE ${CMAKE_EXE_LINKER_FLAGS})
endif()
