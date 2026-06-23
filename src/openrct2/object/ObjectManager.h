/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../localisation/StringIdType.h"
#include "../ride/RideTypes.h"
#include "ObjectSourceGame.h"
#include "ObjectTypes.h"

#include <memory>
#include <string_view>
#include <vector>

namespace OpenRCT2
{
    struct IObjectRepository;
    class Object;
    class ObjectList;
    struct ObjectEntryDescriptor;
    struct ObjectRepositoryItem;
    struct RCTObjectEntry;

    struct IObjectManager
    {
        virtual ~IObjectManager()
        {
        }

        virtual Object* GetLoadedObject(ObjectType objectType, size_t index) = 0;
        template<typename TClass>
        TClass* GetLoadedObject(size_t index)
        {
            return static_cast<TClass*>(GetLoadedObject(TClass::kObjectType, index));
        }
        virtual Object* GetLoadedObject(const ObjectEntryDescriptor& entry) = 0;
        virtual ObjectEntryIndex GetLoadedObjectEntryIndex(std::string_view identifier) = 0;
        virtual ObjectEntryIndex GetLoadedObjectEntryIndex(const ObjectEntryDescriptor& descriptor) = 0;
        virtual ObjectEntryIndex GetLoadedObjectEntryIndex(const Object* object) = 0;
        virtual ObjectList GetLoadedObjects() = 0;

        virtual std::unique_ptr<Object> LoadTempObject(std::string_view identifier, bool loadImages) = 0;
        virtual Object* LoadObject(std::string_view identifier) = 0;
        virtual Object* LoadObject(const RCTObjectEntry* entry) = 0;
        virtual Object* LoadObject(const ObjectEntryDescriptor& descriptor) = 0;
        virtual Object* LoadObject(const ObjectEntryDescriptor& descriptor, ObjectEntryIndex slot) = 0;
        virtual Object* LoadRepositoryItem(const ObjectRepositoryItem& ori) = 0;
        virtual void LoadObjects(const ObjectList& entries, bool reportProgress = false) = 0;
        virtual void UnloadObjects(const std::vector<ObjectEntryDescriptor>& entries) = 0;
        virtual void UnloadAllTransient() = 0;
        virtual void UnloadAll() = 0;
        // Final-teardown variant of UnloadAll(): unloads every object without rebuilding the
        // ride-type-to-object map afterward. UnloadAll() always rebuilds that map via
        // ResetTypeToRideEntryIndexMap(), which has a side effect of reloading any custom ride
        // parkobj it finds missing (intended for switching parks mid-session, where custom rides
        // must stay resident) - called from the final shutdown path, that reload immediately
        // re-allocates the images UnloadAll() just freed, with nothing left to free them again
        // before the engine's exit-time "were all images freed?" assertion runs. Call this instead
        // of UnloadAll() anywhere nothing will read the ride-type map again afterward.
        virtual void UnloadAllForShutdown() = 0;

        virtual void ResetObjects() = 0;

        virtual std::vector<const ObjectRepositoryItem*> GetPackableObjects() = 0;
        virtual const std::vector<ObjectEntryIndex>& GetAllRideEntries(ride_type_t rideType) = 0;
    };

    [[nodiscard]] std::unique_ptr<IObjectManager> CreateObjectManager(IObjectRepository& objectRepository);

    [[nodiscard]] Object* ObjectManagerGetLoadedObject(const ObjectEntryDescriptor& entry);
    [[nodiscard]] ObjectEntryIndex ObjectManagerGetLoadedObjectEntryIndex(const Object* loadedObject);
    [[nodiscard]] ObjectEntryIndex ObjectManagerGetLoadedObjectEntryIndex(const ObjectEntryDescriptor& entry);
    Object* ObjectManagerLoadObject(const RCTObjectEntry* entry);
    void ObjectManagerUnloadObjects(const std::vector<ObjectEntryDescriptor>& entries);
    void ObjectManagerUnloadAllObjects();
    [[nodiscard]] StringId ObjectManagerGetSourceGameString(ObjectSourceGame sourceGame);
} // namespace OpenRCT2
