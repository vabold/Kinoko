#pragma once

#include <Types.hh>

#include "game/field/CollisionInfo.hh"
#include "game/field/KCollisionTypes.hh"

#include <egg/math/BoundBox.hh>

// Credit: em-eight/mkw
// Credit: stblr/Hanachan

namespace Kinoko {

namespace EGG {

struct Matrix34f;

} // namespace EGG

namespace Field {

/// @brief Interface that parses a `.kcl` file and allows for spatial querying of KCL triangles
/// @details KCL triangle data is arranged in an octree which allows for more efficient spatial
/// queries.
class KColData {
public:
    /// @brief Describes the type of collision check to perform
    enum class CollisionCheckType {
        Edge,     ///< Checks if a body is colliding with any edge of the prism
        Plane,    ///< Checks if a body is colliding with a plane of the prism
        Movement, ///< Plane collisions that occur when traveling towards the face of the prism
    };

    /// @brief Represents a KCL collision prism
    struct KCollisionPrism {
        f32 height;    ///< The height of the tri
        u16 pos_i;     ///< Index of the first vertex's position in the KCL file's position data
        u16 fnrm_i;    ///< Index of the face normal in the KCL file's normal data
        u16 enrm1_i;   ///< Index of the first edge's normal in the KCL file's normal data
        u16 enrm2_i;   ///< Index of the second edge's normal in the KCL file's normal data
        u16 enrm3_i;   ///< Index of the third edge's normal in the KCL file's normal data
        u16 attribute; ///< KCL attribute of the tri

        /// @brief Non-initializing default constructor
        KCollisionPrism() = default;

        /// @brief Default destructor
        ~KCollisionPrism() = default;

        /// @brief Initializes the prism by reading its data from the provided stream
        /// @param stream The stream from which to read the prism data
        void read(EGG::RamStream &stream) {
            height = stream.read_f32();
            pos_i = stream.read_u16();
            fnrm_i = stream.read_u16();
            enrm1_i = stream.read_u16();
            enrm2_i = stream.read_u16();
            enrm3_i = stream.read_u16();
            attribute = stream.read_u16();
        }
    };

    STATIC_ASSERT(sizeof(KCollisionPrism) == 0x10);

    /// @brief Stores the vertex, height and normals of a collision prism
    /// @details We introduce this struct in Kinoko in order to pre-fetch a prism's geometry rather
    /// than accessing these fields from the various different arrays. This is a small performance
    /// improvement due to cache locality.
    struct alignas(0x40) KCollisionPrismGeometry {
        EGG::Vector3f vtx;   ///< The first vertex of the tri
        EGG::Vector3f enrm1; ///< The first edge's normal
        EGG::Vector3f enrm2; ///< The second edge's normal
        EGG::Vector3f enrm3; ///< The third edge's normal
        EGG::Vector3f fnrm;  ///< The face normal
        f32 height;          ///< The height of the tri
    };

    STATIC_ASSERT(sizeof(KCollisionPrismGeometry) == 0x40);

    KColData(const void *file);
    ~KColData();

    /// @beginPublicColCheckFuncs

    void narrowScopeLocal(const EGG::Vector3f &pos, f32 radius, KCLTypeMask mask);

    [[nodiscard]] bool checkPointCollision(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *flagsOut);
    [[nodiscard]] bool checkSphereCollision(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *flagsOut);

    void lookupPoint(const EGG::Vector3f &pos, const EGG::Vector3f &prevPos, KCLTypeMask typeMask);
    void lookupSphere(f32 radius, const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
            KCLTypeMask typeMask);
    void lookupSphereCached(f32 radius, const EGG::Vector3f &pos, const EGG::Vector3f &prevPos,
            u32 typeMask);

    /// @endPublicColCheckFuncs

    /// @beginGetters

    /// @brief Gets the bounding box containing all KCL tris
    /// @return The bounding box encompassing all KCL triangles, @ref m_bbox
    [[nodiscard]] const EGG::BoundBox3f &bbox() const {
        return m_bbox;
    }

    /// @brief Gets the index of the prism stored in @ref m_prismCache at the specified index `idx`
    /// @param idx The index in the cache to retrieve the prism index from
    /// @return The prism index stored at the specified cache index in @ref m_prismCache
    [[nodiscard]] u16 prismCache(u32 idx) const {
        return m_prismCache[idx];
    }

    /// @brief One-indexed array of each prism's geometry. Entry 0 is unused.
    [[nodiscard]] std::span<const KCollisionPrismGeometry> prismGeometry() const {
        return m_prismGeometry.view();
    }

    /// @brief One-indexed array of each prism's KCL attribute. Entry 0 is unused.
    [[nodiscard]] std::span<const u16> prismAttributes() const {
        return m_prismAttributes.view();
    }

    /// @endGetters

private:
    void preloadPrismGeometry();
    void computeBBox();

    /// @beginPrivateColCheckFuncs

    void narrowPolygon_EachBlock(const u16 *prismArray);
    [[nodiscard]] const u16 *searchBlock(const EGG::Vector3f &pos);
    [[nodiscard]] bool checkSphereSingle(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *flagsOut);
    [[nodiscard]] bool checkPoint(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *attributeOut,
            bool movement);
    [[nodiscard]] bool checkPointCollision(u16 prismIdx, f32 *distOut, EGG::Vector3f *fnrmOut,
            u16 *flagsOut, bool movement);

    template <CollisionCheckType Type>
        requires(Type == CollisionCheckType::Plane || Type == CollisionCheckType::Movement)
    [[nodiscard]] bool checkSphereImpl(f32 *distOut, EGG::Vector3f *fnrmOut, u16 *flagsOut);

    template <CollisionCheckType Type>
    [[nodiscard]] bool checkSphereCollision(u16 prismIdx, f32 *distOut, EGG::Vector3f *fnrmOut,
            u16 *flagsOut);

    /// @endPrivateColCheckFuncs

    const void *m_posData;      ///< Pointer to the KCL file section containing vertex positions
    const void *m_nrmData;      ///< Pointer to the KCL file section containing normal vectors
    const void *m_prismData;    ///< Pointer to the KCL file section containing the prism data
    const void *m_blockData;    ///< Pointer to the KCL file section containing the octree
    f32 m_prismThickness;       ///< The depth of all prisms along their normal vector
    EGG::Vector3f m_areaMinPos; ///< Smallest possible coordinate in the octree
    u32 m_areaXWidthMask;     ///< The x dimension of the octree's bounding box. @see searchBlock()
    u32 m_areaYWidthMask;     ///< The y dimension of the octree's bounding box. @see searchBlock()
    u32 m_areaZWidthMask;     ///< The z dimension of the octree's bounding box. @see searchBlock()
    u32 m_blockWidthShift;    ///< Used to initialize octree navigation. @see searchBlock()
    u32 m_areaXBlocksShift;   ///< Used to initialize octree navigation. @see searchBlock()
    u32 m_areaXYBlocksShift;  ///< Used to initialize octree navigation. @see searchBlock()
    f32 m_sphereRadius;       ///< Clamps the sphere we check collision against. @see searchBlock()
    EGG::Vector3f m_pos;      ///< The point's/sphere's position for collision queries
    EGG::Vector3f m_prevPos;  ///< The point's/sphere's previous position for collision queries
    EGG::Vector3f m_movement; ///< The difference between @ref m_pos and @ref m_prevPos
    f32 m_radius;             ///< The radius of the sphere to query collisions for
    KCLTypeMask m_typeMask;   ///< The KCL types to filter the collision query to
    const u16 *m_prismIter;   ///< Iterator pointing to the current prism in the octree traversal
    EGG::BoundBox3f m_bbox;   ///< The 3-dimensional bounding box of all prisms in the KCL file
    std::array<u16, 256> m_prismCache; ///< Cache of prism indices to avoid expensive octree lookups
    u16 *m_prismCacheIter;     ///< Pointer to the current prism index in the cache traversal
    EGG::Vector3f m_cachedPos; ///< Position of the point/sphere corresponding to the current cache
    f32 m_cachedRadius;        ///< Radius of the sphere corresponding to the current cache

    /// @brief One-indexed array of each prism's geometry
    /// @details Kinoko-specific optimization for cache locality. See @ref KCollisionPrismGeometry.
    owning_span<KCollisionPrismGeometry> m_prismGeometry;

    /// @brief One-indexed array of each prism's KCL attribute
    /// @details Kept separate from @ref m_prismGeometry so that the attribute check, which rejects
    /// most prisms, does not need to touch the geometry.
    owning_span<u16> m_prismAttributes;

    [[nodiscard]] static EGG::Vector3f GetVertex(f32 height, const EGG::Vector3f &vertex1,
            const EGG::Vector3f &fnrm, const EGG::Vector3f &enrm3, const EGG::Vector3f &enrm);
};

} // namespace Field

} // namespace Kinoko
