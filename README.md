# Automatic serialization library For C++17

<p>
This library use c++ 
<a href="http://www.cplusplus.com/doc/oldtutorial/templates/">template</a> 
and 
<a href="http://www.cplusplus.com/doc/tutorial/preprocessor/">macro</a>
to make serialization automatically.
<br/>
It's not a json library. It is based on <a href="https://github.com/nlohmann/json">nlohmann/json</a>.
</p>

## Getting Started

- <b>Include the headers </b><br/>
```
#include "rtti/Rtti.h"
#include "rtti/Serializer.h"
```

- <b>Declare your struct by the provided macro</b><br/>
```
RTTI_STRUCT_START(Point)
    RTTI_FIELD(0, x, int, "E")
    RTTI_FIELD(1, y, int, "E")
RTTI_STRUCT_END()
```

- <b>Enjoy it</b><br>
```
Point point {
    .x = 100,
    .y = 200
};

rtti::SerialStream outputStream;
rtti::Serializer::serial(outputStream, point);

std::cout << outputStream.dump(4) << std::endl;
```

## Authors
* **ZhangYeqi**. <a href="mailto:hello.zhangyeqi@gmail.com">send me an email</a>.

## License
The class is licensed under the <a href="https://opensource.org/licenses/MIT">MIT License</a>.