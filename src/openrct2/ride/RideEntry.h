/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../rct2/DATLimits.h"
#include "CarEntry.h"
#include "RideColour.h"
#include "RideTypes.h"
#include "ShopItem.h"
#include "VehicleColour.h"

#include <cstdint>
#include <optional>

// Set to 255 on all tracked ride entries
static uint8_t constexpr kNoFlatRideCars = 0xFF;

// Forward declaration — full definition in RideData.h.
// Allows RideObjectEntry to carry a pointer to the parsed animation descriptor
// without creating a circular include dependency (RideData.h already includes RideEntry.h).
struct FlatRideRotationDescriptor;

// Forward declaration — full definition in Ride.h. Lets RideObjectEntry carry an
// optional per-object breakdown set without including Ride.h (which forward-declares
// RideObjectEntry, so the include can't go the other way). FlagHolder only stores the
// underlying integer, so the fixed-underlying-type forward declaration is sufficient here.
enum class Breakdown : uint8_t;

struct RideNaming
{
    StringId Name;
    StringId Description;
};

struct TrackColourPresetList
{
    uint8_t count;
    TrackColour list[256];
};

struct VehicleColourPresetList
{
    uint8_t count;
    VehicleColour list[256];
};

// Constants used by the ride_type->flags property at 0x008
enum class RideEntryFlag : uint8_t
{
    tabIconIsHalfScale,
    noInversions,
    noBankedTrack,
    playDepartSound,
    inverterShipSwingMode,
    hasTwistRotationType,
    hasEnterpriseRotationType,
    disableWanderingDeprecated,
    playSplashSound,
    /**
     * Since the Water Coaster water channel pieces share the same TrackElemType as the Dinghy Slide covered track pieces,
     * this flag was originally named playSplashSoundSlide
     */
    coveredTrackIsWaterChannel,
    isACoveredRide,
    limitAirTimeBonus,
    // Both flags below were made redundant by ride groups and deprecated in favour of it
    separateRideNameDeprecated,
    separateRideDeprecated,
    cannotBreakDown,
    disableLastOperatingModeDeprecated,
    disableDoorConstructionDeprecated,
    disableFirstTwoOperatingModesDeprecated,
    disableCollisionCrashes,
    disableColourTab,
    // Must be set with Inverter Ship swing mode as well
    magicCarpetSwingMode,
    riderControlsSpeed,
    hideEmptyTrains,
    // Hide the ‘Reverse trains’ checkbox in the Ride window. Used for symmetrical spinning trains and legacy pre-reversed
    // vehicles (to avoid double-dipping on the reverse bonus).
    noReverseOption,
};
using RideEntryFlags = FlagHolder<uint32_t, RideEntryFlag>;

/**
 * Ride type structure.
 */
struct RideObjectEntry
{
    RideNaming naming;
    // The first three images are previews. They correspond to the ride_type[] array.
    uint32_t images_offset;
    RideEntryFlags flags;
    ride_type_t ride_type[OpenRCT2::RCT2::ObjectLimits::kMaxRideTypesPerRideEntry];
    uint8_t min_cars_in_train;
    uint8_t max_cars_in_train;
    uint8_t cars_per_flat_ride;
    // Number of cars that can't hold passengers
    uint8_t zero_cars;
    // The index to the vehicle type displayed in the vehicle tab.
    uint8_t TabCar;
    uint8_t DefaultCar;
    // Convert from first - fourth car to vehicle structure
    uint8_t FrontCar;
    uint8_t SecondCar;
    uint8_t RearCar;
    uint8_t ThirdCar;
    uint8_t BuildMenuPriority;
    CarEntry Cars[OpenRCT2::RCT2::ObjectLimits::kMaxCarTypesPerRideEntry];
    VehicleColourPresetList* vehicle_preset_list;
    int8_t excitement_multiplier;
    int8_t intensity_multiplier;
    int8_t nausea_multiplier;
    uint8_t maxHeight;
    ShopItem shop_item[OpenRCT2::RCT2::ObjectLimits::kMaxShopItemsPerRideEntry];
    StringId capacity;
    uint8_t Clearance;

    // Set at load time by RideObject::ReadJsonFlatRideAnimation when the parkobj contains a
    // "flatRideAnimation" JSON block. Points into RideObject-owned storage; valid for the
    // lifetime of the loaded object. nullptr for all legacy/non-generic-flat-ride entries.
    const FlatRideRotationDescriptor* flatRideAnimation = nullptr;

    // Set at load time from the parkobj's "breakdowns" property (see RideObject.cpp). When
    // present it REPLACES the ride type's availableBreakdowns for this entry's rides (see
    // Ride::getAvailableBreakdowns) - an empty set means the ride never breaks down, exactly
    // like the cannotBreakDown flag. nullopt for legacy/other objects, which keep their ride
    // type's default breakdown set unchanged.
    std::optional<FlagHolder<uint8_t, Breakdown>> breakdownOverride;

    // Set at load time from the parkobj's "bonusValue" property (see RideObject.cpp). When
    // present it REPLACES the ride type's RideTypeDescriptor::BonusValue for this entry's
    // rides when computing the park's soft guest cap (see Park.cpp's calculateSuggestedMaxGuests),
    // letting a custom ride contribute more or less to attendance. nullopt => the ride type's
    // default BonusValue, unchanged.
    std::optional<uint8_t> bonusValueOverride;

    const CarEntry* GetCar(size_t id) const
    {
        if (id < std::size(Cars))
        {
            return &Cars[id];
        }
        return nullptr;
    }

    const CarEntry* GetDefaultCar() const
    {
        return GetCar(DefaultCar);
    }

    ride_type_t GetFirstNonNullRideType() const
    {
        for (const auto& currentRideType : ride_type)
        {
            if (currentRideType != kRideTypeNull)
                return currentRideType;
        }

        return kRideTypeNull;
    }
};

RideNaming GetRideNaming(ride_type_t rideType, const RideObjectEntry* rideEntry);
