#pragma once

#include "Camera.h"

namespace gl
{
    class CameraControlls {
    public:
        CameraControlls() {}

        virtual void onUpdate() = 0;
    };
}
