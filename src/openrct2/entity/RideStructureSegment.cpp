/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideStructureSegment.h"

#include "../GameState.h"
#include "../SpriteIds.h"
#include "../core/DataSerialiser.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/RideEntry.h"
#include "../ride/Vehicle.h"
#include "../world/Location.hpp"
#include "../world/Map.h"
#include "EntityList.h"
#include "EntityRegistry.h"

#include <unordered_map>
#include <utility>
#include <vector>

namespace OpenRCT2
{
    template<>
    bool EntityBase::is<RideStructureSegment>() const
    {
        return type == EntityType::rideStructureSegment;
    }

    RideStructureSegment* RideStructureSegment::Create(
        const CoordsXYZ& pos, uint32_t imageId, uint8_t sliceIndex, uint8_t sliceCount, uint8_t bboxSizeZ)
    {
        auto* seg = getGameState().entities.CreateEntity<RideStructureSegment>();
        if (seg == nullptr)
            return nullptr;

        // Give the entity a bbox-derived sprite bound so the painter's cull rect covers the whole
        // slice. width/heightMin/heightMax are uint8 (from centre to edge); a slice is small enough
        // to fit without the tall-sprite clamping the flat-ride structure needs.
        seg->spriteData.width = 24;
        seg->spriteData.heightMin = 24;
        seg->spriteData.heightMax = static_cast<uint8_t>(bboxSizeZ + 8);
        seg->moveTo(pos);
        seg->rideId = RideId::GetNull();
        seg->baseImageId = imageId;
        seg->baseFrame = 0;
        seg->sliceIndex = sliceIndex;
        seg->sliceCount = sliceCount;
        seg->segmentGroup = 0;
        seg->bboxSizeZ = bboxSizeZ;
        return seg;
    }

    void RideStructureSegment::Update()
    {
        // Static segments (step 1) never change; nothing to do. Spin/swing modes will recompute the
        // frame / off-grid position here and invalidate, driven from the owning ride's update pass.
    }

    void RideStructureSegment::serialise(DataSerialiser& stream)
    {
        EntityBase::serialise(stream);
        stream << rideId;
        stream << baseImageId;
        stream << baseFrame;
        stream << sliceIndex;
        stream << sliceCount;
        stream << segmentGroup;
        stream << bboxSizeZ;
    }

    // rideId -> (count, height). THROWAWAY runtime override (see header); empty in normal play.
    static std::unordered_map<uint16_t, std::pair<uint8_t, uint8_t>> gSegmentOverride;

    // Effective (count, height) for the ENTITY path — now the console debug probe ONLY. A normal
    // authored large ride draws its z-sliced structure INLINE in the track paint
    // (PaintGenericRotatingStructure), NOT via entities: that paint is already invoked once per
    // footprint tile, which spreads the sprite across screen columns for free — something a
    // single-tile-anchored entity cannot do (an entity is only painted in the ~2 columns that reach its
    // home tile, so a wide ride would clip to ~2 columns). Entities therefore spawn ONLY when a
    // seg_stack/ride_segments override is active; without one this returns {0,0} and no entities are
    // created (nor does GenericFlatRide suppress the real structure draw). The entity subsystem stays
    // for a FUTURE off-grid animated vehicle — a swinging pendulum arm that leaves the footprint — which
    // genuinely needs a movable sprite the per-tile track paint can't reach.
    static std::pair<uint8_t, uint8_t> EffectiveSegments(const Ride& ride)
    {
        const auto it = gSegmentOverride.find(ride.id.ToUnderlying());
        if (it != gSegmentOverride.end())
            return it->second;
        return { 0, 0 };
    }

    uint8_t RideStructureSegmentGetEffectiveCount(const Ride& ride)
    {
        return EffectiveSegments(ride).first;
    }

    void RideStructureSegmentSetOverride(RideId rideId, uint8_t count, uint8_t height)
    {
        if (count == 0)
            gSegmentOverride.erase(rideId.ToUnderlying());
        else
            gSegmentOverride[rideId.ToUnderlying()] = { count, height };
    }

    void RideStructureSegmentsEnsure(Ride& ride)
    {
        const auto [count, heightRaw] = EffectiveSegments(ride);
        // Early out for the overwhelming majority of rides (no segments) BEFORE scanning the entity
        // list — this runs every tick for every ride. A ride that just had its count cleared has its
        // stragglers removed by the explicit RideStructureSegmentsRemove call at that site (console
        // override / RideDelete), so nothing to clean up here.
        if (count == 0)
            return;

        const uint8_t sliceH = heightRaw > 0 ? heightRaw : 16;

        // Gather this ride's live segments once (segments carry their owning rideId).
        std::vector<RideStructureSegment*> mine;
        for (auto* seg : EntityList<RideStructureSegment>())
        {
            if (seg->rideId == ride.id)
                mine.push_back(seg);
        }

        // Count changed (e.g. override 10 -> 5) or stragglers present: rebuild cleanly from scratch.
        if (!mine.empty() && mine.size() != count)
        {
            for (auto* seg : mine)
                getGameState().entities.EntityRemove(seg);
            mine.clear();
        }

        // overallView is the ride's representative tile (world coords); null until the first track
        // piece lands. Placeholder placement at that tile's centre is fine for the engine milestone;
        // pixel-accurate centring on the footprint is a later concern.
        if (ride.overallView.IsNull())
            return;

        const CoordsXY loc = ride.overallView.ToTileCentre();
        const int32_t baseZ = TileElementHeight(loc);

        if (mine.empty())
        {
            // A console-forced ride (ride_segments) draws the SPR_BALLOON placeholder wave, because its
            // atlas is the normal unsliced structure sheet; a descriptor-driven ride draws its REAL
            // sliced structure block from Cars[0].base_image_id. segmentGroup records which: 0 =
            // placeholder, 1 = real sliced structure (read by the paint).
            const bool isOverride = gSegmentOverride.find(ride.id.ToUnderlying()) != gSegmentOverride.end();
            uint32_t structBase = SPR_BALLOON;
            if (!isOverride)
            {
                const auto* entry = GetRideEntryByIndex(ride.subtype);
                structBase = (entry != nullptr) ? entry->Cars[0].base_image_id : 0;
            }

            // Per-column paint + invalidation bounds. EntityPaintSetup only paints an entity in the
            // screen columns its spriteData covers, so a too-small width clips a WIDE ride to a narrow
            // central column and leaves stale/flickering edges under animation. Cover the ride's full
            // sprite extent from the descriptor's invalidation bounds (uint8-clamped exactly as the
            // monolithic structure paint does for its frame-carrier vehicle); fall back to the uint8 max
            // when a bound is unset. A tall invalidation is fine — sorting is still local because each
            // slice's BBOX z stays short (bboxSizeZ); only the dirty/paint-selection rect grows.
            const auto& desc = GetFlatRideDescriptor(ride);
            const auto boundOrMax = [](uint16_t v) -> uint8_t {
                return v == 0 ? 255 : static_cast<uint8_t>(v > 255 ? 255 : v);
            };
            const uint8_t sdWidth = boundOrMax(desc.InvalidationHalfWidth);
            const uint8_t sdAbove = boundOrMax(desc.InvalidationHeightAbove);
            const uint8_t sdBelow = boundOrMax(desc.InvalidationHeightBelow);

            // Spawn. Covers fresh placement AND post-load (segments are never serialized, so a
            // just-loaded ride has none).
            for (uint8_t i = 0; i < count; i++)
            {
                auto* seg = RideStructureSegment::Create(
                    { loc.x, loc.y, baseZ + i * sliceH }, structBase, i, count, sliceH);
                if (seg != nullptr)
                {
                    seg->rideId = ride.id;
                    seg->segmentGroup = isOverride ? 0 : 1;
                    seg->spriteData.width = sdWidth;
                    seg->spriteData.heightMax = sdAbove;
                    seg->spriteData.heightMin = sdBelow;
                }
            }
            // Freshly-created entities aren't findable by the painter until the per-tick spatial-index
            // rebuild runs — force it so the structure appears the moment the ride is placed.
            getGameState().entities.UpdateEntitiesSpatialIndex();
            return;
        }

        // FOLLOW THE RIDE: segments already exist, so reposition them to the ride's CURRENT location.
        // This is what makes the column track a ride that is moved in construction mode (delete +
        // re-place elsewhere keeps the same ride id — RideDelete never fires — so overallView updates
        // but the segments would otherwise stay stranded at their original spot). Only touch the
        // spatial index when something actually moved, to avoid per-tick churn on a stationary ride.
        bool moved = false;
        for (auto* seg : mine)
        {
            const int32_t targetZ = baseZ + seg->sliceIndex * sliceH;
            if (seg->x != loc.x || seg->y != loc.y || seg->z != targetZ)
            {
                seg->moveTo({ loc.x, loc.y, targetZ });
                moved = true;
            }
        }
        if (moved)
            getGameState().entities.UpdateEntitiesSpatialIndex();
    }

    void RideStructureSegmentsUpdate(Ride& ride)
    {
        if (RideStructureSegmentGetEffectiveCount(ride) == 0)
            return;

        // Read the ride's CURRENT animation frame off its frame-carrier vehicle — the same source
        // the monolithic structure paint used (vehicle->flatRideAnimationFrame % FramesPerDir). When
        // the ride is closed / has no vehicle, the frame holds at 0 and the column is static.
        const auto& desc = GetFlatRideDescriptor(ride);
        uint16_t animFrame = 0;
        if (desc.FramesPerDir > 0 && ride.flags.has(RideFlag::onTrack) && !ride.vehicles[0].IsNull())
        {
            const auto* vehicle = getGameState().entities.GetEntity<Vehicle>(ride.vehicles[0]);
            if (vehicle != nullptr)
                animFrame = vehicle->flatRideAnimationFrame % desc.FramesPerDir;
        }

        // Push the frame onto each of this ride's segments and invalidate only those that changed —
        // so a stationary/closed ride costs nothing, while a running one re-images every tick. The
        // segments don't move here (SPIN), so a screen invalidate is enough; no spatial-index rebuild.
        for (auto* seg : EntityList<RideStructureSegment>())
        {
            if (seg->rideId != ride.id)
                continue;
            if (seg->baseFrame != animFrame)
            {
                seg->baseFrame = animFrame;
                seg->invalidate();
            }
        }
    }

    void RideStructureSegmentsRemove(RideId rideId)
    {
        // Collect first — EntityRemove mutates the entity list, so we can't remove mid-iteration.
        std::vector<RideStructureSegment*> doomed;
        for (auto* seg : EntityList<RideStructureSegment>())
        {
            if (seg->rideId == rideId)
                doomed.push_back(seg);
        }
        for (auto* seg : doomed)
            getGameState().entities.EntityRemove(seg);
    }
} // namespace OpenRCT2
