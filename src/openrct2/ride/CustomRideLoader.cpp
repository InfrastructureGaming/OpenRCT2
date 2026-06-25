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
#include "TrackData.h"
#include "rtd/thrill/FlatRideGeneric.h"
#include "ted/TrackElementDescriptor.h"

#include <list>
#include <string>
#include <utility>

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

    // Maps a manifest "footprint" width/length (see AttractionEditorTool's
    // build/object_json.py custom_ride_manifest) to the matching TrackElemType - either
    // one of the pre-existing flat-ride footprint pieces, or one of the customFootprintWxH
    // pieces reserved for sizes that didn't already exist (see TrackElemType.h's own
    // comment on that block, and TED.FlatRide.h's BuildGenericFootprintDescriptor).
    // Rotation already produces the WxH<->HxW mirror for any piece, so only one canonical
    // (width <= length) orientation needs a case here. Falls back to the pre-existing
    // default footprint (6x6) for anything outside the supported 1x1..8x8 range, logging a
    // warning rather than silently mis-sizing the ride's reserved land footprint.
    static TrackElemType FootprintTrackElemType(int width, int length, const std::string& rideId)
    {
        if (width > length)
            std::swap(width, length);

        switch (width * 10 + length)
        {
            case 11: return TrackElemType::flatTrack1x1A;
            case 12: return TrackElemType::customFootprint1x2;
            case 13: return TrackElemType::customFootprint1x3;
            case 14: return TrackElemType::flatTrack1x4A;
            case 15: return TrackElemType::flatTrack1x5;
            case 16: return TrackElemType::customFootprint1x6;
            case 17: return TrackElemType::customFootprint1x7;
            case 18: return TrackElemType::customFootprint1x8;
            case 22: return TrackElemType::flatTrack2x2;
            case 23: return TrackElemType::customFootprint2x3;
            case 24: return TrackElemType::flatTrack2x4;
            case 25: return TrackElemType::customFootprint2x5;
            case 26: return TrackElemType::customFootprint2x6;
            case 27: return TrackElemType::customFootprint2x7;
            case 28: return TrackElemType::customFootprint2x8;
            case 33: return TrackElemType::flatTrack3x3;
            case 34: return TrackElemType::customFootprint3x4;
            case 35: return TrackElemType::customFootprint3x5;
            case 36: return TrackElemType::customFootprint3x6;
            case 37: return TrackElemType::customFootprint3x7;
            case 38: return TrackElemType::customFootprint3x8;
            case 44: return TrackElemType::flatTrack4x4;
            case 45: return TrackElemType::customFootprint4x5;
            case 46: return TrackElemType::customFootprint4x6;
            case 47: return TrackElemType::customFootprint4x7;
            case 48: return TrackElemType::customFootprint4x8;
            case 55: return TrackElemType::flatTrack5x5;
            case 56: return TrackElemType::customFootprint5x6;
            case 57: return TrackElemType::customFootprint5x7;
            case 58: return TrackElemType::customFootprint5x8;
            case 66: return TrackElemType::flatTrack6x6;
            case 67: return TrackElemType::customFootprint6x7;
            case 68: return TrackElemType::customFootprint6x8;
            case 77: return TrackElemType::flatTrack7x7;
            case 78: return TrackElemType::customFootprint7x8;
            case 88: return TrackElemType::flatTrack8x8;
            default:
                LOG_WARNING(
                    "Custom ride '%s': unsupported footprint %dx%d (must be 1-8 on each axis) - using 6x6",
                    rideId.c_str(), width, length);
                return TrackElemType::flatTrack6x6;
        }
    }

    static RideTypeDescriptor BuildDescriptorFromManifest(const json_t& manifest, const std::string& rideId)
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

        // Optional footprint override from manifest ("footprint": {"width", "length"}) -
        // selects which reserved-land-area TrackElemType this ride registers as (see
        // FootprintTrackElemType's own docstring). Defaults to 6x6, matching every custom
        // ride's behaviour before this field existed.
        int width = manifest.contains("footprint") ? manifest["footprint"].value("width", 6) : 6;
        int length = manifest.contains("footprint") ? manifest["footprint"].value("length", 6) : 6;
        rtd.StartTrackPiece = FootprintTrackElemType(width, length, rideId);
        const auto& footprintTed = OpenRCT2::TrackMetadata::GetTrackElementDescriptor(rtd.StartTrackPiece);

        int64_t cost = manifest.value("cost", int64_t(0));
        if (cost > 0)
        {
            rtd.CustomBuildCost = cost * 10; // manifest is whole-pound; money64 uses 1 decimal place

            // Keep placement cost in sync with the display cost.
            // TrackPlaceAction computes: placedCost = (TrackPrice * priceModifier) >> 16
            // Invert using the SELECTED footprint's own priceModifier (varies by size - see
            // BuildGenericFootprintDescriptor/TED.FlatRide.h - it is NOT a fixed constant
            // once footprint is configurable, unlike when every custom ride was flatTrack6x6).
            rtd.BuildCosts.TrackPrice = (cost * 10LL * 65536LL) / footprintTed.priceModifier;
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

        auto rtd = BuildDescriptorFromManifest(manifest, rideId);
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
