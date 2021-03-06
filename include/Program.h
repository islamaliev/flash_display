#pragma once

namespace flash {

namespace math {
    class Mat4;
}

namespace render {

    class Context;

    class Program {
    public:
        Program();

        void init();

        void activate(Context* context);

        void dispose();

        void setUniform(const char* name, const flash::math::Mat4& matrix);

        void setUniform(const char* name, int val);

        void setUniform(const char* name, int* val, int count);
    };

}

}