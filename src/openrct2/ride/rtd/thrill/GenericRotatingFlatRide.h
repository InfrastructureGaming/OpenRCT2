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

// Generic data-driven flat ride type. Parkobjs that set "type": "generic_rotating_flat_ride"
// and include a "flatRideAnimation" JSON block get all paint/animation parameters from that
// block rather than from a compiled per-ride RTD. This is the entry point for modders to add
// new spinning flat rides without any C++ changes.
//
// FlatRideRotation is intentionally empty here (FramesPerDir = 0 = sentinel):
// PaintGenericRotatingStructure and UpdateRotatingGeneric both call GetFlatRideDescriptor(),
// which prefers RideObjectEntry::flatRideAnimation (set at parkobj load time) over this RTD.
// The sentinel causes an early-return in the paint function if somehow a parkobj forgets to
// include the "flatRideAnimation" block, rather than drawing garbage.
constexpr RideTypeDescriptor GenericRotatingFlatRideRTD =
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
    .OperatingSettings = { 2, 10 },
    .Naming = { STR_RIDE_NAME_GENERIC_ROTATING_FLAT_RIDE, STR_RIDE_DESCRIPTION_GENERIC_ROTATING_FLAT_RIDE },
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
        { Drawing::Colour::yellow,     Drawing::Colour::white, Drawing::Colour::black },
        { Drawing::Colour::brightRed,  Drawing::Colour::yellow, Drawing::Colour::black },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_MAGIC_CARPET_TRACK, SPR_RIDE_DESIGN_PREVIEW_MAGIC_CARPET_SUPPORTS },
    .ColourKey = RideColourKey::Ride,
    .Name = "generic_rotating_flat_ride",
    .RatingsData =
    {
        RatingsCalculationType::FlatRide,
        { RideRating::make(3, 00), RideRating::make(2, 00), RideRating::make(1, 50) },
        16,
        0,
        false,
        {
            { RatingsModifierType::BonusOperationOption, 0, 10, 20, 20 },
            { RatingsModifierType::BonusScenery,         0, 11155, 0, 0 },
        },
    },
    // FlatRideRotation intentionally empty: FramesPerDir = 0 means "delegate to parkobj".
    // GetFlatRideDescriptor() in GenericFlatRide.cpp / Vehicle.Animation.cpp prefers
    // RideObjectEntry::flatRideAnimation (parsed from the parkobj JSON) over this stub.
    .FlatRideRotation = {},
};
} // namespace OpenRCT2
// clang-format on
