/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/FlagHolder.hpp"
#include "../core/Money.hpp"
#include "../localisation/StringIdType.h"
#include "ObjectTypes.h"

enum class CursorID : uint8_t;

namespace OpenRCT2
{
    enum class SmallSceneryFlag : uint8_t
    {
        occupiesFullTile,
        vOffsetCentre,
        requiresFlatSurface,
        // User can choose rotation when set, otherwise rotation is automatic
        isRotatable,
        isAnimated,
        canWither,
        canBeWatered,
        hasOverlayImage,
        isDiagonal,
        hasGlass,
        hasPrimaryColour,
        isFountain,
        isCupidFountain,
        isClock,
        isSwampGoo,
        hasFrameOffsets,
        flag17,
        // Means item can be placed in the air and over water
        isStackable,
        prohibitWalls,
        hasSecondaryColour,
        hasNoSupports,
        isVisibleWhenZoomed,
        isCogwheel,
        // Means supports can be built on this object. Used for base blocks.
        allowSupportsAbove,
        occupiesHalfTile,
        occupiesThreeQuarters,
        // Used for scenery items which are support structures
        supportsHavePrimaryColour,
        flag27,

        // Added by OpenRCT2:
        isTree,
        hasTertiaryColour,
    };
    using SmallSceneryFlags = FlagHolder<uint32_t, SmallSceneryFlag>;

    // Reactive scenery (OpenRCT2 extension): animated scenery whose current frame is driven by a query
    // of nearby world state each tick, rather than a free-running timer. The first (and initially only)
    // trigger is a railway crossing gate that reads whether the level-crossing path it faces is blocked
    // by an approaching vehicle. Adding a trigger = one enum value + one case in EvaluateReactiveTrigger
    // (MapAnimation.cpp) + one JSON "type" string (SmallSceneryObject.cpp). See the reactive-scenery plan.
    enum class ReactiveTriggerType : uint8_t
    {
        none,     // ordinary scenery (free-running animation or static) — the default
        crossing, // gate: engaged while the faced level-crossing path is IsBlockedByVehicle()
    };

    // A reactive element reuses the tile element's `age` byte as a frame CURSOR over one authored
    // open->closed sequence (frame_offsets[0] = fully open .. FrameOffsetCount-1). While engaged the
    // cursor sweeps in to `holdLoopStart` (lower), then LOOPS [holdLoopStart, last] (the hold, e.g.
    // flashing lights over a lowered arm); while clear it sweeps back to 0 (raise). Because the loop
    // keeps the frame changing, the gate animates continuously through lower/hold/raise. The step cadence
    // is set independently for the two phases (see sweepTicks/flashTicks) so, e.g., a gate can open/close
    // quickly while its lights flash at their own rate. holdLoopStart == 0 (or out of range) means no hold
    // loop -> a static hold at the last frame.
    struct ReactiveTrigger
    {
        ReactiveTriggerType type = ReactiveTriggerType::none;
        uint8_t holdLoopStart = 0; // first frame of the looping "hold" sub-range (0 = static hold)
        // Game ticks per frame in each phase (the map-animation update runs on even ticks, so these are
        // effectively rounded down to an even value with a floor of 2 = fastest). Independent knobs:
        uint8_t sweepTicks = 4; // lower & raise cadence (the gate's open/close speed)
        uint8_t flashTicks = 4; // hold-loop cadence (the flashing rate while a train is present)
    };

    struct SmallSceneryEntry
    {
        static constexpr auto kObjectType = ObjectType::smallScenery;

        StringId name;
        uint32_t image;
        SmallSceneryFlags flags;
        uint8_t height;
        CursorID tool_id;
        money64 price;
        money64 removal_price;
        uint8_t* frame_offsets;
        uint16_t FrameOffsetCount;
        uint16_t animation_delay;
        uint16_t animation_mask;
        uint16_t num_frames;
        ObjectEntryIndex scenery_tab_id;
        ReactiveTrigger reactive; // type==none for every ordinary/legacy scenery object
    };
} // namespace OpenRCT2
