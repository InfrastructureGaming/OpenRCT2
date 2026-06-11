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
    .StartTrackPiece = TrackElemType::flatTrack5x5,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::genericRotatingFlatRide,
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
    .OperatingSettings = { 7, 15 },
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
        .RiderFrameStride = 7,  // 7 cars, 2 seats each (14 total)
        // 5x5 footprint: entity is at origin tile, sprite centre is world (+64,+64) away.
        // That maps to ±128px horizontal or ±64px vertical offset depending on camera rotation.
        // Add half the 166x116 sprite size on top of those offsets.
        .InvalidationHalfWidth   = 220,  // >= 128 + 83 (half sprite width)
        .InvalidationHeightAbove = 140,  // >= 64 + 58 (half sprite height)
        .InvalidationHeightBelow = 140,
    },
};
} // namespace OpenRCT2
// clang-format on
