/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../GameState.h"
#include "../../../entity/EntityRegistry.h"
#include "../../../interface/Viewport.h"
#include "../../../ride/Ride.h"
#include "../../../ride/RideData.h"
#include "../../../ride/RideEntry.h"
#include "../../../ride/TrackData.h"
#include "../../../ride/TrackPaint.h"
#include "../../../ride/Vehicle.h"
#include "../../Boundbox.h"
#include "../../Paint.h"
#include "../../support/WoodenSupports.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"

#include <algorithm>
#include <utility>

using namespace OpenRCT2;

// Returns the FlatRideRotationDescriptor for a ride, preferring a descriptor parsed
// from the ride's parkobj JSON ("flatRideAnimation" block) over the compiled RTD default.
// Allows fully data-driven flat rides defined in a parkobj with no per-ride C++ code.
static const FlatRideRotationDescriptor& GetFlatRideDescriptor(const Ride& ride)
{
    const auto* entry = GetRideEntryByIndex(ride.subtype);
    if (entry != nullptr && entry->flatRideAnimation != nullptr)
        return *entry->flatRideAnimation;
    return GetRideTypeDescriptor(ride.type).FlatRideRotation;
}

/**
 * Draws one structure tile for a generic rotation-style flat ride.
 *
 * Image formula: BaseImageId + (direction * FramesPerDir) + animFrame
 * Guard: BaseImageId == 0 skips draw (ride not yet configured with sprites).
 */
static void PaintGenericRotatingStructure(
    PaintSession& session, const Ride& ride, uint8_t direction, int8_t xOffset, int8_t yOffset, uint16_t height,
    ImageId stationColour)
{
    const auto& desc = GetFlatRideDescriptor(ride);
    if (desc.FramesPerDir == 0)
        return;

    const auto* rideEntry = GetRideEntryByIndex(ride.subtype);
    if (rideEntry == nullptr)
        return;

    const uint32_t baseImageId = rideEntry->Cars[0].base_image_id;
    if (baseImageId == 0)
        return;

    uint16_t animFrame = 0;
    Vehicle* vehicle = nullptr;
    if (ride.flags.has(RideFlag::onTrack) && !ride.vehicles[0].IsNull())
    {
        vehicle = getGameState().entities.GetEntity<Vehicle>(ride.vehicles[0]);
        session.InteractionType = ViewportInteractionItem::entity;
        session.CurrentlyDrawnEntity = vehicle;
        if (vehicle != nullptr)
        {
            // Expand the entity's dirty-rect if the ride footprint is larger than the
            // vehicle's native spriteData bounds (set at vehicle init from carEntry).
            // Without this, invalidate() only marks dirty a small area around the entity's
            // origin tile; the visual (drawn at the footprint centre) falls outside that
            // area in camera rotations 1/2/3, causing direction-dependent sprite staleness.
            if (desc.InvalidationHalfWidth > 0)
                vehicle->spriteData.width = desc.InvalidationHalfWidth;
            if (desc.InvalidationHeightAbove > 0)
                vehicle->spriteData.heightMin = desc.InvalidationHeightAbove;
            if (desc.InvalidationHeightBelow > 0)
                vehicle->spriteData.heightMax = desc.InvalidationHeightBelow;
            animFrame = vehicle->flatRideAnimationFrame % desc.FramesPerDir;
        }
    }

    // Mirrors Enterprise's structure draw exactly: the bb offset is tile-local (0,0,height+7),
    // NOT redirected to the centre like the image draw position is. This lets each of the
    // redundant draws sort correctly against its own host tile's neighbours, while the sprite
    // itself is redrawn at the shared centre point so it visually appears in the right place.
    CoordsXYZ offset(xOffset, yOffset, height + desc.StructureZOffset);
    BoundBoxXYZ bb = { { 0, 0, height + desc.StructureZOffset }, { 24, 24, 48 } };

    // Map the three ride colours to the three remap ranges: Body→primary (243-254, "Main
    // Color"), Trim→secondary (202-213, "Additional Color 1"), Tertiary→tertiary (46-57,
    // "Additional Color 2"). The primary range used to be unreachable - openrct2-cli's
    // -m closest excludes 243-254 (GetClosestPaletteIndex), so a Blender render could never
    // produce those pixels - which is why this once routed Body→secondary and skipped
    // primary entirely. The zone-pass authoring path (build/zone_mask.py + an authored
    // COLOR_PRIMARY mask) places primary-range pixels deliberately, so all three ranges are
    // now real, distinct, player-recolourable colours.
    auto imageTemplate = ImageId(0, ride.vehicleColours[0].Body, ride.vehicleColours[0].Trim, ride.vehicleColours[0].Tertiary);
    if (stationColour != TrackStationColour)
        imageTemplate = stationColour;
    // BaseRotation (0-3 quarter-turns) turns the ride relative to its footprint by picking the
    // sprite for a rotated view direction; the draw position (offset/bb) is unaffected. Lets a
    // wider-than-long ride sit correctly on a footprint the engine normalized to width<=length.
    // SymmetricDirections folds views 2,3 -> 0,1 (numDirs=2): a 180-degree-symmetric ride stores
    // only 2 direction-blocks, so the atlas (structure + every rider) is half the size.
    const uint8_t numDirs = desc.SymmetricDirections ? 2 : 4;
    const uint8_t spriteDirection = ((direction + desc.BaseRotation) & 3) % numDirs;
    auto imageId = imageTemplate.WithIndex(baseImageId + spriteDirection * desc.FramesPerDir + animFrame);
    PaintAddImageAsParent(session, imageId, offset, bb);

    // Rider overlays — each gondola has its own full numDirs-direction x FramesPerDir sheet,
    // immediately following the main structure block and (for gondola g) every prior
    // gondola's block: base + (g+1) * numDirs*FramesPerDir. Each sheet shows both of that
    // gondola's seats; the first is recoloured via secondary remap and the second via
    // tertiary (primary remap is unreachable from the -m closest Blender pipeline, so it's
    // an unused placeholder). Only drawn at zoom 0.
    //
    // The ride is ONE train of M gondola cars (object_json's num_gondola_cars), so walk
    // next_vehicle_on_train and draw gondola g from car c's OWN peep[]/shirt colours. A
    // unified gondola->(car, seat-pair) index keeps both layouts working: M==1 (one car
    // holding K gondola-pairs in its peep[]) and M>1 (one pair per car). Guests pick a
    // RANDOM car (existing coaster-style boarding), so a car may be occupied while a later
    // one isn't — advance to the next car rather than breaking globally.
    if (vehicle != nullptr && desc.RiderFrameStride > 0 && session.rt.zoom_level <= ZoomLevel{ 0 })
    {
        const uint32_t structureBlockSize = numDirs * desc.FramesPerDir;
        const uint8_t numGondolas = desc.RiderFrameStride;
        uint8_t g = 0;
        for (Vehicle* car = vehicle; car != nullptr && g < numGondolas;
             car = getGameState().entities.GetEntity<Vehicle>(car->next_vehicle_on_train))
        {
            const uint8_t seatsInCar = car->num_seats & kVehicleSeatNumMask;
            for (uint8_t s = 0; s + 1 < seatsInCar && g < numGondolas; s += 2, g++)
            {
                // g is the gondola's fixed PHYSICAL slot (sprite index). Skip an empty pair's
                // draw but still advance g, so an unoccupied car leaves its slot empty rather
                // than packing later cars' riders into the low gondola indices (which made
                // random boarding re-display as a clockwise fill). Seats fill in order within a
                // car, so s >= num_peeps means this pair is empty.
                if (s >= car->num_peeps)
                    continue;
                const uint32_t riderIdx = baseImageId
                    + static_cast<uint32_t>(g + 1) * structureBlockSize
                    + spriteDirection * desc.FramesPerDir
                    + animFrame;
                ImageId riderId;
                if (stationColour != TrackStationColour)
                    riderId = stationColour.WithIndex(riderIdx);
                else
                    riderId = ImageId(0, Drawing::Colour::black, car->peep_tshirt_colours[s],
                                       car->peep_tshirt_colours[s + 1])
                                  .WithIndex(riderIdx);
                PaintAddImageAsChild(session, riderId, offset, bb);
            }
        }
    }

    session.CurrentlyDrawnEntity = nullptr;
    session.InteractionType = ViewportInteractionItem::ride;
}

/**
 * Top-level paint function for generic rotation-style flat rides (4x4 footprint).
 * Structure is drawn redundantly from 12 of 16 tiles (identical coverage to Enterprise),
 * each redirecting its sprite to the ride's shared centre point. The wider redundant
 * coverage (vs. a 3x3's 6-of-9) is what lets a larger composite sprite survive the
 * per-tile painter's-algorithm sort without being clipped by neighbouring floor tiles.
 */
static void PaintFlatRideGeneric(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kTrackMap4x4[direction][trackSequence];

    const uint8_t edges = kEdges4x4[trackSequence];

    auto stationColour = GetStationColourScheme(session, trackElement);
    WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::truss, WoodenSupportSubType::neSw, direction, height, stationColour);

    const StationObject* stationObject = ride.getStationObject();
    TrackPaintUtilPaintFloor(session, edges, stationColour, height, kFloorSpritesMulch, stationObject);

    TrackPaintUtilPaintFences(
        session, edges, session.MapPosition, trackElement, ride, stationColour, height, kFenceSpritesRope,
        session.CurrentRotation);

    switch (trackSequence)
    {
        case 5:
            PaintGenericRotatingStructure(session, ride, direction, 16, 16, height, stationColour);
            break;
        case 6:
            PaintGenericRotatingStructure(session, ride, direction, 16, -16, height, stationColour);
            break;
        case 10:
            PaintGenericRotatingStructure(session, ride, direction, -16, -16, height, stationColour);
            break;
        case 9:
            PaintGenericRotatingStructure(session, ride, direction, -16, 16, height, stationColour);
            break;

        case 0:
            PaintGenericRotatingStructure(session, ride, direction, 48, 48, height, stationColour);
            break;
        case 3:
            PaintGenericRotatingStructure(session, ride, direction, 48, -48, height, stationColour);
            break;
        case 15:
            PaintGenericRotatingStructure(session, ride, direction, -48, -48, height, stationColour);
            break;
        case 12:
            PaintGenericRotatingStructure(session, ride, direction, -48, 48, height, stationColour);
            break;

        case 7:
            PaintGenericRotatingStructure(session, ride, direction, 16, -48, height, stationColour);
            break;
        case 11:
            PaintGenericRotatingStructure(session, ride, direction, -16, -48, height, stationColour);
            break;
        case 14:
            PaintGenericRotatingStructure(session, ride, direction, -48, -16, height, stationColour);
            break;
        case 13:
            PaintGenericRotatingStructure(session, ride, direction, -48, 16, height, stationColour);
            break;
    }

    int32_t cornerSegments = 0;
    switch (trackSequence)
    {
        case 0:
            cornerSegments = EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight);
            break;
        case 3:
            cornerSegments = EnumsToFlags(PaintSegment::topRight, PaintSegment::right, PaintSegment::bottomRight);
            break;
        case 12:
            cornerSegments = EnumsToFlags(PaintSegment::topLeft, PaintSegment::left, PaintSegment::bottomLeft);
            break;
        case 15:
            cornerSegments = EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::bottom, PaintSegment::bottomRight);
            break;
    }
    PaintUtilSetSegmentSupportHeight(session, cornerSegments, height + 2, 0x20);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll & ~cornerSegments, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 160);
}

static void PaintFlatRideGeneric5x5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kTrackMap5x5[direction][trackSequence];

    const uint8_t edges = kEdges5x5[trackSequence];

    auto stationColour = GetStationColourScheme(session, trackElement);
    WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::truss, WoodenSupportSubType::neSw, direction, height, stationColour);

    const StationObject* stationObject = ride.getStationObject();
    TrackPaintUtilPaintFloor(session, edges, stationColour, height, kFloorSpritesMulch, stationObject);

    TrackPaintUtilPaintFences(
        session, edges, session.MapPosition, trackElement, ride, stationColour, height, kFenceSpritesRope,
        session.CurrentRotation);

    // All 25 tiles draw redundantly to the ride centre at (64,64) in tile-local coords.
    // Offsets = (64 - tileX, 64 - tileY) where tileX/Y = (row*32, col*32).
    switch (trackSequence)
    {
        case  0: PaintGenericRotatingStructure(session, ride, direction,  64,  64, height, stationColour); break;
        case  1: PaintGenericRotatingStructure(session, ride, direction,  64,  32, height, stationColour); break;
        case  2: PaintGenericRotatingStructure(session, ride, direction,  64,   0, height, stationColour); break;
        case  3: PaintGenericRotatingStructure(session, ride, direction,  64, -32, height, stationColour); break;
        case  4: PaintGenericRotatingStructure(session, ride, direction,  64, -64, height, stationColour); break;
        case  5: PaintGenericRotatingStructure(session, ride, direction,  32,  64, height, stationColour); break;
        case  6: PaintGenericRotatingStructure(session, ride, direction,  32,  32, height, stationColour); break;
        case  7: PaintGenericRotatingStructure(session, ride, direction,  32,   0, height, stationColour); break;
        case  8: PaintGenericRotatingStructure(session, ride, direction,  32, -32, height, stationColour); break;
        case  9: PaintGenericRotatingStructure(session, ride, direction,  32, -64, height, stationColour); break;
        case 10: PaintGenericRotatingStructure(session, ride, direction,   0,  64, height, stationColour); break;
        case 11: PaintGenericRotatingStructure(session, ride, direction,   0,  32, height, stationColour); break;
        case 12: PaintGenericRotatingStructure(session, ride, direction,   0,   0, height, stationColour); break;
        case 13: PaintGenericRotatingStructure(session, ride, direction,   0, -32, height, stationColour); break;
        case 14: PaintGenericRotatingStructure(session, ride, direction,   0, -64, height, stationColour); break;
        case 15: PaintGenericRotatingStructure(session, ride, direction, -32,  64, height, stationColour); break;
        case 16: PaintGenericRotatingStructure(session, ride, direction, -32,  32, height, stationColour); break;
        case 17: PaintGenericRotatingStructure(session, ride, direction, -32,   0, height, stationColour); break;
        case 18: PaintGenericRotatingStructure(session, ride, direction, -32, -32, height, stationColour); break;
        case 19: PaintGenericRotatingStructure(session, ride, direction, -32, -64, height, stationColour); break;
        case 20: PaintGenericRotatingStructure(session, ride, direction, -64,  64, height, stationColour); break;
        case 21: PaintGenericRotatingStructure(session, ride, direction, -64,  32, height, stationColour); break;
        case 22: PaintGenericRotatingStructure(session, ride, direction, -64,   0, height, stationColour); break;
        case 23: PaintGenericRotatingStructure(session, ride, direction, -64, -32, height, stationColour); break;
        case 24: PaintGenericRotatingStructure(session, ride, direction, -64, -64, height, stationColour); break;
    }

    int32_t cornerSegments = 0;
    switch (trackSequence)
    {
        case  0: cornerSegments = EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight);       break;
        case  4: cornerSegments = EnumsToFlags(PaintSegment::topRight, PaintSegment::right, PaintSegment::bottomRight); break;
        case 20: cornerSegments = EnumsToFlags(PaintSegment::topLeft, PaintSegment::left, PaintSegment::bottomLeft);    break;
        case 24: cornerSegments = EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::bottom, PaintSegment::bottomRight); break;
    }
    PaintUtilSetSegmentSupportHeight(session, cornerSegments, height + 2, 0x20);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll & ~cornerSegments, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 160);
}

// Rotates a tile-local (x,y) offset by direction*90°. This is the rotation rule implicitly
// encoded by kTrackMap5x5 + the fixed offsets in PaintFlatRideGeneric5x5's switch
// (verified against all 4 direction rows: storedSeq1's -clearance (64,64) rotates to
// (64,64)/case0, (64,-64)/case4, (-64,-64)/case24, (-64,64)/case20 for dir 0-3).
static constexpr std::pair<int8_t, int8_t> RotateOffset90(int8_t x, int8_t y, uint8_t direction)
{
    switch (direction & 3)
    {
        case 0:  return { x, y };
        case 1:  return { y, static_cast<int8_t>(-x) };
        case 2:  return { static_cast<int8_t>(-x), static_cast<int8_t>(-y) };
        default: return { static_cast<int8_t>(-y), x };
    }
}

// Cyclically rotates edge flags by `direction` steps. EDGE_NE(1)/EDGE_SE(2)/EDGE_SW(4)/EDGE_NW(8)
// are already in rotational order, so this is a 4-bit cyclic rotate. Verified against
// kEdges5x5: RotateEdges(EDGE_NE|EDGE_NW, dir) reproduces case0/4/24/20 for dir 0-3.
static constexpr uint8_t RotateEdges(uint8_t edges, uint8_t direction)
{
    direction &= 3;
    return static_cast<uint8_t>(((edges << direction) | (edges >> (4 - direction))) & 0xF);
}

/**
 * Footprint variant for any WxH flat-ride-generic shape (square or not) that isn't one of
 * the two oldest, lookup-table-based sizes (4x4/5x5 above, kept untouched - they predate
 * this approach and other code may still depend on their exact kTrackMap4x4/5x5 behaviour).
 *
 * Generalizes 6x6/7x7/8x8's own analytical approach (see their prior history: each tile's
 * draw offset/edges/corner role computed from its own `clearance`, read from the TED at
 * runtime, plus `direction` - no per-direction lookup table needed) by deriving the centre
 * and corner bounds from the descriptor's OWN actual clearance range instead of hardcoded
 * per-shape literals. This works regardless of which origin convention a given TED uses -
 * cursor-at-one-corner (clearance always >= 0, e.g. 4x4/2x2/1x4A's tables) or
 * cursor-near-the-geometric-centre (clearance spans negative and positive, e.g. 6x6/8x8's
 * own tables) - because the centre is computed as (xMin+xMax)/2 from whatever the real
 * extremes are, not assumed. Verified to reproduce 6x6/7x7/8x8's exact prior behaviour
 * (their hardcoded bounds, e.g. 112 for 8x8, fall out of this formula automatically) and
 * 4x4's real per-tile offsets (e.g. clearance (0,0) -> centre-relative offset (48,48)).
 */
static void PaintFlatRideGenericAnySize(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    const auto& ted = OpenRCT2::TrackMetadata::GetTrackElementDescriptor(trackElement.GetTrackType());
    const auto& clearance = ted.sequenceData.sequences[trackSequence].clearance;

    int16_t xMin = clearance.x;
    int16_t xMax = clearance.x;
    int16_t yMin = clearance.y;
    int16_t yMax = clearance.y;
    for (uint8_t i = 0; i < ted.sequenceData.numSequences; i++)
    {
        const auto& c = ted.sequenceData.sequences[i].clearance;
        xMin = std::min(xMin, c.x);
        xMax = std::max(xMax, c.x);
        yMin = std::min(yMin, c.y);
        yMax = std::max(yMax, c.y);
    }

    uint8_t localEdges = 0;
    if (clearance.x == xMin)
        localEdges |= EDGE_NE;
    if (clearance.x == xMax)
        localEdges |= EDGE_SW;
    if (clearance.y == yMin)
        localEdges |= EDGE_NW;
    if (clearance.y == yMax)
        localEdges |= EDGE_SE;
    const uint8_t edges = RotateEdges(localEdges, direction);

    auto stationColour = GetStationColourScheme(session, trackElement);
    WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::truss, WoodenSupportSubType::neSw, direction, height, stationColour);

    const StationObject* stationObject = ride.getStationObject();
    TrackPaintUtilPaintFloor(session, edges, stationColour, height, kFloorSpritesMulch, stationObject);

    TrackPaintUtilPaintFences(
        session, edges, session.MapPosition, trackElement, ride, stationColour, height, kFenceSpritesRope,
        session.CurrentRotation);

    const int16_t centreX = static_cast<int16_t>((xMin + xMax) / 2);
    const int16_t centreY = static_cast<int16_t>((yMin + yMax) / 2);
    const auto [ox, oy] = RotateOffset90(
        static_cast<int8_t>(centreX - clearance.x), static_cast<int8_t>(centreY - clearance.y), direction);
    PaintGenericRotatingStructure(session, ride, direction, ox, oy, height, stationColour);

    int32_t cornerSegments = 0;
    const bool isCorner = (clearance.x == xMin || clearance.x == xMax) && (clearance.y == yMin || clearance.y == yMax);
    if (isCorner)
    {
        // Identify which of the 4 *pre-rotation* corners this tile is, then rotate that
        // corner's own canonical screen-quadrant role by `direction` - avoids needing a
        // per-shape bounds table (the prior kFlatTrackNxNBounds approach only worked
        // because those are square, where rotation just swaps two equal-magnitude axes).
        const auto maxOx = static_cast<int8_t>(centreX - xMin);
        const auto maxOy = static_cast<int8_t>(centreY - yMin);
        const auto [topX, topY] = RotateOffset90(maxOx, maxOy, direction);
        const auto [rightX, rightY] = RotateOffset90(maxOx, static_cast<int8_t>(-maxOy), direction);
        const auto [leftX, leftY] = RotateOffset90(static_cast<int8_t>(-maxOx), maxOy, direction);
        if (ox == topX && oy == topY)
            cornerSegments = EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight);
        else if (ox == rightX && oy == rightY)
            cornerSegments = EnumsToFlags(PaintSegment::topRight, PaintSegment::right, PaintSegment::bottomRight);
        else if (ox == leftX && oy == leftY)
            cornerSegments = EnumsToFlags(PaintSegment::topLeft, PaintSegment::left, PaintSegment::bottomLeft);
        else
            cornerSegments = EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::bottom, PaintSegment::bottomRight);
    }
    PaintUtilSetSegmentSupportHeight(session, cornerSegments, height + 2, 0x20);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll & ~cornerSegments, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 160);
}

TrackPaintFunction GetTrackPaintFunctionFlatRideGeneric(TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::flatTrack4x4: return PaintFlatRideGeneric;
        case TrackElemType::flatTrack5x5: return PaintFlatRideGeneric5x5;

        // Every other footprint shape - existing (previously unwired into this dispatcher
        // at all, or wired to the now-removed per-size 6x6/7x7/8x8 functions) and new -
        // shares the one analytical implementation above.
        case TrackElemType::flatTrack1x1A:
        case TrackElemType::flatTrack2x2:
        case TrackElemType::flatTrack3x3:
        case TrackElemType::flatTrack1x4A:
        case TrackElemType::flatTrack1x5:
        case TrackElemType::flatTrack2x4:
        case TrackElemType::flatTrack6x6:
        case TrackElemType::flatTrack7x7:
        case TrackElemType::flatTrack8x8:
        case TrackElemType::customFootprint1x2:
        case TrackElemType::customFootprint1x3:
        case TrackElemType::customFootprint1x6:
        case TrackElemType::customFootprint1x7:
        case TrackElemType::customFootprint1x8:
        case TrackElemType::customFootprint2x3:
        case TrackElemType::customFootprint2x5:
        case TrackElemType::customFootprint2x6:
        case TrackElemType::customFootprint2x7:
        case TrackElemType::customFootprint2x8:
        case TrackElemType::customFootprint3x4:
        case TrackElemType::customFootprint3x5:
        case TrackElemType::customFootprint3x6:
        case TrackElemType::customFootprint3x7:
        case TrackElemType::customFootprint3x8:
        case TrackElemType::customFootprint4x5:
        case TrackElemType::customFootprint4x6:
        case TrackElemType::customFootprint4x7:
        case TrackElemType::customFootprint4x8:
        case TrackElemType::customFootprint5x6:
        case TrackElemType::customFootprint5x7:
        case TrackElemType::customFootprint5x8:
        case TrackElemType::customFootprint6x7:
        case TrackElemType::customFootprint6x8:
        case TrackElemType::customFootprint7x8:
            return PaintFlatRideGenericAnySize;

        default: return TrackPaintFunctionDummy;
    }
}
