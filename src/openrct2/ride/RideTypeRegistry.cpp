/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideTypeRegistry.h"

#include "../core/Guard.hpp"

namespace OpenRCT2
{
    uint32_t RideTypeRegistry::Register(std::string_view stringId, RideTypeDescriptor descriptor)
    {
        Guard::Assert(
            _idToIndex.find(std::string(stringId)) == _idToIndex.end(),
            "Duplicate ride type registration: %s", std::string(stringId).c_str());

        const uint32_t index = RIDE_TYPE_COUNT + static_cast<uint32_t>(_custom.size());
        _idToIndex.emplace(std::string(stringId), index);
        _custom.push_back(std::move(descriptor));
        return index;
    }

    const RideTypeDescriptor& RideTypeRegistry::Get(uint32_t index) const
    {
        if (index < RIDE_TYPE_COUNT)
            return kRideTypeDescriptors[index];

        const uint32_t customIndex = index - RIDE_TYPE_COUNT;
        if (customIndex < static_cast<uint32_t>(_custom.size()))
            return _custom[customIndex];

        return kDummyRTD;
    }

    std::optional<uint32_t> RideTypeRegistry::FindByStringId(std::string_view stringId) const
    {
        auto it = _idToIndex.find(std::string(stringId));
        if (it != _idToIndex.end())
            return it->second;
        return std::nullopt;
    }

    uint32_t RideTypeRegistry::Count() const
    {
        return RIDE_TYPE_COUNT + static_cast<uint32_t>(_custom.size());
    }

    bool RideTypeRegistry::IsValid(uint32_t index) const
    {
        return index < Count();
    }

    bool RideTypeRegistry::IsCustom(uint32_t index) const
    {
        return index >= RIDE_TYPE_COUNT && index < Count();
    }

    RideTypeRegistry& GetRideTypeRegistry()
    {
        static RideTypeRegistry sRegistry;
        return sRegistry;
    }

} // namespace OpenRCT2

// ---------------------------------------------------------------------------
// Free-function implementations declared in RideData.h.
// These are the codebase-wide accessors — all existing call sites continue
// to work without include changes.
// ---------------------------------------------------------------------------

const RideTypeDescriptor& GetRideTypeDescriptor(ride_type_t rideType)
{
    return OpenRCT2::GetRideTypeRegistry().Get(static_cast<uint32_t>(rideType));
}

bool RideTypeIsValid(ride_type_t rideType)
{
    return OpenRCT2::GetRideTypeRegistry().IsValid(static_cast<uint32_t>(rideType));
}

uint32_t GetRideTypeCount()
{
    return OpenRCT2::GetRideTypeRegistry().Count();
}
