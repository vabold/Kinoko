#pragma once

#include "egg/core/Heap.hh"

#include <abstract/memory/FrameHeap.hh>

namespace Kinoko::EGG {

class FrmHeap : public Heap {
public:
    ~FrmHeap() override;
    void destroy() override;

    /// @addr{0x80226EFC}
    [[nodiscard]] Kind getHeapKind() const override {
        return Heap::Kind::Frame;
    }

    [[nodiscard]] void *alloc(size_t size, s32 align) override;
    void free(void *block) override;
    [[nodiscard]] u32 getAllocatableSize(s32 align = 4) const override;

    [[nodiscard]] Abstract::Memory::MEMiFrmHeapHead *dynamicCastHandleToFrm();
    [[nodiscard]] const Abstract::Memory::MEMiFrmHeapHead *dynamicCastHandleToFrm() const;

    [[nodiscard]] static FrmHeap *create(void *startAddress, size_t size, u16 opt);
    [[nodiscard]] static FrmHeap *create(size_t size, Heap *heap, u16 opt);

private:
    FrmHeap(Abstract::Memory::MEMiHeapHead *handle);
};

} // namespace Kinoko::EGG
