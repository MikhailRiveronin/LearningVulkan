include(FetchContent)

FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG e7ea71be039836da3a98cea55ae5569cb5eb885c
    FIND_PACKAGE_ARGS)

FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 2d4c4b4dd31fde06cfffad7915c2b3006402322f
    FIND_PACKAGE_ARGS)

FetchContent_MakeAvailable(glfw glm)
