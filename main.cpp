#include <iostream>
#include <vector>

#include "rtti/Rtti.h"
#include "rtti/Serializer.h"
// --------------------------------

struct NormalPoint {
};

RTTI_STRUCT_START(Point)
    RTTI_FIELD(0, x, double, "E")
    RTTI_FIELD(1, y, double, "E")
RTTI_STRUCT_END()

RTTI_STRUCT_START(Rect)
    RTTI_FIELD(0, p1, std::string,"E")
    RTTI_FIELD(1, p2, float, "E")
    RTTI_FIELD(2, color, uint32_t, "E")
RTTI_STRUCT_END()

RTTI_STRUCT_START(Circle)
    RTTI_FIELD(0, p1, Point,"E")
    RTTI_FIELD(1, radius, float, "E")
RTTI_STRUCT_END()

void printRect(const char * prefix, const Rect& rect) {
    std::cout << prefix << "Rect : p1=(" << rect.p1 << "), p2=(" << rect.p2 << "), color=(" << rect.color << ")" << std::endl;
}

int main(int argc, char** argv) {

    std::cout << "NormalPoint is RTTI Struct : " << rtti::IsRTTIStruct<NormalPoint>::Value << std::endl;
    std::cout << "Point is RTTI Struct : " << rtti::IsRTTIStruct<Point>::Value << std::endl;
    std::cout << "Rect is RTTI Struct : " << rtti::IsRTTIStruct<Rect>::Value << std::endl;
    std::cout << "Circle is RTTI Struct : " << rtti::IsRTTIStruct<Circle>::Value << std::endl;

    // 1, Rect
    Rect rect {
        .p1 = {"ok"},
        .p2 = 1.2f,
        .color = 123456
    };
    printRect("Firstly : ", rect);

    auto stream = rtti::Serializer::serial(rect);
    if (stream) {
        std::cout << "Stream : " << stream->dump() << std::endl;
    }

    auto newRect = rtti::Serializer::unSerial<Rect>(*stream);
    if (newRect) {
        printRect("Secondly : ", *newRect);
    }

    // 2. Circle
    Circle circle {
        {{1.0f,2.0f}},
        100.0f
    };
    try {
        auto streamCircle = rtti::Serializer::serial(circle);
        if (streamCircle) {
            std::cout << "StreamCircle : " << streamCircle->dump() << std::endl;
        }
    } catch (...) {
        std::cout << "serial Circle exception" << std::endl;
    }


}
