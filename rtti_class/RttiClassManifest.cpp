//
// Created by zhangyeqi on 2020/11/4.
//

#include "Point.h"
#include "SuperPoint.h"
#include "Rect.h"

#include <Serializer.h>
#include <Serializable.h>

#include <nlohmann/json.hpp>

RTTI_SERIALIZABLE_IMP(rtti_class::Point)
RTTI_SERIALIZABLE_IMP(rtti_class::SuperPoint)
RTTI_SERIALIZABLE_IMP(rtti_class::Rect)