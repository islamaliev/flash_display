#include <cstring>
#include <ComponentContainer.h>
#include "Contex.h"
#include "RenderBufferOrganizer.h"
#include "StackAllocator.h"
#include "DisplayObject.h"

#include "Program.h"
#include "Texture.h"

using namespace flash;
using namespace render;

using Mat4 = math::Mat4;
using DisplayObject = display::DisplayObject;
using StackAllocator = allocator::StackAllocator;

// TODO: find a way to get rid of this MAX_TREE_DEPTH
#define MAX_TREE_DEPTH 100

namespace {
    // TODO: calculate depending on GL_MAX_TEXTURE_IMAGE_UNITS value
    const unsigned bitsInTextureGroup = 1;
}

void RenderBufferOrganizer::organize(flash::display::DisplayObject& stage, StackAllocator& allocator, BufferData& bufData) {
    ComponentContainer& components = stage._getComponents();
    components.sort();

    // TODO: should be real num of textures. Move it to some kind of texture manager
    const int textureNum = 4; // this includes shapes, e.g. objects with no texture
    unsigned* batchSizes = (unsigned*) allocator.alloc(sizeof(unsigned) * textureNum);
    memset(batchSizes, 0, sizeof(unsigned) * textureNum);

    int lastDepth = 1;
    int numLeafComponents = 0;

    components.forEachTextureData([&numLeafComponents, batchSizes, &lastDepth](TextureData& textureData, int depth) {
        if (depth <= 0)
            return;
        bool toOverride = lastDepth == depth - 1;
        lastDepth = depth;
        numLeafComponents += !toOverride;
        auto batchIndex = textureData.textureId  >> bitsInTextureGroup;
        batchSizes[batchIndex] = batchSizes[batchIndex] + !toOverride;
    });

    bufData.batchSizes = batchSizes;
    // TODO: make value correct value is set
    bufData.numDraws = (textureNum + 1) >> bitsInTextureGroup;

    int* offsets = (int*) allocator.alloc(sizeof(int) * textureNum);

    offsets[0] = 0;
    for (int i = 1; i < textureNum + 1; ++i) {
        offsets[i] = offsets[i - 1] + batchSizes[i - 1];
    }

    Mat4* parentMatrices = (Mat4*) allocator.alloc(sizeof(Mat4) * MAX_TREE_DEPTH);
    *parentMatrices = Mat4();

    bufData.matrices = (Mat4*) allocator.alloc(sizeof(Mat4) * numLeafComponents);
    bufData.textures = (int*) allocator.alloc(sizeof(Context::TextureIndexType) * numLeafComponents);

    lastDepth = 1;
    int lastIndex = 0;

    components.forEach2([parentMatrices, &lastDepth, &lastIndex, &bufData, offsets]
                                (SpatialComponent& spatial, TextureData& textureData, int depth) {
        if (depth <= 0)
            return;
        auto batchIndex = textureData.textureId >> bitsInTextureGroup;
        bool toOverride = lastDepth == depth - 1;
        // TODO: check if conditional move is used here
        auto index = toOverride ? lastIndex : offsets[batchIndex];
        offsets[batchIndex] = offsets[batchIndex] + !toOverride;
        bufData.textures[index] = textureData.textureId ? int(textureData.textureId - (batchIndex << bitsInTextureGroup)) : -1;
        Mat4* m = bufData.matrices + index;
        *m = Mat4();
        m->translate(spatial.x - spatial.pivotX * spatial.scaleX, spatial.y - spatial.pivotY * spatial.scaleY, 0);
        m->scale(spatial.width, spatial.height, 0);
        *m = parentMatrices[depth] = parentMatrices[depth - 1] * *m;
        lastDepth = depth;
        lastIndex = index;
    });
}