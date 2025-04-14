#pragma once

#include "Application.h"
#include "Logger.h"
#include "SampleBase.h"

#include <memory>
#include <stdexcept>

static std::unique_ptr<SampleBase> createSample();

int main()
{
    auto sample = createSample();
    Application::run(sample.get());

    return 0;
}
