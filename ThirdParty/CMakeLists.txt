include(FetchContent)

FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 2d4c4b4dd31fde06cfffad7915c2b3006402322f
    FIND_PACKAGE_ARGS)

FetchContent_Declare(stb
    GIT_REPOSITORY https://github.com/nothings/stb
    GIT_TAG f0569113c93ad095470c54bf34a17b36646bbbb5
    FIND_PACKAGE_ARGS)

FetchContent_Declare(tinygltf
    GIT_REPOSITORY https://github.com/syoyo/tinygltf
    GIT_TAG a5e653e46cca64bbc0ecee7ede007347a94c39b0
    FIND_PACKAGE_ARGS)

FetchContent_MakeAvailable(glm stb tinygltf)
