/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "CustomRideLoader.h"

#include "../Diagnostic.h"
#include "../PlatformEnvironment.h"
#include "../core/FileSystem.hpp"
#include "../core/Json.hpp"
#include "../localisation/Language.h"
#include "../localisation/StringIdType.h"
#include "Ride.h"
#include "RideData.h"
#include "RideTypeRegistry.h"
#include "rtd/thrill/FlatRideGeneric.h"

namespace OpenRCT2::CustomRideLoader
{
    static RideTypeDescriptor BuildDescriptorFromManifest(const json_t& manifest)
    {
        // Start from the flat ride generic defaults so all required fields are populated.
        RideTypeDescriptor rtd = FlatRideGenericRTD;

        // Allocate dynamic string IDs for name and description.
        std::string nameStr = manifest.value("name", "Custom Ride");
        std::string descStr = manifest.value("description", "");

        rtd.Naming.Name = LanguageAllocateObjectString(nameStr);
        rtd.Naming.Description = descStr.empty() ? kStringIdNone : LanguageAllocateObjectString(descStr);

        // Store the json id string so RideTypeRegistry can use it for dedup.
        // Note: Name field (std::string_view) can't hold this; it's stored via the registry's id map.
        rtd.Name = "";

        // Set backing ride type so the New Ride window knows which object provides the vehicle entry.
        rtd.BackingRideType = RIDE_TYPE_FLAT_RIDE_GENERIC;

        std::string authorStr = manifest.value("author", "");
        if (!authorStr.empty())
            rtd.CustomAuthor = LanguageAllocateObjectString(authorStr);

        int64_t cost = manifest.value("cost", int64_t(0));
        if (cost > 0)
            rtd.CustomBuildCost = cost * 10; // manifest is whole-pound; money64 uses 1 decimal place

        // Optional ratings overrides from manifest (whole number, 0-9).
        if (manifest.contains("ratings"))
        {
            const auto& ratings = manifest.at("ratings");
            int16_t ex = ratings.value("excitement", 3);
            int16_t in = ratings.value("intensity",  2);
            int16_t na = ratings.value("nausea",     1);

            rtd.RatingsData.BaseRatings = {
                RideRating::make(ex, 0),
                RideRating::make(in, 0),
                RideRating::make(na, 50),
            };
        }

        return rtd;
    }

    static void TryLoadManifest(const fs::path& manifestPath, const std::string& rideId)
    {
        json_t manifest;
        try
        {
            manifest = Json::ReadFromFile(manifestPath.u8string());
        }
        catch (const std::exception& ex)
        {
            LOG_WARNING("Custom ride '%s': failed to parse manifest.json — %s", rideId.c_str(), ex.what());
            return;
        }

        // Validate required fields.
        if (!manifest.contains("name") || !manifest["name"].is_string())
        {
            LOG_WARNING("Custom ride '%s': manifest.json missing required 'name' field", rideId.c_str());
            return;
        }

        auto rtd = BuildDescriptorFromManifest(manifest);
        GetRideTypeRegistry().Register(rideId, std::move(rtd));
        LOG_VERBOSE("Custom ride loaded: %s", rideId.c_str());
    }

    void LoadAll(const IPlatformEnvironment& env)
    {
        auto customRidesPath = fs::u8path(env.GetDirectoryPath(DirBase::user, DirId::customRides));
        if (!fs::exists(customRidesPath) || !fs::is_directory(customRidesPath))
            return;

        for (const auto& entry : fs::directory_iterator(customRidesPath))
        {
            if (!entry.is_directory())
                continue;

            auto manifestPath = entry.path() / "manifest.json";
            if (!fs::exists(manifestPath))
                continue;

            std::string rideId = entry.path().filename().u8string();
            TryLoadManifest(manifestPath, rideId);
        }

        auto count = GetRideTypeRegistry().Count() - RIDE_TYPE_COUNT;
        if (count > 0)
            LOG_INFO("Loaded %u custom ride(s) from '%s'", count, customRidesPath.u8string().c_str());
    }

} // namespace OpenRCT2::CustomRideLoader
