#pragma once

#include "RenderLayerBase.h"

class RenderLayerSkybox : public RenderLayerBase {
public:
    RenderLayerSkybox(
        Context const& context,
        char const* front,
        char const* back,
        char const* left,
        char const* right,
        char const* up,
        char const* down);

private:
    Image cubeTexture;

    void createRenderPass(Context const& context) override;
};
