#pragma once

#include "abstract/g3d/ResAnmChr.hh"
#include "abstract/g3d/ResDic.hh"

// Credit: kiwi515/ogws

namespace Abstract {
namespace g3d {

/// @brief Represents a binary resource file which contains object models, textures, and animations.
/// @details All resource files start with a header of length 0x10. The header is followed by a root
/// section (represented as @ref TopLevelDictData) which uses a @ref ResDic to point to different
/// sections of the file.
class ResFile {
public:
    /// @brief The "root" section of the file.
    struct TopLevelDictData {
        ResBlockHeaderData header;
        ResDic::Data topLevel;
    };

    struct Data {
        ResFileHeaderData header;
        TopLevelDictData dict;
    };

    ResFile(const void *data) : m_data(reinterpret_cast<const Data *>(data)) {}

    /// @addr{0x8004C780}
    /// @brief Retrieves the AnmChr section from the binary file.
    [[nodiscard]] ResAnmChr resAnmChr(const char *pName) const {
        ResDic dic = ResDic(&m_data->dict.topLevel);
        void *dicData = dic["AnmChr(NW4R)"];
        ASSERT(dicData);
        void *anmChrData = ResDic(dicData)[pName];
        ASSERT(anmChrData);
        return ResAnmChr(anmChrData);
    }

private:
    const Data *m_data;
};

} // namespace g3d
} // namespace Abstract
