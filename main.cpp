#include <iostream>
#include <vector>

#include "rtti/Rtti.h"
#include "rtti/Creator.h"
// --------------------------------

//template<typename T>
//void dumpObj(const T& obj, const char * fieldName = "") {
//    if constexpr (rtti::IsRttiField<std::decay_t<T>>::Value) {
//        auto& fieldRef = obj.get();
//        if constexpr(std::is_class_v<std::decay_t<decltype(fieldRef)>>) {
//            dumpObj(fieldRef, fieldName);
//        } else {
//            std::cout << fieldName << ":" << fieldRef << "," << std::endl;
//        }
//    } else {
//        std::cout << fieldName << (*fieldName ? ": {" : "{") << std::endl;
//        rtti::forEach(obj, [](auto&& fieldName, auto&& ref) {
//            dumpObj(ref, fieldName);
//        });
//        std::cout << "}" << std::endl;
//    }
//}
//
//template<typename T>
//void dumpObj(T& obj, const char * fieldName = "") {
//    if constexpr (rtti::IsRttiField<std::decay_t<T>>::Value) {
//        auto& fieldRef = obj.mut_get();
//        if constexpr(std::is_class_v<std::decay_t<decltype(fieldRef)>>) {
//            dumpObj(fieldRef, fieldName);
//        } else {
//            std::cout << fieldName << ":" << fieldRef << "," << std::endl;
//        }
//    } else {
//        std::cout << fieldName << (*fieldName ? ": {" : "{") << std::endl;
//        rtti::forEach(obj, [](auto&& fieldName, auto&& ref) {
//            dumpObj(ref, fieldName);
//        });
//        std::cout << "}" << std::endl;
//    }
//}

RTTI_STRUCT_START(Point)
    RTTI_FIELD(0, x, double, "E")
    RTTI_FIELD(1, y, double, "E")
RTTI_STRUCT_END()

RTTI_STRUCT_START(Rect)
    RTTI_FIELD(0, p1, Point,"E")
    RTTI_FIELD(1, p2, Point, "E")
    RTTI_FIELD(2, color, uint32_t, "E")
RTTI_STRUCT_END()

int main(int argc, char** argv) {

    Point::registerCreator();
    Rect::registerCreator();

    auto rectc = new Rect();

    auto rect = (Rect *) rtti::Creator::one().create("Rect");
    rect->p1 = Point {
            .x = 0.1f, .y = 0.2f
    };

    Rect rect2 {
        .p2 = Point {
            .y = 2.0f
        },
        .color = 123456
    };

    rect2.p1.set(Point {.x = 1.0f});

    std::cout << "-------------- field count = (" << rtti::FieldCounter<Point>::Value << ")" << std::endl;
    std::cout << "-------------- rect.p1.has() = (" << rect->p1.has() << ")" << std::endl;
    std::cout << "-------------- rect.p2.has() = (" << rect->p2.has() << ")" << std::endl;
    std::cout << "-------------- rect.p1.get().x.has() = (" << rect->p1.get().x.has() << ")" << std::endl;
    std::cout << "-------------- rect.p1.get().y.has() = (" << rect->p1.get().y.has() << ")" << std::endl;
    std::cout << "-------------- rect.p2.get().x.has() = (" << rect->p2.get().x.has() << ")" << std::endl;
    std::cout << "-------------- rect.p2.get().y.has() = (" << rect->p2.get().y.has() << ")" << std::endl;
//    dumpObj(rect);

    Point point;
    point.x.set(1);
    std::cout << "------ A -------- point.x.has() = (" << point.x.has() << ")" << std::endl;
    std::cout << "------ A -------- point.y.has() = (" << point.y.has() << ")" << std::endl;
//    dumpObj(point);
    point.y.set(2);
    std::cout << "------ B -------- point.x.has() = (" << point.x.has() << ")" << std::endl;
    std::cout << "------ B -------- point.y.has() = (" << point.y.has() << ")" << std::endl;
//    dumpObj(point);

    std::cout << "--------------" << std::endl;
}
