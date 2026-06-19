/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "RideData.h"

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace OpenRCT2
{
    /**
     * Dynamic ride type registry.
     *
     * Built-in types (indices 0..RIDE_TYPE_COUNT-1) are backed by the static
     * kRideTypeDescriptors array and are always present. Custom ride types
     * loaded at runtime from the custom_rides folder receive indices
     * >= RIDE_TYPE_COUNT and are stored in the dynamic extension.
     *
     * All code that previously compared against RIDE_TYPE_COUNT as a runtime
     * bound should call GetRideTypeCount() instead so that custom types are
     * included in the valid range.
     */
    class RideTypeRegistry
    {
    public:
        /**
         * Register a custom ride type; returns its runtime index.
         * stringId must be globally unique (e.g. "mymod.cool_ride").
         * Asserts on duplicate registration.
         */
        uint32_t Register(std::string_view stringId, RideTypeDescriptor descriptor);

        /** Returns the descriptor for the given index; kDummyRTD for out-of-range. */
        const RideTypeDescriptor& Get(uint32_t index) const;

        /** Resolve a string ID to its runtime index, if registered. */
        std::optional<uint32_t> FindByStringId(std::string_view stringId) const;

        /** Total registered type count: RIDE_TYPE_COUNT built-ins + any custom. */
        uint32_t Count() const;

        bool IsValid(uint32_t index) const;
        bool IsCustom(uint32_t index) const;

    private:
        std::vector<RideTypeDescriptor> _custom;
        std::unordered_map<std::string, uint32_t> _idToIndex;
    };

    /** Returns the global ride type registry singleton. */
    RideTypeRegistry& GetRideTypeRegistry();

} // namespace OpenRCT2
