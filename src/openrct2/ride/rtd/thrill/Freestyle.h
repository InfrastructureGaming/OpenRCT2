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
constexpr RideTypeDescriptor FreestyleRTD =
{
    .Category = RideCategory::thrill,
    .StartTrackPiece = TrackElemType::flatTrack6x6,
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
    // Controls ride.rotations, the shared repeat budget consumed independently by each of
    // the Long Program's three RepeatUntilRotationsComplete phases (see kFreestylePhases).
    .OperatingSettings = { 3, 15 },
    .Naming = { STR_RIDE_NAME_FREESTYLE, STR_RIDE_DESCRIPTION_FREESTYLE },
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
    .Name = "freestyle",
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
        .FramesPerDir = 1024,
        .RiderFrameStride = 12,   // 12 gondola pairs = 24 seats
        // Placeholders carried over from Tilt-A-Whirl; re-derive once Freestyle's sprites
        // are rendered and their on-screen extents are known.
        .InvalidationHalfWidth   = 255,
        .InvalidationHeightAbove = 170,
        .InvalidationHeightBelow = 170,
        // The 9-node "Long Program": Restraints Close -> Spin-Up -> Ground Spin (rise,
        // repeats) -> Tilt-Up -> Tilt Spin (repeats) -> Tilt-Down -> Ground Spin (fall,
        // repeats) -> Spin-Down -> Restraints Open (final). See VehicleData.cpp
        // kFreestylePhases for the per-phase frame ranges.
        .Programs = kFreestylePrograms,
        .NumPrograms = 1,
    },
};
} // namespace OpenRCT2
// clang-format on
