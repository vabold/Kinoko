#pragma once

#include <Common.hh>

#include <cstring>

// Credit: kiwi515/ogws

namespace Abstract {
namespace g3d {

/// @brief Essentially a lookup table to find different sections within the resource file.
class ResDic {
public:
    struct NodeData {
        u16 ref;
        u16 flag;
        u16 idxLeft;
        u16 idxRight;
        s32 ofsString;
        s32 ofsData;
    };
    STATIC_ASSERT(sizeof(NodeData) == 0x10);

    struct Data {
        u32 size;
        u32 numData;
        NodeData data[1];
    };

    ResDic(const void *data);

    /// @addr{0x8004C050}
    [[nodiscard]] void *operator[](const char *name) const {
        if (!m_data || !name) {
            return nullptr;
        }

        const NodeData *node = get(name, std::strlen(name));

        if (node) {
            return reinterpret_cast<void *>(
                    reinterpret_cast<uintptr_t>(m_data) + parse<s32>(node->ofsData));
        }

        return nullptr;
    }

private:
    [[nodiscard]] const NodeData *get(const char *pName, u32 len) const;

    const Data *m_data;
};

} // namespace g3d
} // namespace Abstract
