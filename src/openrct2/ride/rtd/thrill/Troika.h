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
constexpr RideTypeDescriptor TroikaRTD =
{
    .Category = RideCategory::thrill,
    .StartTrackPiece = TrackElemType::flatTrack8x8,
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
    .OperatingSettings = { 2, 10 },
    .Naming = { STR_RIDE_NAME_TROIKA, STR_RIDE_DESCRIPTION_TROIKA },
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
    .Name = "troika",
    .RatingsData =
    {
        RatingsCalculationType::FlatRide,
        { RideRating::make(3, 50), RideRating::make(2, 30), RideRating::make(1, 90) },
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
        .FramesPerDir = 2461,
        // Riders not yet rendered; RiderFrameStride = 0 disables gondola overlays entirely.
        // Bump to the gondola count once rider sheets are packaged.
        .RiderFrameStride = 0,
        .StructureZOffset        = 42,
        // Sprites are 440x325; all three invalidation fields are uint8_t (max 255).
        .InvalidationHalfWidth   = 255,
        .InvalidationHeightAbove = 255,
        .InvalidationHeightBelow = 255,
        .Programs = kTroikaPrograms,
        .NumPrograms = 1,
    },
};
} // namespace OpenRCT2
// clang-format on
