//
// Created by zhangyeqi on 2020/11/3.
//

#include "rtti_test.h"

#include <Point.h>
#include <SuperPoint.h>
#include <Rect.h>

#include <iostream>
#include <vector>
#include <string>

using namespace rtti_class;

struct NormalStruct {
    int hello;
};

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

    assert(rtti::Field<int>::FIELD_TYPE == rtti::FIELD_TYPE_PRIMARY);
    assert(rtti::Field<Point>::FIELD_TYPE == rtti::FIELD_TYPE_SHARED);
    assert(rtti::ArrayField<int>::FIELD_TYPE == rtti::FIELD_TYPE_PRIMARY_ARRAY);
    assert(rtti::ArrayField<Point>::FIELD_TYPE == rtti::FIELD_TYPE_SHARED_ARRAY);
    std::cout << ">> checkRTTIField \tPASS !" << std::endl;
}

void checkRTTISerial() {
    auto pointOnePtr = std::make_shared<SuperPoint>();
    pointOnePtr->x = 100;
    pointOnePtr->y = 101;
    pointOnePtr->z = 102;

    auto pointTwoPtr = std::make_shared<Point>();
    pointTwoPtr->x = 200;
    pointTwoPtr->y = 201;

    auto pointThreePtr = std::make_shared<Point>();
    pointThreePtr->x = 300;
    pointThreePtr->y = 301;

    auto pointFourPtr = std::make_shared<Point>();
    pointFourPtr->x = 400;
    pointFourPtr->y = 401;

    auto pointFivePtr = std::make_shared<Point>();
    pointFivePtr->x = 500;
    pointFivePtr->y = 501;

    auto rectPtr = std::make_shared<Rect>();
    rectPtr->p1 = std::static_pointer_cast<Point>(pointOnePtr);
    rectPtr->p2 = pointTwoPtr;
    rectPtr->name = std::string("color-rect");
    rectPtr->color = 123456;
    rectPtr->stingVector() = std::vector<std::string>({"H", "E", "L"});
    rectPtr->stingVector().emplace_back("L");
    rectPtr->stingVector().emplace_back("O");
    rectPtr->pointVector().emplace_back(rtti::SharedObject(pointThreePtr));
    rectPtr->pointVector().emplace_back(rtti::SharedObject(pointFourPtr));
    rectPtr->pointVector().emplace_back(rtti::SharedObject(pointFivePtr));

    // serial
    auto basePtr = std::dynamic_pointer_cast<rtti::ClassBase>(rectPtr);
    std::string data = basePtr->serial();

    std::cout << "Rect : " << data << std::endl;

    // unSerial
    auto newRect = std::make_shared<Rect>();
    newRect->deserial(data);

    // check if they are the same
    assert(*newRect == *basePtr);

    std::cout << ">> checkRTTISerial \tPASS !" << std::endl;
}
