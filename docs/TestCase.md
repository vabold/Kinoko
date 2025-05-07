# Test Suite Binary Format
Test suite binary files contain a suite of test cases to test in Kinoko. They are generated via Python script [here](https://github.com/vabold/Kinoko/blob/main/tools/generate_tests.py).
Suite files are big-endian.

## Header
Test suite files have an 6 byte header.
|Offset|Type|Length|Description|
|---|---|---|---|
|0x00|u16|2|Number of test cases|
|0x02|u16|2|Suite major version|
|0x04|u16|2|Suite minor version|
|0x06|||End of header; start of test cases|

## Test Case Format
Test cases specify the target sync frame and which RKG and KRKG files to sync. Padding and alignment _do not apply_. The total length of a given test case is variable.
|Offset|Type|Length|Description|
|---|---|---|---|
|0x00|u32|4|Case signature; `0x54535448` (TSTH)|
|0x04|u16|2|Case total size (exclusive of self and footer)|
|0x06|u16|2|***N*** = Case name length|
|0x08|String|***N***|Case name|
|0x08 + ***N***|u16|2|***R*** = RKG path length|
|0x0A + ***N***|String|***R***|RKG path|
|0x0A + ***N*** + ***R***|u16|2|***K*** = KRKG path length|
|0x0C + ***N*** + ***R***|String|***K***|KRKG path|
|0x0C + ***N*** + ***R*** + ***K***|u16|2|Target frame|
|0x0E + ***N*** + ***R*** + ***K***|u32|4|Case footer; `0x54535446` (TSTF)|
