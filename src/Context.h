#pragma once

namespace flash {

namespace display {
    class DisplayObject;
}

namespace math {
    class Mat4;
}

namespace render {
    struct BufferData {
        math::Mat4* matrices;
        int* textures;
		int* batchSizes;
		int numDraws;
	};

    class Context {
    public:
        static int s_maxTextureUnits;
        static int s_batchBitsNum;
    
        using TextureIndexType = int;
        
        void init(int width, int height);

        void start(display::DisplayObject& displayObject);

        void stop();

        void dispose();

        void setProjection(const math::Mat4& matrix);
    };
}

}