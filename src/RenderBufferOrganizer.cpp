#include <cstring>
#include <ComponentContainer.h>
#include <Texture.h>
#include "Contex.h"
#include "RenderBufferOrganizer.h"
#include "StackAllocator.h"
#include "DisplayObject.h"
#include "Program.h"

using namespace flash;
using namespace render;

using Mat4 = math::Mat4;
using DisplayObject = display::DisplayObject;
using StackAllocator = allocator::StackAllocator;

// TODO: find a way to get rid of this MAX_TREE_DEPTH
#define MAX_TREE_DEPTH 100

void RenderBufferOrganizer::organize(flash::display::DisplayObject& stage, StackAllocator& allocator, BufferData& bufData) {
    ComponentContainer& components = stage._getComponents();
    components.sort();
    
    const unsigned textureNum = display::Texture::s_numTextures + 1; // this includes shapes, e.g. objects with no texture
    unsigned* batchSizes = (unsigned*) allocator.alloc(sizeof(unsigned) * textureNum >> Context::s_batchBitsNum);
    memset(batchSizes, 0, sizeof(unsigned) * textureNum);

    int lastDepth = 1;
    int numLeafComponents = 0;

    components.forEachTextureData([&numLeafComponents, batchSizes, &lastDepth](TextureData& textureData, int depth) {
        if (depth <= 0)
            return;
        bool toOverride = lastDepth == depth - 1;
        lastDepth = depth;
        numLeafComponents += !toOverride;
        auto batchIndex = textureData.textureId  >> Context::s_batchBitsNum;
        batchSizes[batchIndex] = batchSizes[batchIndex] + !toOverride;
    });

    bufData.batchSizes = batchSizes;
    bufData.numDraws = ((textureNum - 1) >> Context::s_batchBitsNum) + 1;

    // offset for each batch
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

    components.forEach([parentMatrices, &lastDepth, &lastIndex, &bufData, offsets]
                      (SpatialComponent& spatial, TextureData& textureData, int depth) {
        if (depth <= 0)
            return;
        auto batchIndex = textureData.textureId >> Context::s_batchBitsNum;
        bool toOverride = lastDepth == depth - 1;
        // TODO: check if conditional move is used here
        auto index = toOverride ? lastIndex : offsets[batchIndex];
        // current batch offset advances once an object is added
        offsets[batchIndex] = offsets[batchIndex] + !toOverride;
        // for shapes the index is -1 so that it can be checked in ths frag shader
        // all other indices must be from 0 to GL_MAX_TEXTURE_IMAGE_UNITS
        bufData.textures[index] = textureData.textureId ? int(textureData.textureId - (batchIndex << Context::s_batchBitsNum)) : -1;
        Mat4* m = bufData.matrices + index;
        *m = Mat4();
        m->translate(spatial.x - spatial.pivotX * spatial.scaleX, spatial.y - spatial.pivotY * spatial.scaleY, 0);
        m->scale(spatial.width, spatial.height, 0);
        *m = parentMatrices[depth] = parentMatrices[depth - 1] * *m;
        lastDepth = depth;
        lastIndex = index;
    });
}