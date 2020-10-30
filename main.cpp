#include <iostream>
#include <vector>

#include "rtti/Rtti.h"
#include "rtti/Serializer.h"
// --------------------------------

RTTI_STRUCT_START(Point)
    RTTI_FIELD(0, x, double, "E")
    RTTI_FIELD(1, y, double, "E")
RTTI_STRUCT_END()

RTTI_STRUCT_START(Rect)
    RTTI_FIELD(0, p1, std::string,"E")
    RTTI_FIELD(1, p2, float, "E")
    RTTI_FIELD(2, color, uint32_t, "E")
RTTI_STRUCT_END()

void printRect(const char * prefix, const Rect& rect) {
    std::cout << prefix << "Rect : p1=(" << rect.p1 << "), p2=(" << rect.p2 << "), color=(" << rect.color << ")" << std::endl;
}

int main(int argc, char** argv) {

    // 2. 通过初始化结构体的方式创建实例
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
}
