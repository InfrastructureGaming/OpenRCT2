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
// Display names for kFreestylePrograms, shown in the Operating tab's "Sequence" dropdown.
// Extend in lockstep with kFreestylePrograms/NumPrograms/OperatingSettings.MaxValue when
// the Short Program and Carnival are added.
static constexpr StringId kFreestyleProgramNames[] = {
    STR_FREESTYLE_PROGRAM_LONG, // operationOption 0
};

constexpr RideTypeDescriptor FreestyleRTD =
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
    // Selects which single-pass program (see kFreestylePrograms) plays for the next ride
    // cycle: ride.operationOption % NumPrograms. Currently only the Long Program exists
    // (NumPrograms=1, so this is a no-op); widen to {0,1} when the Short Program is added.
    .OperatingSettings = { 0, 0 },
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
        { RideRating::make(5, 64), RideRating::make(6, 80), RideRating::make(2, 60) },
        16,
        0,
        false,
        {
            // operationOption selects the program (currently always 0, see
            // OperatingSettings); no-op until more programs exist and this is tuned.
            { RatingsModifierType::BonusOperationOption, 0, 10, 20, 20 },
            { RatingsModifierType::BonusScenery,         0, 11155, 0, 0 },
        },
    },
    .FlatRideRotation =
    {
        .FramesPerDir = 3600,
        // 12 gondola pairs = 24 seats, all rider sprite-sets rendered and packaged.
        .RiderFrameStride = 12,
        // Placeholders carried over from Tilt-A-Whirl; re-derive once Freestyle's sprites
        // are rendered and their on-screen extents are known.
        .InvalidationHalfWidth   = 255,
        .InvalidationHeightAbove = 170,
        .InvalidationHeightBelow = 170,
        // Long Program only (frames 0-3599, single-pass). See VehicleData.cpp
        // kFreestylePrograms; the Short Program will be appended as operationOption 1
        // once rendered.
        .Programs = kFreestylePrograms,
        .NumPrograms = 1,
        .ProgramNames = kFreestyleProgramNames,
    },
};
} // namespace OpenRCT2
// clang-format on
