#include "rtti/Rtti.h"
#include "rtti/Serializer.h"

#include <iostream>
#include <vector>
#include <string>

struct NormalStruct {
    int hello;
};

RTTI_STRUCT_START(Point)
    RTTI_FIELD(0, x, int, "E")
    RTTI_FIELD(1, y, int, "E")
RTTI_STRUCT_END()

RTTI_STRUCT_START(Rect)
    RTTI_FIELD(0, p1, Point,"E")
    RTTI_FIELD(1, p2, Point, "E")
    RTTI_FIELD(2, name, std::string, "E")
    RTTI_FIELD(3, color, uint32_t, "E")
    RTTI_FIELD(4, stingVector, std::vector<std::string>,"E")
RTTI_STRUCT_END()

// TODO: test vector of rtti struct

// 判断指定类型是否为 RTTI 类型
void checkRTTIStruct() {
    assert(! rtti::IsRTTIStruct<int>::Value);                       // primitive types
    assert(! rtti::IsRTTIStruct<std::vector<std::string>>::Value);  // stl class
    assert(! rtti::IsRTTIStruct<NormalStruct>::Value);              // struct
    assert(rtti::IsRTTIStruct<Point>::Value);                       // rtti struct with primitive types
    assert(rtti::IsRTTIStruct<Rect>::Value);                        // rtti struct with primitive types and rtti struct and stl
    std::cout << ">> checkRTTIStruct \tPASS !" << std::endl;
}

void checkRTTIField() {
    assert(! rtti::IsRTTIField<NormalStruct>::Value);
    assert(! rtti::IsRTTIStructField<NormalStruct>::Value);
    assert(rtti::IsRTTIField<rtti::Field<int>>::Value);
    assert(! rtti::IsRTTIStructField<rtti::Field<int>>::Value);
    assert(rtti::IsRTTIField<rtti::Field<Point>>::Value);
    assert(rtti::IsRTTIStructField<rtti::Field<Point>>::Value);
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
    auto pointOnePtr = std::make_shared<Point>();
    pointOnePtr->x = 100;
    pointOnePtr->y = 101;

    auto pointTwoPtr = std::make_shared<Point>();
    pointTwoPtr->x = 200;
    pointTwoPtr->y = 201;

    // 1, Rect
    Rect rect {
            .p1 = pointOnePtr,
            .p2 = pointTwoPtr,
            .name = {"color-rect"},
            .color = 123456,
            .stingVector = {{"H", "E", "L", "L", "O"}}
    };

    rtti::SerialStream rectObjectsStream;
    auto key = rtti::Serializer::serial(rectObjectsStream, rect);
    std::cout << "Rect : " << rectObjectsStream.dump(4) << std::endl;
    auto newRect = rtti::Serializer::unSerial<Rect>(rectObjectsStream, key);
    assert(rect == *newRect);
    std::cout << ">> checkRTTISerial \tPASS !" << std::endl;
}

int main(int argc, char** argv) {
    checkRTTIStruct();
    checkRTTIField();
    checkRTTISerial();
}
