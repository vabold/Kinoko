# KRKG File Format
KRKG files contain information required to sync Time Trial ghosts in Kinoko. Instructions on generating KRKG files can be found [here](https://github.com/vabold/Kinoko/blob/main/README.md#create-krkg), and the source code for the build can be found [here](https://github.com/vabold/mkw-sp/tree/krkg).
KRKG files are big-endian.

## Header
A KRKG file begins with a 16 byte header.
|Offset|Type|Length|Description|
|---|---|---|---|
|0x00|u32|4|Signature; always `0x4b524b47` (KRKG)|
|0x04|u16|2|BOM; `0xFEFF` in big-endian|
|0x06|u16|2|Frame count|
|0x08|u16|2|Major version|
|0x0A|u16|2|Minor version|
|0x0C|u32|4|Offset to packet data|
|0x10|||End of header; start of packet data|

## Packet Data
Packets are `0x64` (100) bytes in length. Each packet corresponds to a frame. The first packet consists of data immediately after initialization, and subsequent packets map to each subsequent per-frame update. Frame data is polled at the end of `RaceScene::calcEnginesUnpaused`.
|Offset|Type|Length|Description|Version Added|
|---|---|---|---|---|
|0x00|`EGG::Vector3f`|12|`pos`|0.1|
|0x0C|`EGG::Quatf`|16|`fullRot`|0.1|
|0x1C|`EGG::Vector3f`|12|`extVel`|0.2|
|0x28|`EGG::Vector3f`|12|`intVel`|0.3|
|0x34|f32|4|`speed`|0.4|
|0x38|f32|4|`acceleration`|0.4|
|0x3C|f32|4|`softSpeedLimit`|0.4|
|0x40|`EGG::Quatf`|16|`mainRot`|0.5|
|0x50|`EGG::Vector3f`|12|`angVel2`|0.5|
|0x5C|f32|4|`raceCompletion`|0.6|
|0x60|u16|2|`checkpointId`|0.6|
|0x62|u8|1|`jugemId`|0.6|
|0x63|u8|1|Padding|0.6|

`angVel2` is always 0, and is scheduled to be removed in the next major version bump.
