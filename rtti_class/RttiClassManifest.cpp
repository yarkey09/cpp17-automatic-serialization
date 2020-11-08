//
// Created by zhangyeqi on 2020/11/4.
//

#include "Point.h"
#include "SuperPoint.h"
#include "Rect.h"

#include <Serializer.h>
#include <Serializable.h>

RTTI_SERIALIZABLE_IMP(rtti_class::Point, rtti::ClassBase)
RTTI_SERIALIZABLE_IMP(rtti_class::SuperPoint, rtti_class::Point)
RTTI_SERIALIZABLE_IMP(rtti_class::Rect, rtti::ClassBase)