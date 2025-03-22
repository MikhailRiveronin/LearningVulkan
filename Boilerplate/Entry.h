#pragma once

#include "Logger.h"
#include "SampleBase.h"

#include <memory>
#include <stdexcept>

static std::unique_ptr<SampleBase> createSample();

int main()
{
    auto sample = createSample();
    try {
        sample->run();
    }
    catch (const std::exception& e) {
        LOG_ERROR("%s", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
