//
// Created by zhangyeqi on 2020/11/4.
//

#ifndef CPPRTTI_RECT_H
#define CPPRTTI_RECT_H

#include "Point.h"
#include <Rtti.h>
#include <vector>
#include <string>

namespace rtti_class {

    RTTI_CLASS_START(Rect)
        RTTI_FIELD(0, p1, Point, "E")
        RTTI_FIELD(1, p2, Point, "E")
        RTTI_FIELD(2, name, std::string, "E")
        RTTI_FIELD(3, color, uint32_t, "E")
        RTTI_ARRAY_FIELD(4, stingVector, std::string, "E")
        RTTI_ARRAY_FIELD(5, pointVector, Point, "E")
    RTTI_CLASS_END()

}

#endif //CPPRTTI_RECT_H
