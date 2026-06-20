/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "CustomRideLoader.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../PlatformEnvironment.h"
#include "../core/FileSystem.hpp"
#include "../core/Json.hpp"
#include "../localisation/Language.h"
#include "../localisation/StringIdType.h"
#include "../object/ObjectManager.h"
#include "Ride.h"
#include "RideData.h"
#include "RideTypeRegistry.h"
#include "rtd/thrill/FlatRideGeneric.h"

#include <list>
#include <string>

namespace OpenRCT2::CustomRideLoader
{
    // Persistent storage for const char* strings set on RideTypeDescriptor fields.
    // std::list never invalidates pointers on push_back, so .c_str() is stable for the process lifetime.
    static std::list<std::string> sStringPool;

    static const char* PersistString(const std::string& s)
    {
        sStringPool.push_back(s);
        return sStringPool.back().c_str();
    }
    static RideTypeDescriptor BuildDescriptorFromManifest(const json_t& manifest)
    {
        // All custom rides start from FlatRideGenericRTD. The parkobj provides all
        // paint and animation parameters via its "flatRideAnimation" JSON block,
        // keeping the ride fully self-contained with no dependency on C++ RTD data.
        RideTypeDescriptor rtd = FlatRideGenericRTD;

        // Allocate dynamic string IDs for name and description.
        std::string nameStr = manifest.value("name", "Custom Ride");
        std::string descStr = manifest.value("description", "");

        rtd.Naming.Name = LanguageAllocateObjectString(nameStr);
        rtd.Naming.Description = descStr.empty() ? kStringIdNone : LanguageAllocateObjectString(descStr);

        // Name is a string_view into static data; clear it so the registry ID map is authoritative.
        rtd.Name = "";

        // BackingRideType drives GetPreviewImage — must match what the vehicle parkobj
        // declares in its ride_type[] array. Custom parkobjs always declare flat_ride_generic.
        rtd.BackingRideType = RIDE_TYPE_FLAT_RIDE_GENERIC;

        std::string parkobjId = manifest.value("parkobj", "");
        if (!parkobjId.empty())
            rtd.CustomParkObjId = PersistString(parkobjId);

        std::string authorStr = manifest.value("author", "");
        if (!authorStr.empty())
            rtd.CustomAuthor = LanguageAllocateObjectString(authorStr);

        int64_t cost = manifest.value("cost", int64_t(0));
        if (cost > 0)
        {
            rtd.CustomBuildCost = cost * 10; // manifest is whole-pound; money64 uses 1 decimal place

            // Keep placement cost in sync with the display cost.
            // TrackPlaceAction computes: placedCost = (TrackPrice * priceModifier) >> 16
            // For flatTrack6x6: priceModifier = 1638400 = 25 * 65536.
            // Invert: TrackPrice = (cost * 10 * 65536) / 1638400
            rtd.BuildCosts.TrackPrice = (cost * 10LL * 65536LL) / 1638400LL;
        }

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
        auto customType = GetRideTypeRegistry().Register(rideId, std::move(rtd));

        // Force-load the vehicle object so it's indexed before any park opens.
        const auto& finalRtd = GetRideTypeRegistry().Get(customType);
        if (finalRtd.CustomParkObjId != nullptr)
        {
            auto& objManager = GetContext()->GetObjectManager();
            Object* obj = objManager.LoadObject(finalRtd.CustomParkObjId);
            if (obj == nullptr)
                LOG_WARNING(
                    "Custom ride '%s': parkobj '%s' not found in object repository", rideId.c_str(),
                    finalRtd.CustomParkObjId);
        }

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
