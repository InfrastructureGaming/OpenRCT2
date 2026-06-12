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

#include <utility>

using namespace OpenRCT2;

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
    const auto& desc = GetRideTypeDescriptor(ride.type).FlatRideRotation;
    if (desc.FramesPerDir == 0)
        return;

    const auto* rideEntry = GetRideEntryByIndex(ride.subtype);
    if (rideEntry == nullptr)
        return;

    const uint32_t baseImageId = rideEntry->Cars[0].base_image_id;
    if (baseImageId == 0)
        return;

    uint8_t animFrame = 0;
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
    CoordsXYZ offset(xOffset, yOffset, height + 7);
    BoundBoxXYZ bb = { { 0, 0, height + 7 }, { 24, 24, 48 } };

    // Primary remap (indices 243-254) is excluded from GetClosestPaletteIndex, so Blender
    // renders via -m closest can never produce those pixels. Route Body→secondary remap
    // (hot pink range 202-213) and Trim→tertiary remap (yellow range 46-57) instead.
    auto imageTemplate = ImageId(0, ride.vehicleColours[0].Body, ride.vehicleColours[0].Body, ride.vehicleColours[0].Trim);
    if (stationColour != TrackStationColour)
        imageTemplate = stationColour;
    auto imageId = imageTemplate.WithIndex(baseImageId + direction * desc.FramesPerDir + animFrame);
    PaintAddImageAsParent(session, imageId, offset, bb);

    // Rider overlays — each gondola has its own full 4-direction x FramesPerDir sheet,
    // immediately following the main structure block and (for gondola g) every prior
    // gondola's block: base + (g+1) * 4*FramesPerDir. A shared/phase-shifted sheet
    // (Twist/Enterprise-style) isn't usable here because every gondola follows a unique
    // hand-keyframed path rather than a copy of one curve at different offsets.
    // Each sheet shows both of that gondola's seats; seat 2g is recoloured via secondary
    // remap and seat 2g+1 via tertiary remap (primary remap is unreachable from the
    // -m closest Blender pipeline, so it's left as an unused placeholder).
    // Only drawn at zoom 0 (identical to Enterprise/Twist rider draw behavior).
    if (vehicle != nullptr && desc.RiderFrameStride > 0 && vehicle->num_peeps > 0
        && session.rt.zoom_level <= ZoomLevel{ 0 })
    {
        const uint32_t structureBlockSize = 4 * desc.FramesPerDir;
        const uint8_t  numGondolas = desc.RiderFrameStride;
        for (uint8_t g = 0; g < numGondolas; g++)
        {
            const uint8_t seatA = g * 2;
            const uint8_t seatB = seatA + 1;
            if (seatA >= vehicle->num_peeps)
                break;
            const uint32_t riderIdx = baseImageId
                + static_cast<uint32_t>(g + 1) * structureBlockSize
                + direction * desc.FramesPerDir
                + animFrame;
            ImageId riderId;
            if (stationColour != TrackStationColour)
                riderId = stationColour.WithIndex(riderIdx);
            else
                riderId = ImageId(0, Drawing::Colour::black, vehicle->peep_tshirt_colours[seatA],
                                   vehicle->peep_tshirt_colours[seatB])
                              .WithIndex(riderIdx);
            PaintAddImageAsChild(session, riderId, offset, bb);
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
static void PaintGenericRotatingFlatRide(
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

static void PaintGenericRotatingFlatRide5x5(
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
// encoded by kTrackMap5x5 + the fixed offsets in PaintGenericRotatingFlatRide5x5's switch
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

// {xMin, xMax, yMin, yMax} of the rotated 6x6 offset range. The draw offset is centered
// on the true grid center (see the +16,+16 shift in PaintGenericRotatingFlatRide6x6),
// which makes this range symmetric about zero and identical for every camera direction.
// Used to identify which screen-corner a corner tile's rotated offset falls on.
constexpr int8_t kFlatTrack6x6Bounds[4] = { -80, 80, -80, 80 };

/**
 * 6x6 footprint variant. Unlike 5x5 (an odd grid whose center tile doubles as the cursor
 * tile and maps onto itself under 90° rotation, enabling fixed kTrackMap5x5/kEdges5x5
 * lookup tables), 6x6 is an even grid with no true center tile — the cursor tile (idx 0,
 * clearance (0,0)) is one tile-width off from the geometric center, and the grid does not
 * map onto itself under rotation around it. Instead of per-direction lookup tables, each
 * tile's draw offset/edges/corner role are computed analytically from its own `clearance`
 * (read from the TED at runtime) and the current `direction`.
 */
static void PaintGenericRotatingFlatRide6x6(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    const auto& ted = OpenRCT2::TrackMetadata::GetTrackElementDescriptor(TrackElemType::flatTrack6x6);
    const auto& clearance = ted.sequenceData.sequences[trackSequence].clearance;

    uint8_t localEdges = 0;
    if (clearance.x == -64)
        localEdges |= EDGE_NE;
    if (clearance.x == 96)
        localEdges |= EDGE_SW;
    if (clearance.y == -64)
        localEdges |= EDGE_NW;
    if (clearance.y == 96)
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

    // Shift the draw target from the cursor/trackOrigin tile (grid 2,2) to the true
    // geometric center of the 6x6 grid (grid 2.5,2.5) — +16 world units in both axes —
    // before rotating, so the structure renders centered on the 6x6 plot in every
    // camera direction instead of one tile high/left of center.
    const auto [ox, oy] = RotateOffset90(
        static_cast<int8_t>(-clearance.x + 16), static_cast<int8_t>(-clearance.y + 16), direction);
    PaintGenericRotatingStructure(session, ride, direction, ox, oy, height, stationColour);

    int32_t cornerSegments = 0;
    const bool isCorner = (clearance.x == -64 || clearance.x == 96) && (clearance.y == -64 || clearance.y == 96);
    if (isCorner)
    {
        const auto& b = kFlatTrack6x6Bounds;
        if (ox == b[1] && oy == b[3])
            cornerSegments = EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight);
        else if (ox == b[1] && oy == b[2])
            cornerSegments = EnumsToFlags(PaintSegment::topRight, PaintSegment::right, PaintSegment::bottomRight);
        else if (ox == b[0] && oy == b[3])
            cornerSegments = EnumsToFlags(PaintSegment::topLeft, PaintSegment::left, PaintSegment::bottomLeft);
        else
            cornerSegments = EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::bottom, PaintSegment::bottomRight);
    }
    PaintUtilSetSegmentSupportHeight(session, cornerSegments, height + 2, 0x20);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll & ~cornerSegments, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 160);
}

TrackPaintFunction GetTrackPaintFunctionGenericFlatRide(TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::flatTrack4x4: return PaintGenericRotatingFlatRide;
        case TrackElemType::flatTrack5x5: return PaintGenericRotatingFlatRide5x5;
        case TrackElemType::flatTrack6x6: return PaintGenericRotatingFlatRide6x6;
        default:                          return TrackPaintFunctionDummy;
    }
}
