
if (MINGW)
    #Required for static SDL linking
    SET(EXE_LINK_LIBS_POST ${EXE_LINK_LIBS_POST} "winmm;imm32;setupapi;version;ws2_32;iphlpapi")
endif ()

message("SDL version: ${OPTION_SDL}")
if (OPTION_SDL MATCHES "SDL2")
    if (MSVC_CL_BUILD)
        #Specifics to MSVC+VCPKG
        FIND_PACKAGE(SDL2 CONFIG REQUIRED)
        SET(SDL2_INCLUDE_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/SDL2")
        SET(SDL2_LIBRARY SDL2::SDL2)
        SET(SDL2MAIN_LIBRARY SDL2::SDL2main)
        FIND_PACKAGE(SDL2_image CONFIG REQUIRED)
        SET(SDL2_IMAGE_LIBRARY $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>)
        FIND_PACKAGE(SDL2_mixer CONFIG REQUIRED)
        SET(SDL2_MIXER_LIBRARY $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>)
        FIND_PACKAGE(SDL2_net CONFIG REQUIRED)
        SET(SDL2_NET_LIBRARY $<IF:$<TARGET_EXISTS:SDL2_net::SDL2_net>,SDL2_net::SDL2_net,SDL2_net::SDL2_net-static>)
    else ()
        FIND_PACKAGE(SDL2 REQUIRED)
        FIND_PACKAGE(SDL2-image REQUIRED)
        FIND_PACKAGE(SDL2-mixer REQUIRED)
        FIND_PACKAGE(SDL2-net REQUIRED)
    endif ()
    
    include_directories(SYSTEM
            ${SDL2_INCLUDE_DIR}
            ${SDL2_IMAGE_INCLUDE_DIR}
            ${SDL2_MIXER_INCLUDE_DIR}
            ${SDL2_NET_INCLUDE_DIR}
    )

    set(PERIMETER_SDL_LIBRARY ${SDL2_LIBRARY})
    set(PERIMETER_SDL_EXTRA_LIBRARIES ${SDL2_IMAGE_LIBRARY} ${SDL2_MIXER_LIBRARY} ${SDL2_NET_LIBRARY})
    set(PERIMETER_SDL_MAIN_LIBRARY ${SDL2MAIN_LIBRARY})
elseif (OPTION_SDL MATCHES "SDL3")
    message(SEND_ERROR "Still not finished!")
    
    #TODO wait until distros actually ship all of them, or include ourselves?

    set(PERIMETER_SDL_LIBRARY ${SDL3_LIBRARY})
    set(PERIMETER_SDL_EXTRA_LIBRARIES ${SDL3_IMAGE_LIBRARY} ${SDL3_MIXER_LIBRARY} ${SDL3_NET_LIBRARY})
    set(PERIMETER_SDL_MAIN_LIBRARY ${SDL3MAIN_LIBRARY})
else ()
    message(SEND_ERROR "Unknown version!")
endif ()
