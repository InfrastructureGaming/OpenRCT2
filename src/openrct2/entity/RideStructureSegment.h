/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Identifiers.h"
#include "EntityBase.h"

struct CoordsXYZ;
// NB: the engine's Ride struct lives in the GLOBAL namespace (ride/Ride.h), so it must be
// forward-declared here at global scope — declaring it inside namespace OpenRCT2 would create a
// spurious OpenRCT2::Ride that clashes with ::Ride (ambiguous-symbol errors in Ride.cpp).
struct Ride;

namespace OpenRCT2
{
    class DataSerialiser;

    /**
     * A single short (<128px) vertical slice of a large ride's structure, hosted as its own
     * entity.
     *
     * WHY THIS EXISTS: a flat ride's structure is normally painted as ONE tile-anchored sprite at
     * the footprint base Z (GenericFlatRide.cpp). Once that sprite is tall, a neighbouring tile's
     * partial redraw re-sorts its far-from-anchor top against an incomplete local paint set and it
     * flickers (the "tall-sprite wall"). Splitting the structure into a stack of short slice
     * entities, each with a bbox at its OWN Z, makes every slice sort locally against its true
     * neighbours — the property the balloon-hijack spike proved flicker-free.
     *
     * WHY AN ENTITY (not a tile element): a large *animated* vehicle — a rotating Ferris wheel rim,
     * or a big pendulum arm that is off the tile grid mid-swing — cannot be expressed as
     * grid-locked tile elements at all. An entity can sit at any sub-tile (x,y,z) and move smoothly,
     * so the same primitive covers static supports, spinning wheels AND swinging arms. "Static vs
     * animated" is therefore a per-segment behaviour (segmentGroup), not a separate subsystem.
     *
     * Deliberately NOT a Vehicle: vehicles carry peeps and velocity/track state and are walked by
     * guest/boarding code; a support-truss slice is non-boardable. The peep-carrying gondolas stay
     * Vehicles; only the tall non-boarding geometry becomes segments.
     *
     * Most drawing state (base image, colours, current animation frame) is DERIVED from the owning
     * ride at paint time so a segment can never drift out of sync with its ride. The fields here are
     * only what the segment itself must know: which slice it is, and (for the throwaway probe) a
     * placeholder image.
     */
    struct RideStructureSegment : EntityBase
    {
        static constexpr auto cEntityType = EntityType::rideStructureSegment;

        RideId rideId;          // owning ride (null for the standalone probe)
        uint32_t baseImageId;   // slice-atlas base; probe uses a fixed placeholder sprite
        uint16_t baseFrame;     // frame within the atlas (spin/swing modes override at paint time)
        uint8_t sliceIndex;     // which Z-slice of the column this segment draws
        uint8_t sliceCount;     // total slices in the column (for image indexing)
        uint8_t segmentGroup;   // 0 = static structure; >0 = an animated part id (future spin/swing)
        uint8_t bboxSizeZ;      // slice height in world units for the bbox (kept < 128 to sort local)

        static RideStructureSegment* Create(
            const CoordsXYZ& pos, uint32_t imageId, uint8_t sliceIndex, uint8_t sliceCount, uint8_t bboxSizeZ);

        // No-op for static segments (step 1). Spin/swing modes will drive frame/position here,
        // called from the owning ride's own update pass (ride-owned lifecycle, not misc-updated).
        void Update();
        void serialise(DataSerialiser& stream);
    };

    // Ride-owned lifecycle for a large ride's structure segments. The segments are a render-time
    // decomposition of the ride's current state — EPHEMERAL: respawned, never serialized.
    //   Ensure: if the ride's descriptor asks for N segments and it has none live, spawn them.
    //           Called every tick from Ride::Update, so it covers both fresh placement AND load.
    //   Remove: destroy every segment owned by the ride. Called from RideDelete (all demolish paths).
    // (Ride is the global ::Ride forward-declared above.)
    void RideStructureSegmentsEnsure(Ride& ride);
    //   Update: push the ride's current animation frame onto its segments (SPIN). Called each tick
    //           from Ride::Update, after Ensure. Invalidates only segments whose frame changed.
    void RideStructureSegmentsUpdate(Ride& ride);
    void RideStructureSegmentsRemove(RideId rideId);

    // Effective segment count for a ride: the runtime test override if one is set (console
    // `ride_segments`), otherwise the parkobj descriptor's StructureSegmentCount. Read by BOTH the
    // spawn path (Ensure) and the paint-suppression path (PaintGenericRotatingStructure) so a
    // force-enabled ride behaves identically to an authored one — no double-draw.
    // THROWAWAY: the override exists only because there is no tool UI yet to author the field; retire
    // it together with the placeholder imagery once the slice-atlas tool lands.
    uint8_t RideStructureSegmentGetEffectiveCount(const Ride& ride);
    void RideStructureSegmentSetOverride(RideId rideId, uint8_t count, uint8_t height);
} // namespace OpenRCT2
