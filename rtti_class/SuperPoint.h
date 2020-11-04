//
// Created by zhangyeqi on 2020/11/4.
//

#ifndef CPPRTTI_SUPERPOINT_H
#define CPPRTTI_SUPERPOINT_H

#include "Point.h"
#include <Rtti.h>
#include <Equals.h>

namespace rtti_class {

    RTTI_SUPER_CLASS_START(SuperPoint, Point)
        RTTI_FIELD(0, z, int, "E")
    RTTI_SUPER_CLASS_END()

}

#endif //CPPRTTI_SUPERPOINT_H
