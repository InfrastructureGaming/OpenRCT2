/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../../SpriteIds.h"
#include "../../RideData.h"
#include "../../ShopItem.h"

// clang-format off
namespace OpenRCT2
{
constexpr RideTypeDescriptor TiltAWhirlRTD =
{
    .Category = RideCategory::thrill,
    .StartTrackPiece = TrackElemType::flatTrack6x6,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::flatRideGeneric,
        .enabledTrackGroups = {},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .flags = RtdFlags(RtdFlag::hasSinglePieceStation,
                     RtdFlag::cannotHaveGaps, RtdFlag::hasLoadOptions, RtdFlag::vehicleIsIntegral,
                     RtdFlag::noWallsAroundTrack, RtdFlag::isFlatRide, RtdFlag::hasVehicleColours,
                     RtdFlag::allowMusic, RtdFlag::hasEntranceAndExit, RtdFlag::singleSession,
                     RtdFlag::interestingToLookAt),
    .RideModes = EnumsToFlags(RideMode::rotation),
    .DefaultMode = RideMode::rotation,
    .OperatingSettings = { 3, 15 },
    .Naming = { STR_RIDE_NAME_TILT_A_WHIRL, STR_RIDE_DESCRIPTION_TILT_A_WHIRL },
    .NameConvention = { RideComponentType::Car, RideComponentType::Structure, RideComponentType::Station },
    .availableBreakdowns = { Breakdown::safetyCutOut },
    .Heights = { 15, 176, 7, 11, },
    .MaxMass = 255,
    .LiftData = { Audio::SoundId::null, 5, 5 },
    .RatingsMultipliers = { 50, 30, 10 },
    .UpkeepCosts = { 50, 1, 0, 0, 0, 0 },
    .BuildCosts = { 99.00_GBP, 1.00_GBP, 1, },
    .DefaultPrices = { 15, 0 },
    .DefaultMusic = kMusicObjectEgyptian,
    .PhotoItem = ShopItem::photo,
    .BonusValue = 35,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { Drawing::Colour::darkPurple, Drawing::Colour::white, Drawing::Colour::black },
        { Drawing::Colour::yellow, Drawing::Colour::white, Drawing::Colour::black },
        { Drawing::Colour::brightRed, Drawing::Colour::yellow, Drawing::Colour::black },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_MAGIC_CARPET_TRACK, SPR_RIDE_DESIGN_PREVIEW_MAGIC_CARPET_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "tilt_a_whirl",
    .RatingsData =
    {
        RatingsCalculationType::FlatRide,
        { RideRating::make(2, 45), RideRating::make(1, 60), RideRating::make(2, 60) },
        16,
        0,
        false,
        {
            { RatingsModifierType::BonusOperationOption, 0, 10, 20, 20 },
            { RatingsModifierType::BonusScenery,         0, 11155, 0, 0 },
        },
    },
    .FlatRideRotation =
    {
        .FramesPerDir = 128,
        // All 7 cars re-rendered at the new 243x170 size (see project_flat_ride memory).
        .RiderFrameStride = 7,
        // 6x6 footprint: cursor/trackOrigin tile is grid (2,2), one tile-width off from the
        // true geometric center. PaintFlatRideGeneric6x6 applies a +16,+16 world-unit
        // shift (rotated per direction) to draw the sprite centered on the grid instead of on
        // the cursor tile — a small (<=16 unit) offset from the entity's exact position.
        // Re-rendered Core sprite is 243x170 (spriteWidth=122, spriteHeight=85), with
        // per-direction sprite anchors calibrated in generate_rider_manifest.ps1.
        // Invalidation values scaled up proportionally from the prior 200x140/100x70
        // sprite (220/140/140 -> ~270/170/170), but InvalidationHalfWidth is uint8_t
        // (max 255), so it's capped at 255 instead of the proportional 270.
        .InvalidationHalfWidth   = 255,
        .InvalidationHeightAbove = 170,
        .InvalidationHeightBelow = 170,
        // Regression replication of the legacy Start/Loop/End MerryGoRound walk via
        // UpdateFlatRideGeneric (see VehicleData.cpp kTiltAWhirlPrograms) - validates the
        // generalized multi-phase/multi-program path before any new ride adopts it.
        .Programs = kTiltAWhirlPrograms,
        .NumPrograms = 1,
    },
};
} // namespace OpenRCT2
// clang-format on
