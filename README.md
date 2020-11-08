# Automatic serialization library For C++17

<p>
This library use c++ 
<a href="http://www.cplusplus.com/doc/oldtutorial/templates/">template</a> 
and 
<a href="http://www.cplusplus.com/doc/tutorial/preprocessor/">macro</a>
to make serialization automatically.
<br/>
This is not a json library. It is based on <a href="https://github.com/nlohmann/json">nlohmann/json</a>.
<br/>
This is a header-only library.
</p>

## Getting Started

- <b>Include the headers </b><br/>
```
#include <Rtti.h>
```

- <b>Declare your class by the provided macro</b><br/>
```
RTTI_STRUCT_START(Point)
    RTTI_FIELD(0, x, int, "E")
    RTTI_FIELD(1, y, int, "E")
RTTI_STRUCT_END()
```

- <b>Enjoy it</b><br>
```
// create object
Point pointA;
pointA.x = 100;
pointA.y = 200;

// serial
auto serialData = pointA.serial();

// unSerial
Point pointB;
pointB.unSerial(serialData);

// check equals
assert(pointA == pointB);
```

## Authors
* **ZhangYeqi**. <a href="mailto:hello.zhangyeqi@gmail.com">send me an email</a>.

## License
The class is licensed under the <a href="https://opensource.org/licenses/MIT">MIT License</a>.