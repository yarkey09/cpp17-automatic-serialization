//
// Created by zhangyeqi on 2020/11/4.
//

#ifndef CPPRTTI_POINT_H
#define CPPRTTI_POINT_H

#include <Rtti.h>
#include <vector>

namespace rtti_class {

    RTTI_CLASS_START(Point)
        RTTI_FIELD(0, x, int, "E")
        RTTI_FIELD(1, y, int, "E")
    RTTI_CLASS_END()

}

#endif //CPPRTTI_POINT_H
