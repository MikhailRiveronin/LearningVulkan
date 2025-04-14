include(FetchContent)

FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 2d4c4b4dd31fde06cfffad7915c2b3006402322f
    FIND_PACKAGE_ARGS)

FetchContent_Declare(stb
    GIT_REPOSITORY https://github.com/nothings/stb
    GIT_TAG f0569113c93ad095470c54bf34a17b36646bbbb5
    FIND_PACKAGE_ARGS)

FetchContent_MakeAvailable(glm stb)
