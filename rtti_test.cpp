//
// Created by zhangyeqi on 2020/11/3.
//

#include "rtti_test.h"
#include "rtti/Rtti.h"
#include "rtti/Serializer.h"

#include <iostream>
#include <vector>
#include <string>

struct NormalStruct {
    int hello;
};

RTTI_CLASS_START(Point)
    RTTI_FIELD(0, x, int, "E")
    RTTI_FIELD(1, y, int, "E")
RTTI_CLASS_END()

RTTI_SUPER_CLASS_START(SuperPoint, Point)
    RTTI_FIELD(0, z, int, "E")
RTTI_SUPER_CLASS_END()

RTTI_CLASS_START(Rect)
    RTTI_FIELD(0, p1, Point,"E")
    RTTI_FIELD(1, p2, Point, "E")
    RTTI_FIELD(2, name, std::string, "E")
    RTTI_FIELD(3, color, uint32_t, "E")
    RTTI_FIELD(4, stingVector, std::vector<std::string>,"E")
RTTI_CLASS_END()

// TODO: test vector of rtti struct

// 判断指定类型是否为 RTTI 类型
void checkRTTIStruct() {
    assert(! rtti::IsClass<int>::Value);                       // primitive types
    assert(! rtti::IsClass<std::vector<std::string>>::Value);  // stl class
    assert(! rtti::IsClass<NormalStruct>::Value);              // struct
    assert(rtti::IsClass<Point>::Value);                       // rtti struct with primitive types
    assert(rtti::IsClass<Rect>::Value);                        // rtti struct with primitive types and rtti struct and stl
    std::cout << ">> checkRTTIStruct \tPASS !" << std::endl;
}

void checkRTTIField() {
    assert(! rtti::IsField<NormalStruct>::Value);
    assert(! rtti::IsSharedField<NormalStruct>::Value);
    assert(rtti::IsField<rtti::Field<int>>::Value);
    assert(! rtti::IsSharedField<rtti::Field<int>>::Value);
    assert(rtti::IsField<rtti::Field<Point>>::Value);
    assert(rtti::IsSharedField<rtti::Field<Point>>::Value);
    std::cout << ">> checkRTTIField \tPASS !" << std::endl;
}

bool operator==(const Point& lhs, const Point& rhs) {
    return lhs.x.has() == rhs.x.has()
           && lhs.x.get() == rhs.x.get()
           && lhs.y.has() == rhs.y.has()
           && lhs.y.get() == rhs.y.get();
}

// TODO: override operator == by themselves
bool operator==(const Rect& lhs, const Rect& rhs) {
    return lhs.p1.has() == rhs.p1.has()
           && *lhs.p1.get() == *rhs.p1.get()
           && lhs.p2.has() == rhs.p2.has()
           && *lhs.p2.get() == *rhs.p2.get()
           && lhs.name.has() == rhs.name.has()
           && lhs.name.get() == rhs.name.get()
           && lhs.color.has() == rhs.color.has()
           && lhs.color.get() == rhs.color.get()
           && lhs.stingVector.has() == rhs.stingVector.has()
           && lhs.stingVector.get() == rhs.stingVector.get();
}

void checkRTTISerial() {
    auto pointOnePtr = std::make_shared<SuperPoint>(); // TODO: SuperPoint 无法支持！！！
    pointOnePtr->x = 100;
    pointOnePtr->y = 101;
    pointOnePtr->z = 102;

    auto pointTwoPtr = std::make_shared<Point>();
    pointTwoPtr->x = 200;
    pointTwoPtr->y = 201;

    auto rectPtr = std::make_shared<Rect>();
    rectPtr->p1 = pointOnePtr;
    rectPtr->p2 = pointTwoPtr;
    rectPtr->name = "color-rect";
    rectPtr->color = 123456;
    rectPtr->stingVector = std::vector<std::string>({"H", "E", "L", "L", "O"});

    rtti::SerialStream rectObjectsStream;
    auto key = rtti::Serializer::serial(rectObjectsStream, *rectPtr);
    std::cout << "Rect : " << rectObjectsStream.dump(4) << std::endl;
    auto newRect = rtti::Serializer::unSerial<Rect>(rectObjectsStream, key);
    assert(*rectPtr == *newRect);
    std::cout << ">> checkRTTISerial \tPASS !" << std::endl;
}
