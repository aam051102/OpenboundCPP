cmake_minimum_required(VERSION 3.25)  # Specifies the required CMake version.
project(Openbound)  # Defines the project name.

include(${CMAKE_CURRENT_SOURCE_DIR}/conanbuildinfo.cmake)  # Includes the contents of the conanbuildinfo.cmake file.
conan_basic_setup()  # Prepares the CMakeList.txt for Conan.

# $source_files is a space-delimited list of filenames.
add_executable(Openbound 
    "openbound/src/Action.cpp"
    "openbound/src/ActionQueue.cpp"
    "openbound/src/Animation.cpp"
    "openbound/src/AssetAudio.cpp"
    "openbound/src/AssetFont.cpp"
    "openbound/src/AssetGraphic.cpp"
    "openbound/src/ASsetManager.cpp"
    "openbound/src/AssetPath.cpp"
    "openbound/src/BatchHandler.cpp"
    "openbound/src/Character.cpp"
    "openbound/src/Chooser.cpp"
    "openbound/src/CommandHandler.cpp"
    "openbound/src/Common.cpp"
    "openbound/src/Dialoger.cpp"
    "openbound/src/Event.cpp"
    "openbound/src/EventFactory.cpp"
    "openbound/src/EventGameState.cpp"
    "openbound/src/EventInBox.cpp"
    "openbound/src/EventMovie.cpp"
    "openbound/src/EventNoActions.cpp"
    "openbound/src/EventNudge.cpp"
    "openbound/src/EventPlayed.cpp"
    "openbound/src/EventSpriteProperty.cpp"
    "openbound/src/EventTime.cpp"
    "openbound/src/EventWithinRange.cpp"
    "openbound/src/Fighter.cpp"
    "openbound/src/FontEngine.cpp"
    "openbound/src/Input.cpp"
    "openbound/src/Logger.cpp"
    "openbound/src/main.cpp"
    "openbound/src/Music.cpp"
    "openbound/src/Parser.cpp"
    "openbound/src/Room.cpp"
    "openbound/src/Sburb.cpp"
    "openbound/src/Serializer.cpp"
    "openbound/src/Sound.cpp"
    "openbound/src/Sprite.cpp"
    "openbound/src/SpriteButton.cpp"
    "openbound/src/Trigger.cpp"
    "openbound/src/Window.cpp"
    "openbound/openbound.rc"
)  # Specifies the executable to build.
    
set_property(TARGET Openbound PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
add_compile_definitions(_UNICODE UNICODE)
target_compile_features(Openbound PUBLIC cxx_std_17)
target_include_directories(Openbound PUBLIC "openbound/includes/" "lib/")
    
add_custom_command(TARGET Openbound POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CONAN_OPENAL_ROOT}/bin/OpenAL32.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    
target_link_libraries(Openbound ${CONAN_LIBS})  # Specifies what libraries to link, using Conan.