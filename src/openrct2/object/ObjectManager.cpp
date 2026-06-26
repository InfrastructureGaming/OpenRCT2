/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ObjectManager.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../ParkImporter.h"
#include "../audio/Audio.h"
#include "../drawing/Drawing.Sprite.h" // [DIAG] GfxGetG1Element for G1 content integrity check
#include "../core/Console.hpp"
#include "../core/EnumUtils.hpp"
#include "../core/JobPool.h"
#include "../localisation/StringIds.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/RideAudio.h"
#include "../ride/RideTypeRegistry.h"
#include "../ui/WindowManager.h"
#include "BannerSceneryEntry.h"
#include "LargeSceneryObject.h"
#include "Object.h"
#include "ObjectLimits.h"
#include "ObjectList.h"
#include "ObjectRepository.h"
#include "PathAdditionObject.h"
#include "RideObject.h"
#include "SceneryGroupObject.h"
#include "SmallSceneryObject.h"
#include "WallObject.h"

#include <algorithm>
#include <array>
#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

namespace OpenRCT2
{

    /**
     * Represents an object that is to be loaded or is loaded and ready
     * to be placed in an object list.
     */
    struct ObjectToLoad
    {
        const ObjectRepositoryItem* RepositoryItem{};
        Object* LoadedObject{};
        ObjectEntryIndex Index{};
    };

    class ObjectManager final : public IObjectManager
    {
    private:
        IObjectRepository& _objectRepository;

        std::array<std::vector<Object*>, EnumValue(ObjectType::count)> _loadedObjects;
        std::vector<std::vector<ObjectEntryIndex>> _rideTypeToObjectMap;

        // Re-entrancy guard for ResetTypeToRideEntryIndexMap() - see its own comment for why
        // this is needed: every single object load (any type, not just rides) ends with a call
        // back into ResetTypeToRideEntryIndexMap(), which can itself load more objects in its
        // own pre-pass, recursing back in. With 2+ custom rides registered this recurses 2+
        // levels deep, and the inner call's "build object lists" pass ends up reading
        // _loadedObjects/_rideTypeToObjectMap while an outer, still-on-the-stack invocation
        // is also mid-flight against the same member state - confirmed via a real crash
        // (read access violation, 3-deep recursive call stack through
        // RepositoryItemToObject -> ResetTypeToRideEntryIndexMap) when entering the Scenario
        // Editor with multiple custom rides registered.
        bool _isResettingTypeMap = false;

        // Used to return a safe empty vector back from GetAllRideEntries, can be removed when std::span is available
        std::vector<ObjectEntryIndex> _nullRideTypeEntries;

    public:
        explicit ObjectManager(IObjectRepository& objectRepository)
            : _objectRepository(objectRepository)
        {
            UpdateSceneryGroupIndexes();
            ResetTypeToRideEntryIndexMap();
        }

        ~ObjectManager() override
        {
            // Not UnloadAll(): the whole manager is being destroyed, so there's no point in
            // ResetTypeToRideEntryIndexMap()'s reload-missing-custom-parkobjs pre-pass resurrecting
            // anything we just unloaded - see UnloadAllForShutdown's comment in the header.
            UnloadAll(false, false);
        }

        Object* GetLoadedObject(ObjectType objectType, size_t index) override
        {
            // This is sometimes done deliberately (to avoid boilerplate), so no need to log_warn for this.
            if (index == kObjectEntryIndexNull)
            {
                return nullptr;
            }

            if (index >= static_cast<size_t>(getObjectEntryGroupCount(objectType)))
            {
#ifdef DEBUG
                if (index != kObjectEntryIndexNull)
                {
                    LOG_WARNING("Object index %u exceeds maximum for type %d.", index, objectType);
                }
#endif
                return nullptr;
            }

            const auto& list = GetObjectList(objectType);
            if (index >= list.size())
            {
                return nullptr;
            }

            return list[index];
        }

        Object* GetLoadedObject(const ObjectEntryDescriptor& entry) override
        {
            const ObjectRepositoryItem* ori = _objectRepository.FindObject(entry);
            if (ori == nullptr)
                return nullptr;

            return ori->LoadedObject.get();
        }

        ObjectEntryIndex GetLoadedObjectEntryIndex(std::string_view identifier) override
        {
            const auto* obj = GetLoadedObject(ObjectEntryDescriptor(identifier));
            if (obj != nullptr)
            {
                return GetLoadedObjectEntryIndex(obj);
            }
            return kObjectEntryIndexNull;
        }

        ObjectEntryIndex GetLoadedObjectEntryIndex(const ObjectEntryDescriptor& descriptor) override
        {
            auto obj = GetLoadedObject(descriptor);
            if (obj != nullptr)
            {
                return GetLoadedObjectEntryIndex(obj);
            }
            return kObjectEntryIndexNull;
        }

        ObjectEntryIndex GetLoadedObjectEntryIndex(const Object* object) override
        {
            ObjectEntryIndex result = kObjectEntryIndexNull;
            size_t index = GetLoadedObjectIndex(object);
            if (index != SIZE_MAX)
            {
                ObjectGetTypeEntryIndex(index, nullptr, &result);
            }
            return result;
        }

        ObjectList GetLoadedObjects() override
        {
            ObjectList objectList;
            for (auto objectType : getAllObjectTypes())
            {
                auto maxObjectsOfType = static_cast<ObjectEntryIndex>(getObjectEntryGroupCount(objectType));
                for (ObjectEntryIndex i = 0; i < maxObjectsOfType; i++)
                {
                    auto obj = GetLoadedObject(objectType, i);
                    if (obj != nullptr)
                    {
                        objectList.SetObject(i, obj->GetDescriptor());
                    }
                }
            }
            return objectList;
        }

        std::unique_ptr<Object> LoadTempObject(std::string_view id, bool loadImages) override
        {
            const ObjectRepositoryItem* ori = _objectRepository.FindObject(id);
            if (ori == nullptr)
            {
                LOG_ERROR("Object '%s' not found in repository.", std::string{ id }.c_str());
                return nullptr;
            }

            auto object = _objectRepository.LoadObject(ori, loadImages);
            return object;
        }

        Object* LoadObject(std::string_view identifier) override
        {
            const ObjectRepositoryItem* ori = _objectRepository.FindObject(identifier);
            return RepositoryItemToObject(ori);
        }

        Object* LoadObject(const RCTObjectEntry* entry) override
        {
            const ObjectRepositoryItem* ori = _objectRepository.FindObject(entry);
            return RepositoryItemToObject(ori);
        }

        Object* LoadObject(const ObjectEntryDescriptor& descriptor) override
        {
            const ObjectRepositoryItem* ori = _objectRepository.FindObject(descriptor);
            return RepositoryItemToObject(ori);
        }

        Object* LoadObject(const ObjectEntryDescriptor& descriptor, ObjectEntryIndex slot) override
        {
            const ObjectRepositoryItem* ori = _objectRepository.FindObject(descriptor);
            return RepositoryItemToObject(ori, slot);
        }

        Object* LoadRepositoryItem(const ObjectRepositoryItem& ori) override
        {
            return RepositoryItemToObject(&ori);
        }

        void LoadObjects(const ObjectList& objectList, const bool reportProgress) override
        {
            // Find all the required objects
            auto requiredObjects = GetRequiredObjects(objectList);

            // Load the required objects
            LoadObjects(requiredObjects, reportProgress);

            // Update indices.
            UpdateSceneryGroupIndexes();
            ResetTypeToRideEntryIndexMap();

            // [DIAG] Temporary: dump loaded-object image state to diff the first (bad) editor
            // landscape load against a subsequent (clean) load. Remove once root-caused.
            DumpLoadedObjectState("LoadObjects(ObjectList)");
        }

        // [DIAG] Temporary diagnostic. Renderer-independent corruption => the G1 image data itself is
        // wrong on the bad load. The most likely mechanism is image-allocator free-list corruption
        // producing OVERLAPPING [base, base+num) ranges across objects (one object's pixel pointers
        // clobber another's) or DUPLICATE identifiers (an object loaded twice). This scans EVERY
        // loaded object of EVERY type live from _loadedObjects and reports both, per load, so the
        // first (bad) editor landscape load can be compared against the next (clean) one.
        void DumpLoadedObjectState(const char* tag)
        {
            static int sLoadSeq = 0;
            const int seq = ++sLoadSeq;

            struct Range
            {
                uint32_t base;
                uint32_t end; // base + num (exclusive)
                int type;
                size_t slot;
                std::string id;
            };
            std::vector<Range> ranges;
            std::map<std::string, int> idCounts;
            size_t totalLoaded = 0;

            // [DIAG] G1 content fingerprint per object. Object slots/counts are already proven
            // identical bad-vs-clean; the only remaining suspect is the actual G1 pixel-metadata
            // at each object's [base) range being zeroed (object's images freed out from under it)
            // or clobbered (another object's bad free overwrote them). w/h/x/y are intrinsic to the
            // image, so they're base-independent and can be compared object-by-object across loads.
            struct G1Sig
            {
                int type = 0;
                size_t slot = 0;
                int16_t w = 0, h = 0, x = 0, y = 0; // g1[base] metadata for display
                int nullCount = 0;                  // sampled g1 elements with null pixel offset
                int sampled = 0;
                uint32_t fp = 0; // fingerprint over sampled metadata
            };
            std::map<std::string, G1Sig> sigs;

            for (size_t typeIdx = 0; typeIdx < _loadedObjects.size(); typeIdx++)
            {
                const auto& list = _loadedObjects[typeIdx];
                for (size_t slot = 0; slot < list.size(); slot++)
                {
                    auto* obj = list[slot];
                    if (obj == nullptr)
                        continue;
                    totalLoaded++;
                    idCounts[std::string(obj->GetIdentifier())]++;

                    const uint32_t base = static_cast<uint32_t>(obj->GetBaseImageId());
                    const uint32_t num = obj->GetNumImages();
                    // kImageIndexUndefined (0xFFFFFFFF) / zero-image objects don't own a G1 range.
                    if (num == 0 || base == 0 || base == 0xFFFFFFFFu)
                        continue;
                    ranges.push_back(Range{ base, base + num, static_cast<int>(typeIdx), slot,
                        std::string(obj->GetIdentifier()) });

                    // [DIAG] Sample this object's G1 range at first/mid/last image and fingerprint
                    // the base-independent metadata; flag any sampled element with a null pixel
                    // pointer (means its g1 entry was freed/zeroed while the object still claims it).
                    G1Sig sig;
                    sig.type = static_cast<int>(typeIdx);
                    sig.slot = slot;
                    const uint32_t samplePts[3] = { base, base + num / 2, base + num - 1 };
                    uint32_t prevPt = 0xFFFFFFFFu;
                    for (uint32_t pt : samplePts)
                    {
                        if (pt == prevPt)
                            continue;
                        prevPt = pt;
                        const auto* g1 = GfxGetG1Element(static_cast<ImageIndex>(pt));
                        sig.sampled++;
                        if (g1 == nullptr || g1->offset == nullptr)
                        {
                            sig.nullCount++;
                            continue;
                        }
                        if (pt == base)
                        {
                            sig.w = g1->width;
                            sig.h = g1->height;
                            sig.x = g1->xOffset;
                            sig.y = g1->yOffset;
                        }
                        sig.fp = sig.fp * 31u
                            + (static_cast<uint32_t>(static_cast<uint16_t>(g1->width)) << 16
                               | static_cast<uint16_t>(g1->height));
                        sig.fp = sig.fp * 31u
                            + (static_cast<uint32_t>(static_cast<uint16_t>(g1->xOffset)) << 16
                               | static_cast<uint16_t>(g1->yOffset));
                    }
                    sigs[std::string(obj->GetIdentifier())] = sig;
                }
            }

            // Overlap detection: sort by base, report any range whose start falls inside the
            // previous range. (Ranges are half-open [base, end).)
            std::sort(ranges.begin(), ranges.end(), [](const Range& a, const Range& b) { return a.base < b.base; });
            int overlaps = 0;
            for (size_t i = 1; i < ranges.size(); i++)
            {
                const auto& prev = ranges[i - 1];
                const auto& cur = ranges[i];
                if (cur.base < prev.end)
                {
                    overlaps++;
                    if (overlaps <= 60)
                    {
                        LOG_ERROR(
                            "[DIAG]   OVERLAP %u imgs: '%s'(t%d s%zu) [%u,%u) vs '%s'(t%d s%zu) [%u,%u)",
                            prev.end - cur.base, prev.id.c_str(), prev.type, prev.slot, prev.base, prev.end,
                            cur.id.c_str(), cur.type, cur.slot, cur.base, cur.end);
                    }
                }
            }

            int dupes = 0;
            for (const auto& [id, count] : idCounts)
            {
                if (count > 1)
                {
                    dupes++;
                    LOG_ERROR("[DIAG]   DUPLICATE id='%s' loaded %d times", id.c_str(), count);
                }
            }

            // [DIAG] (1) Intra-load: any loaded object whose own G1 range has been zeroed (null pixel
            // pointer) is the corruption itself - it renders nothing ("missing scenery"). (2) Cross-load:
            // an object whose base-independent G1 metadata differs from the previous load had its pixels
            // clobbered (another object's image now occupies its range) on one of the two loads.
            static std::map<std::string, G1Sig> sPrevSigs;
            int g1Zeroed = 0;
            int g1Mismatch = 0;
            for (const auto& [id, cur] : sigs)
            {
                if (cur.nullCount > 0)
                {
                    g1Zeroed++;
                    if (g1Zeroed <= 60)
                        LOG_ERROR(
                            "[DIAG]   ZEROED-G1 id='%s'(t%d s%zu) %d/%d sampled imgs have NULL pixel offset"
                            " (images freed while object still loaded -> renders blank)",
                            id.c_str(), cur.type, cur.slot, cur.nullCount, cur.sampled);
                }
                auto it = sPrevSigs.find(id);
                if (it != sPrevSigs.end() && it->second.fp != cur.fp)
                {
                    g1Mismatch++;
                    if (g1Mismatch <= 60)
                        LOG_ERROR(
                            "[DIAG]   G1-MISMATCH id='%s'(t%d s%zu) prev{w%d h%d x%d y%d} -> cur{w%d h%d x%d y%d}"
                            " (pixel data differs across loads -> clobbered on one of them)",
                            id.c_str(), cur.type, cur.slot, it->second.w, it->second.h, it->second.x, it->second.y,
                            cur.w, cur.h, cur.x, cur.y);
                }
            }
            sPrevSigs = sigs;

            // [DIAG] smallScenery-specific census. The first-editor-load bug resolves EVERY
            // scenery tile element's object lookup to null (proven by Editor::DiagMapCensus),
            // while this dump still counts the objects as loaded. That means the smallScenery
            // list is either emptied/shrunk or its slot assignment shifts away from the tile
            // elements' entry indices somewhere between end-of-LoadObjects and post-Import.
            // Snapshot the list here and again post-Import to localise which step changes it.
            const auto& ssList = _loadedObjects[EnumValue(ObjectType::smallScenery)];
            size_t ssOccupied = 0;
            size_t ssMaxSlot = 0;
            // [DIAG] FNV-1a hash over the (slot -> identifier) assignment. If this hash differs
            // between the bad and clean load, the SAME objects sit at DIFFERENT slots (the list
            // stats can be identical while the per-slot assignment shifts). If it matches, the
            // slot map is genuinely identical and the divergence is in the tile elements' entry
            // indices instead (compare against Editor::DiagMapCensus's entry-index hash).
            uint64_t ssSlotHash = 1469598103934665603ull;
            for (size_t s = 0; s < ssList.size(); s++)
            {
                if (ssList[s] == nullptr)
                    continue;
                ssOccupied++;
                ssMaxSlot = s;
                ssSlotHash = (ssSlotHash ^ s) * 1099511628211ull;
                for (char c : ssList[s]->GetIdentifier())
                    ssSlotHash = (ssSlotHash ^ static_cast<uint8_t>(c)) * 1099511628211ull;
            }

            LOG_INFO(
                "[DIAG] ===== load #%d (%s): %zu objects, %zu with G1 ranges, %d OVERLAPS, %d DUPLICATE ids, "
                "%d ZEROED-G1, %d G1-MISMATCH | smallScenery list size=%zu occupied=%zu maxSlot=%zu slotHash=%016llx =====",
                seq, tag, totalLoaded, ranges.size(), overlaps, dupes, g1Zeroed, g1Mismatch, ssList.size(),
                ssOccupied, ssMaxSlot, static_cast<unsigned long long>(ssSlotHash));
        }

        // [DIAG] Temporary: expose the dump so Context can call it again AFTER park import,
        // bracketing parkImporter->Import() to see whether the import step is what empties or
        // re-slots the smallScenery list on the first editor load. Remove with DumpLoadedObjectState.
        void DiagDumpLoadedObjectState(const char* tag) override
        {
            DumpLoadedObjectState(tag);
        }

        void UnloadObjects(const std::vector<ObjectEntryDescriptor>& entries) override
        {
            // [DIAG] temporary - catches the public unload path. The first-editor-load scenery
            // wipe nulls every transient smallScenery in place (size kept, occupied->0), which is
            // exactly what looping UnloadObject over a transient descriptor list does. Logging the
            // count here, bracketed by the post-import / end-of-LoadParkFromStream dumps, pins
            // whether THIS is the call that empties the lists and how big the descriptor set is.
            LOG_INFO("[DIAG] ObjectManager::UnloadObjects called with %zu entries", entries.size());

            // TODO there are two performance issues here:
            //        - FindObject for every entry which is a dictionary lookup
            //        - GetLoadedObjectIndex for every entry which enumerates _loadedList

            size_t numObjectsUnloaded = 0;
            for (const auto& descriptor : entries)
            {
                const auto* ori = _objectRepository.FindObject(descriptor);
                if (ori != nullptr)
                {
                    auto* loadedObject = ori->LoadedObject.get();
                    if (loadedObject != nullptr)
                    {
                        UnloadObject(loadedObject);
                        numObjectsUnloaded++;
                    }
                }
            }

            if (numObjectsUnloaded > 0)
            {
                UpdateSceneryGroupIndexes();
                ResetTypeToRideEntryIndexMap();
            }
        }

        void UnloadAllTransient() override
        {
            UnloadAll(true);
        }

        void UnloadAll() override
        {
            UnloadAll(false);
        }

        void UnloadAllForShutdown() override
        {
            UnloadAll(false, false);
        }

        void UnloadAllForRepopulation() override
        {
            // resetTypeMap=false: do NOT let ResetTypeToRideEntryIndexMap()'s pre-pass resurrect
            // custom ride parkobjs here. The caller is about to clear and rebuild the object
            // repository (ObjectRepository::LoadOrConstruct), which would free anything we
            // resurrect now and leave dangling raw pointers in _loadedObjects. See the header
            // comment on UnloadAllForRepopulation() for the full rationale.
            UnloadAll(false, false);
        }

        void ResetObjects() override
        {
            for (auto& list : _loadedObjects)
            {
                for (auto* loadedObject : list)
                {
                    if (loadedObject != nullptr)
                    {
                        loadedObject->Unload();
                        loadedObject->Load();
                    }
                }
            }
            UpdateSceneryGroupIndexes();
            ResetTypeToRideEntryIndexMap();

            // We will need to replay the title music if the title music object got reloaded
            Audio::StopTitleMusic();
            Audio::PlayTitleMusic();
            RideAudio::StopAllChannels();
        }

        std::vector<const ObjectRepositoryItem*> GetPackableObjects() override
        {
            std::vector<const ObjectRepositoryItem*> objects;
            size_t numObjects = _objectRepository.GetNumObjects();
            for (size_t i = 0; i < numObjects; i++)
            {
                const ObjectRepositoryItem* item = &_objectRepository.GetObjects()[i];
                if (item->LoadedObject != nullptr && IsObjectCustom(item))
                {
                    objects.push_back(item);
                }
            }
            return objects;
        }

        static StringId GetObjectSourceGameString(const ObjectSourceGame sourceGame)
        {
            switch (sourceGame)
            {
                case ObjectSourceGame::rct1:
                    return STR_SCENARIO_CATEGORY_RCT1;
                case ObjectSourceGame::addedAttractions:
                    return STR_SCENARIO_CATEGORY_RCT1_AA;
                case ObjectSourceGame::loopyLandscapes:
                    return STR_SCENARIO_CATEGORY_RCT1_LL;
                case ObjectSourceGame::rct2:
                    return STR_ROLLERCOASTER_TYCOON_2_DROPDOWN;
                case ObjectSourceGame::wackyWorlds:
                    return STR_OBJECT_FILTER_WW;
                case ObjectSourceGame::timeTwister:
                    return STR_OBJECT_FILTER_TT;
                case ObjectSourceGame::openRCT2Official:
                    return STR_OBJECT_FILTER_OPENRCT2_OFFICIAL;
                default:
                    return STR_OBJECT_FILTER_CUSTOM;
            }
        }

        const std::vector<ObjectEntryIndex>& GetAllRideEntries(ride_type_t rideType) override
        {
            if (rideType >= RIDE_TYPE_COUNT)
            {
                // Return an empty vector
                return _nullRideTypeEntries;
            }
            return _rideTypeToObjectMap[rideType];
        }

    private:
        std::vector<Object*>& GetObjectList(ObjectType type)
        {
            auto typeIndex = EnumValue(type);
            return _loadedObjects[typeIndex];
        }

        // resetTypeMap controls whether the ride-type-to-object map is rebuilt afterward.
        // ResetTypeToRideEntryIndexMap() has a side effect of reloading any custom ride parkobj
        // it finds missing (see its own comment) - exactly what we just unloaded above, so pass
        // false when nothing will read the map again before this manager (or the process) goes
        // away, e.g. final shutdown. Defaults to true to preserve every existing caller's behaviour.
        void UnloadAll(bool onlyTransient, bool resetTypeMap = true)
        {
            for (auto type : getAllObjectTypes())
            {
                if (!onlyTransient || !IsIntransientObjectType(type))
                {
                    auto& list = GetObjectList(type);
                    for (auto* loadedObject : list)
                    {
                        UnloadObject(loadedObject);
                    }
                    list.clear();
                }
            }
            if (resetTypeMap)
            {
                UpdateSceneryGroupIndexes();
                ResetTypeToRideEntryIndexMap();
            }
        }

        Object* LoadObject(ObjectEntryIndex slot, std::string_view identifier)
        {
            const ObjectRepositoryItem* ori = _objectRepository.FindObject(identifier);
            return RepositoryItemToObject(ori, slot);
        }

        Object* RepositoryItemToObject(const ObjectRepositoryItem* ori, std::optional<ObjectEntryIndex> slot = {})
        {
            if (ori == nullptr)
                return nullptr;

            Object* loadedObject = ori->LoadedObject.get();
            if (loadedObject != nullptr)
                return loadedObject;

            ObjectType objectType = ori->Type;
            if (slot)
            {
                auto& list = GetObjectList(objectType);
                if (list.size() > *slot && list[*slot] != nullptr)
                {
                    // Slot already taken
                    return nullptr;
                }
            }
            else
            {
                slot = FindSpareSlot(objectType);
            }
            if (slot)
            {
                auto* object = GetOrLoadObject(ori);
                if (object != nullptr)
                {
                    auto& list = GetObjectList(objectType);
                    if (list.size() <= *slot)
                    {
                        list.resize(*slot + 1);
                    }
                    loadedObject = object;
                    list[*slot] = object;
                    UpdateSceneryGroupIndexes();
                    ResetTypeToRideEntryIndexMap();
                }
            }
            return loadedObject;
        }

        std::optional<ObjectEntryIndex> FindSpareSlot(ObjectType objectType)
        {
            auto& list = GetObjectList(objectType);
            auto it = std::find(list.begin(), list.end(), nullptr);
            if (it != list.end())
            {
                return static_cast<ObjectEntryIndex>(std::distance(list.begin(), it));
            }

            auto maxSize = getObjectEntryGroupCount(objectType);
            if (list.size() < static_cast<size_t>(maxSize))
            {
                list.emplace_back();
                return static_cast<ObjectEntryIndex>(list.size() - 1);
            }
            return std::nullopt;
        }

        size_t GetLoadedObjectIndex(const Object* object)
        {
            Guard::ArgumentNotNull(object, GUARD_LINE);

            auto result = std::numeric_limits<size_t>().max();
            auto& list = GetObjectList(object->GetObjectType());
            auto it = std::find(list.begin(), list.end(), object);
            if (it != list.end())
            {
                result = std::distance(list.begin(), it);
            }
            return result;
        }

        void UnloadObject(Object* object)
        {
            if (object == nullptr)
                return;

            // Because it's possible to have the same loaded object for multiple
            // slots, we have to make sure find and set all of them to nullptr
            auto& list = GetObjectList(object->GetObjectType());
            std::replace(list.begin(), list.end(), object, static_cast<Object*>(nullptr));

            object->Unload();

            // TODO try to prevent doing a repository search
            const auto* ori = _objectRepository.FindObject(object->GetDescriptor());
            if (ori != nullptr)
            {
                _objectRepository.UnregisterLoadedObject(ori, object);
            }
        }

        void UnloadObjectsExcept(const std::vector<Object*>& newLoadedObjects)
        {
            // Build a hash set for quick checking
            auto exceptSet = std::unordered_set<Object*>();
            for (auto& object : newLoadedObjects)
            {
                if (object != nullptr)
                {
                    exceptSet.insert(object);
                }
            }

            // Unload objects that are not in the hash set
            size_t totalObjectsLoaded = 0;
            size_t numObjectsUnloaded = 0;
            for (auto type : getAllObjectTypes())
            {
                if (!IsIntransientObjectType(type))
                {
                    auto& list = GetObjectList(type);
                    for (auto& object : list)
                    {
                        if (object == nullptr)
                            continue;

                        totalObjectsLoaded++;
                        if (exceptSet.find(object) == exceptSet.end())
                        {
                            UnloadObject(object);
                            object = nullptr;
                            numObjectsUnloaded++;
                        }
                    }
                }
            }

            LOG_VERBOSE("%u / %u objects unloaded", numObjectsUnloaded, totalObjectsLoaded);
        }

        template<typename T>
        void UpdateSceneryGroupIndexes(ObjectType type)
        {
            auto& list = GetObjectList(type);
            for (auto* loadedObject : list)
            {
                if (loadedObject != nullptr)
                {
                    auto* sceneryEntry = static_cast<T*>(loadedObject->GetLegacyData());
                    sceneryEntry->scenery_tab_id = GetPrimarySceneryGroupEntryIndex(loadedObject);
                }
            }
        }

        void UpdateSceneryGroupIndexes()
        {
            UpdateSceneryGroupIndexes<SmallSceneryEntry>(ObjectType::smallScenery);
            UpdateSceneryGroupIndexes<LargeSceneryEntry>(ObjectType::largeScenery);
            UpdateSceneryGroupIndexes<WallSceneryEntry>(ObjectType::walls);
            UpdateSceneryGroupIndexes<BannerSceneryEntry>(ObjectType::banners);
            UpdateSceneryGroupIndexes<PathAdditionEntry>(ObjectType::pathAdditions);

            auto& list = GetObjectList(ObjectType::sceneryGroup);
            for (auto* loadedObject : list)
            {
                auto sgObject = static_cast<SceneryGroupObject*>(loadedObject);
                if (sgObject != nullptr)
                {
                    sgObject->UpdateEntryIndexes();
                }
            }
        }

        ObjectEntryIndex GetPrimarySceneryGroupEntryIndex(Object* loadedObject)
        {
            auto* sceneryObject = dynamic_cast<SceneryObject*>(loadedObject);
            const auto& primarySGEntry = sceneryObject->GetPrimarySceneryGroup();
            Object* sgObject = GetLoadedObject(primarySGEntry);

            auto entryIndex = kObjectEntryIndexNull;
            if (sgObject != nullptr)
            {
                entryIndex = GetLoadedObjectEntryIndex(sgObject);
            }
            return entryIndex;
        }

        std::vector<ObjectToLoad> GetRequiredObjects(const ObjectList& objectList)
        {
            std::vector<ObjectToLoad> requiredObjects;
            std::vector<ObjectEntryDescriptor> missingObjects;

            for (auto objectType : getAllObjectTypes())
            {
                auto& descriptors = objectList.GetList(objectType);
                auto maxSize = static_cast<size_t>(getObjectEntryGroupCount(objectType));
                auto listSize = static_cast<ObjectEntryIndex>(std::min(descriptors.size(), maxSize));
                for (ObjectEntryIndex i = 0; i < listSize; i++)
                {
                    const auto& entry = objectList.GetObject(objectType, i);
                    if (entry.HasValue())
                    {
                        const auto* ori = _objectRepository.FindObject(entry);
                        if (ori == nullptr && entry.GetType() == ObjectType::scenarioMeta)
                        {
                            continue;
                        }

                        if (ori == nullptr)
                        {
                            missingObjects.push_back(entry);
                            ReportMissingObject(entry);
                        }

                        ObjectToLoad otl;
                        otl.RepositoryItem = ori;
                        otl.Index = i;
                        requiredObjects.push_back(otl);
                    }
                }
            }

            if (!missingObjects.empty())
            {
                throw ObjectLoadException(std::move(missingObjects));
            }

            return requiredObjects;
        }

        void ReportProgress(size_t numLoaded, size_t numRequired)
        {
            constexpr auto kObjectLoadMinProgress = 10;
            constexpr auto kObjectLoadMaxProgress = 90;
            constexpr auto kObjectLoadProgressRange = kObjectLoadMaxProgress - kObjectLoadMinProgress;

            const auto currentProgress = kObjectLoadMinProgress + (numLoaded * kObjectLoadProgressRange / numRequired);
            GetContext()->SetProgress(static_cast<uint32_t>(currentProgress), 100, STR_STRING_M_PERCENT);
        }

        void LoadObjects(std::vector<ObjectToLoad>& requiredObjects, bool reportProgress)
        {
            std::vector<Object*> objects;
            std::vector<Object*> newLoadedObjects;
            std::vector<ObjectEntryDescriptor> badObjects;

            // Create a list of objects that are currently not loaded but required.
            std::vector<const ObjectRepositoryItem*> objectsToLoad;
            for (auto& requiredObject : requiredObjects)
            {
                auto* repositoryItem = requiredObject.RepositoryItem;
                if (repositoryItem == nullptr)
                {
                    continue;
                }

                auto* loadedObject = repositoryItem->LoadedObject.get();
                if (loadedObject == nullptr)
                {
                    objectsToLoad.push_back(repositoryItem);
                }
            }

            // De-duplicate the list, since loading happens in parallel we can't have it race the repository item.
            std::sort(objectsToLoad.begin(), objectsToLoad.end());
            objectsToLoad.erase(std::unique(objectsToLoad.begin(), objectsToLoad.end()), objectsToLoad.end());

            // Prepare for loading objects multi-threaded
            std::atomic<int> numProcessed = 0;
            auto numRequired = objectsToLoad.size();
            std::mutex commonMutex;
            auto loadSingleObject = [&](const ObjectRepositoryItem* requiredObject) {
                // Object requires to be loaded, if the object successfully loads it will register it
                // as a loaded object otherwise placed into the badObjects list.
                auto newObject = _objectRepository.LoadObject(requiredObject);

                std::lock_guard<std::mutex> guard(commonMutex);
                if (newObject == nullptr)
                {
                    badObjects.push_back(ObjectEntryDescriptor(requiredObject->ObjectEntry));
                    ReportObjectLoadProblem(&requiredObject->ObjectEntry);
                }
                else
                {
                    newLoadedObjects.push_back(newObject.get());
                    // Connect the ori to the registered object
                    _objectRepository.RegisterLoadedObject(requiredObject, std::move(newObject));
                }

                numProcessed.fetch_add(1);
            };

            auto completionFn = [&]() {
                auto processed = numProcessed.load();
                if (reportProgress && (processed % 100) == 0)
                    ReportProgress(processed, numRequired);
            };

            // Dispatch loading the objects
            JobPool jobs{};
            for (auto* object : objectsToLoad)
            {
                jobs.AddTask([object, &loadSingleObject]() { loadSingleObject(object); }, completionFn);
            }

            // Wait until all jobs are fully completed
            jobs.Join();

            // Assign the loaded objects to the required objects
            for (auto& requiredObject : requiredObjects)
            {
                auto* repositoryItem = requiredObject.RepositoryItem;
                if (repositoryItem == nullptr)
                {
                    continue;
                }
                auto* loadedObject = repositoryItem->LoadedObject.get();
                if (loadedObject == nullptr)
                {
                    continue;
                }
                requiredObject.LoadedObject = loadedObject;
                objects.push_back(loadedObject);
            }

            // Load objects
            for (auto* obj : newLoadedObjects)
            {
                obj->Load();
            }

            if (!badObjects.empty())
            {
                // Unload all the new objects we loaded
                for (auto* object : newLoadedObjects)
                {
                    UnloadObject(object);
                }
                throw ObjectLoadException(std::move(badObjects));
            }

            // Unload objects which are not in the required list.
            if (objects.empty())
            {
                UnloadAllTransient();
            }
            else
            {
                UnloadObjectsExcept(objects);
            }

            // Set the new object lists
            for (auto type : getAllObjectTypes())
            {
                if (!IsIntransientObjectType(type))
                {
                    auto& list = GetObjectList(type);
                    list.clear();
                }
            }
            for (auto& otl : requiredObjects)
            {
                auto objectType = otl.LoadedObject->GetObjectType();
                auto& list = GetObjectList(objectType);
                if (list.size() <= otl.Index)
                {
                    list.resize(otl.Index + 1);
                }
                list[otl.Index] = otl.LoadedObject;
            }

            LOG_VERBOSE("%u / %u new objects loaded", newLoadedObjects.size(), requiredObjects.size());
        }

        Object* GetOrLoadObject(const ObjectRepositoryItem* ori)
        {
            auto* loadedObject = ori->LoadedObject.get();
            if (loadedObject != nullptr)
                return loadedObject;

            // Try to load object
            auto object = _objectRepository.LoadObject(ori);
            if (object != nullptr)
            {
                loadedObject = object.get();

                object->Load();

                // Connect the ori to the registered object
                _objectRepository.RegisterLoadedObject(ori, std::move(object));
            }

            return loadedObject;
        }

        void ResetTypeToRideEntryIndexMap()
        {
            // Re-entrancy guard (see _isResettingTypeMap's own comment): every object load,
            // including the ones the pre-pass below issues itself, ends with a call back into
            // this very function. A nested call used to be allowed to run to completion -
            // rebuilding _rideTypeToObjectMap and re-scanning _loadedObjects - while an outer
            // invocation was still mid-flight against that same state, corrupting it. The
            // outer-most call already covers every registered custom ride type in its own
            // single forward pre-pass loop below, so letting a nested call additionally load
            // objects and rebuild the map was never actually necessary for correctness - it
            // was just an unintended side effect of LoadObject always refreshing the map.
            // A scope guard (not a plain bool set/clear) keeps this safe even if LoadObject
            // throws partway through (e.g. a malformed manifest.json) - the flag must clear on
            // every exit path, not just the normal one, or every later call would short-circuit
            // forever.
            if (_isResettingTypeMap)
                return;
            struct ScopedReentryGuard
            {
                bool& flag;
                explicit ScopedReentryGuard(bool& f) : flag(f) { flag = true; }
                ~ScopedReentryGuard() { flag = false; }
            } reentryGuard(_isResettingTypeMap);

            // Pre-pass: ensure all custom ride parkobjs are loaded.
            // When a park file is opened the object manager evicts objects not saved with that park;
            // custom ride vehicles won't be in any existing park's object list. LoadObject is a no-op
            // if the object is already present.
            auto& registry = GetRideTypeRegistry();
            for (uint32_t ci = RIDE_TYPE_COUNT; ci < registry.Count(); ci++)
            {
                const auto& rtd = registry.Get(ci);
                if (rtd.CustomParkObjId != nullptr
                    && GetLoadedObjectEntryIndex(std::string_view(rtd.CustomParkObjId)) == kObjectEntryIndexNull)
                {
                    LoadObject(std::string_view(rtd.CustomParkObjId));
                }
            }

            // Resize to cover all registered ride types (built-ins + any custom),
            // then clear each slot ready for repopulation below.
            _rideTypeToObjectMap.assign(GetRideTypeCount(), {});

            // Build object lists from loaded parkobjs.
            const auto maxRideObjects = static_cast<size_t>(getObjectEntryGroupCount(ObjectType::ride));
            for (size_t i = 0; i < maxRideObjects; i++)
            {
                auto* rideObject = static_cast<RideObject*>(GetLoadedObject(ObjectType::ride, i));
                if (rideObject == nullptr)
                    continue;

                const auto& entry = rideObject->GetEntry();

                for (auto rideType : entry.ride_type)
                {
                    if (rideType < _rideTypeToObjectMap.size())
                    {
                        auto& v = _rideTypeToObjectMap[rideType];
                        v.push_back(static_cast<ObjectEntryIndex>(i));
                    }
                }
            }

            // Populate custom ride type slots. Custom types aren't listed in any parkobj's ride_type[]
            // array, so we resolve them from CustomParkObjId after the pre-pass guarantees they're loaded.
            for (uint32_t ci = RIDE_TYPE_COUNT; ci < registry.Count(); ci++)
            {
                const auto& rtd = registry.Get(ci);
                if (rtd.CustomParkObjId == nullptr)
                    continue;
                auto entryIndex = GetLoadedObjectEntryIndex(std::string_view(rtd.CustomParkObjId));
                if (entryIndex == kObjectEntryIndexNull)
                    continue;
                if (ci >= _rideTypeToObjectMap.size())
                    _rideTypeToObjectMap.resize(ci + 1);
                _rideTypeToObjectMap[ci].push_back(entryIndex);
            }
        }

        static void ReportMissingObject(const ObjectEntryDescriptor& entry)
        {
            std::string name(entry.GetName());
            Console::Error::WriteLine("[%s] Object not found.", name.c_str());
        }

        void ReportObjectLoadProblem(const RCTObjectEntry* entry)
        {
            utf8 objName[kDatNameLength + 1] = { 0 };
            std::copy_n(entry->name, kDatNameLength, objName);
            Console::Error::WriteLine("[%s] Object could not be loaded.", objName);
        }
    };

    std::unique_ptr<IObjectManager> CreateObjectManager(IObjectRepository& objectRepository)
    {
        return std::make_unique<ObjectManager>(objectRepository);
    }

    Object* ObjectManagerGetLoadedObject(const ObjectEntryDescriptor& entry)
    {
        auto& objectManager = GetContext()->GetObjectManager();
        Object* loadedObject = objectManager.GetLoadedObject(entry);
        return loadedObject;
    }

    ObjectEntryIndex ObjectManagerGetLoadedObjectEntryIndex(const Object* loadedObject)
    {
        auto& objectManager = GetContext()->GetObjectManager();
        auto entryIndex = objectManager.GetLoadedObjectEntryIndex(loadedObject);
        return entryIndex;
    }

    ObjectEntryIndex ObjectManagerGetLoadedObjectEntryIndex(const ObjectEntryDescriptor& entry)
    {
        return ObjectManagerGetLoadedObjectEntryIndex(ObjectManagerGetLoadedObject(entry));
    }

    Object* ObjectManagerLoadObject(const RCTObjectEntry* entry)
    {
        auto& objectManager = GetContext()->GetObjectManager();
        Object* loadedObject = objectManager.LoadObject(entry);
        return loadedObject;
    }

    void ObjectManagerUnloadObjects(const std::vector<ObjectEntryDescriptor>& entries)
    {
        auto& objectManager = GetContext()->GetObjectManager();
        objectManager.UnloadObjects(entries);
    }

    void ObjectManagerUnloadAllObjects()
    {
        auto& objectManager = GetContext()->GetObjectManager();
        objectManager.UnloadAllTransient();
    }

    StringId ObjectManagerGetSourceGameString(const ObjectSourceGame sourceGame)
    {
        return ObjectManager::GetObjectSourceGameString(sourceGame);
    }
} // namespace OpenRCT2
