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

namespace {
    class ForEachTextureDataIterator {
    public:
        ForEachTextureDataIterator(int& numLeafComponents, unsigned* batchSizes, unsigned& numDraws)
            : m_numLeafComponents(numLeafComponents)
            , m_batchSizes(batchSizes)
            , m_numDraws(numDraws){}
        
        void operator()(TextureData& textureData, int depth) {
            if (depth <= 0)
                return;
            bool toOverride = m_lastDepth == depth - 1;
            m_lastDepth = depth;
            m_numLeafComponents += !toOverride;
            auto batchIndex = textureData.textureId >> Context::s_batchBitsNum;
            batchIndex = textureData.textureId > 0 ? batchIndex : 0;
            m_batchSizes[batchIndex] += int(bool(textureData.textureId));
            m_numDraws = m_numDraws < batchIndex ? (unsigned) batchIndex : m_numDraws;
        }
        
    private:
        int& m_numLeafComponents;
        unsigned* m_batchSizes;
        unsigned& m_numDraws;
        int m_lastDepth{1};
    };
    
    class ForEachIterator {
    public:
        ForEachIterator(Mat4* parentMatrices, BufferData& bufData, int* offsets)
            : m_parentMatrices(parentMatrices)
            , m_bufData(bufData)
            , m_offsets(offsets) {}
        
        void operator()(SpatialComponent& spatial, TextureData& textureData, int depth, int order) {
            if (depth <= 0)
                return;
            // TODO: check if conditional move is used everywhere
            auto batchIndex = textureData.textureId >> Context::s_batchBitsNum;
            // shapes with texture id == -1 go to first batch
            batchIndex = textureData.textureId > 0 ? batchIndex : 0;
            bool toOverride = m_lastDepth == depth - 1;
    
            // containers are ignorant to batches because they always have textureId == 0, so when toOverride
            // is set we need to make sure current object also falls into first batch, otherwise we ignore override
            auto index = toOverride && !batchIndex ? m_lastIndex : m_offsets[batchIndex];
            // we advance batch offset only for leaf nodes (for container increment is 0)
            m_offsets[batchIndex] += int(bool(textureData.textureId));
            
            setTextureId(textureData.textureId, batchIndex, index);
            setMatrix(textureData.textureId, index, depth, order, spatial);
            
            m_lastDepth = depth;
            m_lastIndex = index;
        }
                
    private:
        inline void setTextureId(int textureId, int batchIndex, int index) {
            int texUnitIndex = textureId - (batchIndex << Context::s_batchBitsNum);
            texUnitIndex = textureId > 0 ? texUnitIndex : textureId;
            // dummy int to channel texUnitIndex for containers
            int dummyTexUnit;
            int* textUnitP = textureId ? m_bufData.textures + index : &dummyTexUnit;
            *textUnitP = texUnitIndex;
        }
        
        inline void setMatrix(int textureId, int index, int depth, int order, const SpatialComponent& spatial) {
            Mat4* currentMatP = m_parentMatrices + depth;
            // for containers m == currentMatP so that buffer matrices are untouched
            Mat4* m = textureId ? m_bufData.matrices + index : currentMatP;
            *m = *currentMatP = m_parentMatrices[depth - 1] * DisplayObject::_getTransform(spatial, order);
            // z order of an object is given in global space so we prevent concatenation with parent's z
            m_parentMatrices[depth].zt(0);
        }
        
        Mat4* m_parentMatrices;
        BufferData& m_bufData;
        int* m_offsets;
        int m_lastDepth{1};
        int m_lastIndex{0};
    };
}

// TODO: find a way to get rid of this MAX_TREE_DEPTH
#define MAX_TREE_DEPTH 100

void RenderBufferOrganizer::organize(flash::display::DisplayObject& stage, StackAllocator& allocator, BufferData& bufData) {
    ComponentContainer& components = stage._getComponents();
    components.sort();
    
    const unsigned numTextures = display::Texture::s_numTextures + 1; // this includes shapes, e.g. objects with no texture
    // TODO: size of allocated memory should be numTextureGroups, not numTextures, nor numTextures >> Context::s_batchBitsNum
    // we don't control which texture id is generated by OpenGL. In the worst case all textures can fall in different groups
    unsigned* batchSizes = (unsigned*) allocator.alloc(sizeof(unsigned) * numTextures);
    memset(batchSizes, 0, sizeof(unsigned) * numTextures);

    int numLeafComponents = 0;
    unsigned numDraws = 0;

    components.forEachTextureData(ForEachTextureDataIterator(numLeafComponents, batchSizes, numDraws));
    bufData.batchSizes = batchSizes;
    bufData.numDraws = numDraws + 1;

    // offset for each batch
    int* offsets = (int*) allocator.alloc(sizeof(int) * numTextures);
    offsets[0] = 0;
    for (int i = 1; i < bufData.numDraws; ++i) {
        offsets[i] = offsets[i - 1] + batchSizes[i - 1];
    }

    Mat4* parentMatrices = (Mat4*) allocator.alloc(sizeof(Mat4) * MAX_TREE_DEPTH);
    *parentMatrices = Mat4();

    bufData.matrices = (Mat4*) allocator.alloc(sizeof(Mat4) * numLeafComponents);
    bufData.textures = (int*) allocator.alloc(sizeof(Context::TextureIndexType) * numLeafComponents);

    components.forEach(ForEachIterator(parentMatrices, bufData, offsets));
}