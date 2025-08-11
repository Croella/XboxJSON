
# XboxJSON

JSON Parser And Formatter for RGH / Xenia modules. You require a few things to make this work. 

XexUtils by Clem, which can be found [Here](https://github.com/ClementDreptin/XexUtils)

THESE INCLUDES 
```C
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
```

and lastly a write file function ( use std::ofstream!! )

# HOW TO USE EXAMPLE
```c
std::string content;
if (!utils::json::read_file_to_string("config.json", content))
    printf("content not found");
		
if(!utils::json::parse_json(content, utils::json::globObj))
    printf("failed to parse content");
			
bool value;
utils::json::get_value(utils::json::globObj, "key", value, true);
```

if you are using a delimiter(.) , use `utils::json::get_section_key("SECTION", "KEY")`

# Simple write file function
```c
inline bool write_file(std::string path, void* data, int32_t size)
{
    XexUtils::Xam::MountHdd();
    std::ofstream file;

    file.open(filePath.c_str(), std::ios::binary);
    file.write((const char*)data, size);
    file.close();

    return true;
}
```