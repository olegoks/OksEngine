
#Usage:
# get_source_files(
#     PROJECT_SOURCES
#         "${CMAKE_CURRENT_LIST_DIR}/Sources"
#         "${CMAKE_CURRENT_LIST_DIR}/Packages" 
#     )
function(get_source_files FILE_LIST)
    set(TEMP_FILES "")
    foreach(DIR ${ARGN})
        message(STATUS "Scanning directory: ${DIR}")
        if(NOT EXISTS "${DIR}")
            message(WARNING "Directory does not exist: ${DIR}")
            continue()
        endif()
        file(GLOB_RECURSE DIR_FILES "${DIR}/*.cpp" "${DIR}/*.hpp")
        message(STATUS "All files in ${DIR}:")
        foreach(ITEM ${DIR_FILES})
            message(STATUS "  ${ITEM}")
        endforeach()
        list(APPEND TEMP_FILES ${DIR_FILES})
    endforeach()
    set(${FILE_LIST} ${TEMP_FILES} PARENT_SCOPE)
endfunction()



# Функция для создания source_group для в соответствии с их местоположением относительно корневой директории
function(organize_source_files DIRECTORY)
    # Все аргументы после DIRECTORY - это файлы
    set(FILE_LIST ${ARGN})
    
    foreach(FILE_PATH IN LISTS FILE_LIST)
        # Убираем корневую часть пути к файлу
        file(RELATIVE_PATH REL_PATH "${DIRECTORY}" ${FILE_PATH})
        # Извлекаем родительский путь
        get_filename_component(PARENT_DIR "${REL_PATH}" DIRECTORY)
        
        # Заменяем '/' на '\\' в пути, для правильной работы source_group
        string(REPLACE "/" "\\" GROUP_NAME "Sources/${PARENT_DIR}")
        message(STATUS "Source group: ${GROUP_NAME} for: ${FILE_PATH}")
        # Создаем source_group
        source_group("${GROUP_NAME}" FILES "${FILE_PATH}")
    endforeach()
endfunction()


#Usage:
#GenerateTemplates(
#    "${CMAKE_CURRENT_SOURCE_DIR}/CMakeTemplates/Help.h.in"
#    "${CMAKE_CURRENT_SOURCE_DIR}/Sources/CMakeTemplates_generated/Help.h"
#    "CONTENT" "${CMAKE_CURRENT_SOURCE_DIR}/CMakeTemplates/Help.cmake" 
#)
function(GenerateTemplates TEMPLATE_PATH DEST_PATH)
    # Пропускаем первые два аргумента (TEMPLATE_PATH и DEST_PATH)
    set(INDEX 2)
    while(INDEX LESS ${ARGC})
        math(EXPR NAME_INDEX "${INDEX}")
        math(EXPR PATH_INDEX "${INDEX} + 1")
        
        list(GET ARGV ${NAME_INDEX} VAR_NAME)
        list(GET ARGV ${PATH_INDEX} FILE_PATH)
        
        message(STATUS "Processing: ${VAR_NAME} = ${FILE_PATH}")
        
        if(EXISTS "${FILE_PATH}")
            file(READ "${FILE_PATH}" CONTENT)
            #string(REPLACE "\\" "\\\\" CONTENT "${CONTENT}")
            # кавычка -> экранированная кавычка
            #string(REPLACE "\"" "\\\"" CONTENT "${CONTENT}")
            set(${VAR_NAME} "${CONTENT}")
        else()
            message(WARNING "File not found: ${FILE_PATH} — using empty content")
            set(${VAR_NAME} "")
        endif()
        
        math(EXPR INDEX "${INDEX} + 2")
    endwhile()
    
    # Создаём выходную директорию, если её нет
    get_filename_component(DEST_DIR "${DEST_PATH}" DIRECTORY)
    file(MAKE_DIRECTORY "${DEST_DIR}")
    
    configure_file(
        "${TEMPLATE_PATH}"
        "${DEST_PATH}"
        @ONLY
    )
endfunction()


