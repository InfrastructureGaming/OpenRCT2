/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Editor.h"

#include "Context.h"
#include "Diagnostic.h"                            // [DIAG] LOG_INFO for the temporary first-editor-load map census
#include "world/tile_element/LargeSceneryElement.h" // [DIAG] GetEntryIndex() on concrete element type
#include "world/tile_element/SmallSceneryElement.h" // [DIAG] GetEntryIndex() on concrete element type
#include "world/tile_element/WallElement.h"         // [DIAG] GetEntryIndex() on concrete element type
#include "EditorObjectSelectionSession.h"
#include "FileClassifier.h"
#include "Game.h"
#include "GameState.h"
#include "OpenRCT2.h"
#include "ParkImporter.h"
#include "actions/GameActionRunner.h"
#include "actions/ResultWithMessage.h"
#include "actions/park/LandBuyRightsAction.h"
#include "actions/park/LandSetRightsAction.h"
#include "audio/Audio.h"
#include "core/EnumUtils.hpp"
#include "core/Path.hpp"
#include "core/String.hpp"
#include "drawing/Drawing.h"
#include "entity/EntityList.h"
#include "entity/EntityRegistry.h"
#include "entity/Guest.h"
#include "entity/PatrolArea.h"
#include "entity/Staff.h"
#include "interface/WindowBase.h"
#include "localisation/LocalisationService.h"
#include "management/Finance.h"
#include "management/NewsItem.h"
#include "object/DefaultObjects.h"
#include "object/ObjectManager.h"
#include "object/ObjectRepository.h"
#include "peep/PeepAnimations.h"
#include "rct1/RCT1.h"
#include "scenario/Scenario.h"
#include "scripting/ScriptEngine.h"
#include "ui/WindowManager.h"
#include "windows/Intent.h"
#include "world/Entrance.h"
#include "world/Footpath.h"
#include "world/Map.h"
#include "world/MapLimits.h"
#include "world/Park.h"
#include "world/Scenery.h"
#include "world/Weather.h"

#include <algorithm> // [DIAG] std::min/std::max for the smallScenery entry-index range in DiagMapCensus
#include <array>
#include <cassert>
#include <vector>

using namespace OpenRCT2;
using OpenRCT2::GameActions::CommandFlag;

namespace OpenRCT2::Editor
{
    static std::array<std::vector<uint8_t>, EnumValue(ObjectType::count)> _editorSelectedObjectFlags;

    static void ConvertSaveToScenarioCallback(ModalResult result, const utf8* path);
    static void SetAllLandOwned();
    static void FinaliseMainView();
    static void ClearMapForEditing(bool fromSave);

    static void ObjectListLoad()
    {
        auto* context = GetContext();
        context->OpenProgress(STR_LOADING_GENERIC);

        // Unload objects first, the repository is re-populated which owns the objects.
        // UnloadAllForRepopulation() (not UnloadAll()): LoadOrConstruct() below clears and rebuilds
        // the repository, freeing every object it owns. UnloadAll() would resurrect custom ride
        // parkobjs into the loaded-object lists right before that teardown, leaving dangling raw
        // pointers that the next park load's UnloadObjectsExcept() dereferences. The minimum-object
        // loads further down rebuild the ride-type map against the fresh repository.
        auto& objectManager = context->GetObjectManager();
        objectManager.UnloadAllForRepopulation();

        // Scan objects if necessary
        const auto& localisationService = context->GetLocalisationService();
        auto& objectRepository = context->GetObjectRepository();
        objectRepository.LoadOrConstruct(localisationService.GetCurrentLanguage());

        Audio::LoadAudioObjects();

        // Reset loaded objects to just defaults
        // Load minimum required objects (like surface and edge)
        for (const auto& entry : kMinimumRequiredObjects)
        {
            objectManager.LoadObject(entry);
        }

        context->CloseProgress();
    }

    static WindowBase* OpenEditorWindows()
    {
        auto* main = ContextOpenWindow(WindowClass::mainWindow);
        ContextOpenWindow(WindowClass::topToolbar);
        ContextOpenWindowView(WindowView::editorBottomToolbar);
        return main;
    }

    /**
     *
     *  rct2: 0x0066FFE1
     */
    void Load()
    {
        // TODO: replace with dedicated scene
        auto* context = GetContext();
        context->SetActiveScene(context->GetGameScene());

        auto& gameState = getGameState();
        Audio::StopAll();
        gameStateInitAll(gameState, kDefaultMapSize);
        gLegacyScene = LegacyScene::scenarioEditor;
        gameState.editorStep = EditorStep::objectSelection;
        gameState.park.flags |= PARK_FLAGS_SHOW_REAL_GUEST_NAMES;
        gameState.scenarioOptions.category = Scenario::Category::other;
        ObjectListLoad();
        ContextResetSubsystems();
        WindowBase* mainWindow = OpenEditorWindows();
        mainWindow->setViewportLocation(TileCoordsXYZ{ 75, 75, 14 }.ToCoordsXYZ());
        LoadPalette();
        gScreenAge = 0;
        gameState.scenarioOptions.name = LanguageGetString(STR_MY_NEW_SCENARIO);

        GameLoadScripts();
        GameNotifyMapChanged();
    }

    /**
     *
     *  rct2: 0x00672781
     */
    void ConvertSaveToScenario()
    {
        ToolCancel();
        auto intent = Intent(WindowClass::loadsave);
        intent.PutEnumExtra<LoadSaveAction>(INTENT_EXTRA_LOADSAVE_ACTION, LoadSaveAction::load);
        intent.PutEnumExtra<LoadSaveType>(INTENT_EXTRA_LOADSAVE_TYPE, LoadSaveType::park);
        intent.PutExtra(INTENT_EXTRA_CALLBACK, reinterpret_cast<CloseCallback>(ConvertSaveToScenarioCallback));
        ContextOpenIntent(&intent);
    }

    static void ConvertSaveToScenarioCallback(ModalResult result, const utf8* path)
    {
        if (result != ModalResult::ok)
        {
            return;
        }

        if (!GetContext()->LoadParkFromFile(path))
        {
            return;
        }

        auto& gameState = getGameState();
        ScenarioReset(gameState);

        gLegacyScene = LegacyScene::scenarioEditor;
        gameState.editorStep = EditorStep::optionsSelection;
        gameState.scenarioOptions.category = Scenario::Category::other;
        ContextResetSubsystems();
        OpenEditorWindows();
        FinaliseMainView();
        gScreenAge = 0;

        GameLoadScripts();
        GameNotifyMapChanged();

#ifdef ENABLE_SCRIPTING
        // Clear the plugin storage before saving
        auto& scriptEngine = GetContext()->GetScriptEngine();
        scriptEngine.ClearParkStorage();
#endif
    }

    /**
     *
     *  rct2: 0x00672957
     */
    void LoadTrackDesigner()
    {
        // TODO: replace with dedicated scene
        auto* context = GetContext();
        context->SetActiveScene(context->GetGameScene());

        Audio::StopAll();
        gLegacyScene = LegacyScene::trackDesigner;
        gScreenAge = 0;

        auto& gameState = getGameState();
        gameStateInitAll(gameState, kDefaultMapSize);
        gameState.editorStep = EditorStep::objectSelection;
        SetAllLandOwned();
        ObjectListLoad();
        ContextResetSubsystems();
        WindowBase* mainWindow = OpenEditorWindows();
        mainWindow->setViewportLocation(TileCoordsXYZ{ 75, 75, 14 }.ToCoordsXYZ());
        LoadPalette();

        GameLoadScripts();
        GameNotifyMapChanged();
    }

    /**
     *
     *  rct2: 0x006729FD
     */
    void LoadTrackManager()
    {
        // TODO: replace with dedicated scene
        auto* context = GetContext();
        context->SetActiveScene(context->GetGameScene());

        Audio::StopAll();
        gLegacyScene = LegacyScene::trackDesignsManager;
        gScreenAge = 0;

        auto& gameState = getGameState();
        gameStateInitAll(gameState, kDefaultMapSize);
        SetAllLandOwned();
        gameState.editorStep = EditorStep::objectSelection;
        ObjectListLoad();
        ContextResetSubsystems();
        WindowBase* mainWindow = OpenEditorWindows();
        mainWindow->setViewportLocation(TileCoordsXYZ{ 75, 75, 14 }.ToCoordsXYZ());
        LoadPalette();

        GameLoadScripts();
        GameNotifyMapChanged();
    }

    /**
     *
     *  rct2: 0x0068ABEC
     */
    static void SetAllLandOwned()
    {
        auto& gameState = getGameState();

        MapRange range = { 2 * kCoordsXYStep, 2 * kCoordsXYStep, (gameState.mapSize.x - 3) * kCoordsXYStep,
                           (gameState.mapSize.y - 3) * kCoordsXYStep };

        auto landSetRightsAction = GameActions::LandSetRightsAction(range, GameActions::LandSetRightSetting::setForSale);
        landSetRightsAction.SetFlags({ CommandFlag::noSpend });
        GameActions::Execute(&landSetRightsAction, gameState);

        auto landBuyRightsAction = GameActions::LandBuyRightsAction(range, GameActions::LandBuyRightSetting::buyLand);
        landBuyRightsAction.SetFlags({ CommandFlag::noSpend });
        GameActions::Execute(&landBuyRightsAction, gameState);
    }

    // [DIAG] TEMPORARY - first-editor-load scenery-corruption hunt. Object + G1 image state at
    // the end of LoadObjects is proven byte-identical between the bad first load and the clean
    // subsequent loads, so the divergence must live in the imported map itself or in a
    // post-import step. This walks every tile element and reports counts by type; for each
    // scenery/wall element it also resolves the referenced loaded object and counts how many
    // point at a null (unloaded) slot. Compare the "post-import" line of load #1 (bad) against
    // load #2 (clean): if scenery counts differ, the import dropped elements; if counts match but
    // null-refs differ, object resolution is wrong; if both match, the map is intact and the
    // fault is purely render-side. The "post-cleanup" line brackets ClearMapForEditing
    // (MapRemoveAllRides / RideInitAll) to catch the editor wrapper damaging the array.
    // Remove this function and its two call sites once root-caused.
    static void DiagMapCensus(const char* tag)
    {
        static int sSeq = 0;
        const int seq = ++sSeq;

        auto& objManager = GetContext()->GetObjectManager();

        // [DIAG] Dump the loaded-object state through THIS census's own object-manager handle, at
        // census time. The post-import dump (same _loadedObjects, moments earlier) shows smallScenery
        // size=1141 fully occupied, so list[878] is non-null and the bounds check is constant - yet
        // GetLoadedObject(878) returns null here. Either the list emptied in between (this dump shows
        // occupied~0) or this handle reads a DIFFERENT manager than the loader used (this dump shows
        // a different size/slotHash, or 1141 while the lookups below still return null).
        objManager.DiagDumpLoadedObjectState("census-time via census handle");

        size_t total = 0;
        size_t surface = 0, path = 0, track = 0, entrance = 0, banner = 0, other = 0;
        size_t smScen = 0, smScenNull = 0;
        size_t lgScen = 0, lgScenNull = 0;
        size_t wall = 0, wallNull = 0;

        // [DIAG] Track the actual smallScenery entry indices the tile elements reference. The object
        // LIST is proven identical (same size/occupied/slotHash) between the bad and clean load, yet
        // every lookup is null on the bad load. So either the tile elements carry DIFFERENT entry
        // indices on the bad load (idxHash/min/max will differ from the clean load), or the slot
        // assignment behind those indices differs (slotHash in the ObjectManager dump will differ).
        // One sample of the first element pins the exact value and the slot it (fails to) hit.
        uint64_t ssIdxHash = 1469598103934665603ull;
        uint32_t ssMinIdx = 0xFFFFFFFFu, ssMaxIdx = 0;
        bool ssSampled = false;
        uint32_t ssSampleIdx = 0;
        bool ssSampleResolved = false;

        TileElementIterator it;
        TileElementIteratorBegin(&it);
        while (TileElementIteratorNext(&it))
        {
            total++;
            switch (it.element->getType())
            {
                case TileElementType::Surface:
                    surface++;
                    break;
                case TileElementType::Path:
                    path++;
                    break;
                case TileElementType::Track:
                    track++;
                    break;
                case TileElementType::Entrance:
                    entrance++;
                    break;
                case TileElementType::Banner:
                    banner++;
                    break;
                case TileElementType::SmallScenery:
                {
                    smScen++;
                    auto idx = it.element->asSmallScenery()->GetEntryIndex();
                    bool resolved = objManager.GetLoadedObject(ObjectType::smallScenery, idx) != nullptr;
                    if (!resolved)
                        smScenNull++;
                    ssIdxHash = (ssIdxHash ^ idx) * 1099511628211ull;
                    ssMinIdx = std::min(ssMinIdx, static_cast<uint32_t>(idx));
                    ssMaxIdx = std::max(ssMaxIdx, static_cast<uint32_t>(idx));
                    if (!ssSampled)
                    {
                        ssSampled = true;
                        ssSampleIdx = idx;
                        ssSampleResolved = resolved;
                    }
                    break;
                }
                case TileElementType::LargeScenery:
                {
                    lgScen++;
                    auto idx = it.element->asLargeScenery()->GetEntryIndex();
                    if (objManager.GetLoadedObject(ObjectType::largeScenery, idx) == nullptr)
                        lgScenNull++;
                    break;
                }
                case TileElementType::Wall:
                {
                    wall++;
                    auto idx = it.element->asWall()->GetEntryIndex();
                    if (objManager.GetLoadedObject(ObjectType::walls, idx) == nullptr)
                        wallNull++;
                    break;
                }
                default:
                    other++;
                    break;
            }
        }

        LOG_INFO(
            "[DIAG-MAP] #%d (%s): total=%zu surface=%zu path=%zu track=%zu entrance=%zu banner=%zu other=%zu | "
            "smallScenery=%zu (null=%zu) largeScenery=%zu (null=%zu) wall=%zu (null=%zu) | ssIdx min=%u max=%u "
            "idxHash=%016llx sample idx=%u resolved=%d",
            seq, tag, total, surface, path, track, entrance, banner, other, smScen, smScenNull, lgScen, lgScenNull,
            wall, wallNull, ssMinIdx, ssMaxIdx, static_cast<unsigned long long>(ssIdxHash), ssSampleIdx,
            ssSampleResolved ? 1 : 0);
    }

    static void AfterLoadCleanup(bool loadedFromSave)
    {
        DiagMapCensus("post-import (pre-cleanup)"); // [DIAG] temporary
        ClearMapForEditing(loadedFromSave);
        DiagMapCensus("post-cleanup"); // [DIAG] temporary

        // TODO: replace with dedicated scene
        auto* context = GetContext();
        context->SetActiveScene(context->GetGameScene());

        getGameState().editorStep = EditorStep::landscapeEditor;
        gScreenAge = 0;
        gLegacyScene = LegacyScene::scenarioEditor;
        ContextResetSubsystems();
        OpenEditorWindows();
        FinaliseMainView();

        GameLoadScripts();
        GameNotifyMapChanged();
    }

    bool LoadLandscape(const utf8* path)
    {
        // #4996: Make sure the object selection window closes here to prevent unload objects
        //        after we have loaded a new park.
        //
        // CloseAll() alone is not enough: it honours each window's canClose(), and the object
        // selection window's canClose() (EditorObjectSelectionWindowCheck) vetoes closing whenever a
        // required object type is unselected - which is the normal state on the very first landscape
        // load. The window then survives this CloseAll, lives through the park import, and finally
        // fires its onClose() mid-load. onClose() runs UnloadUnselectedObjects(), which unloads every
        // object the *editor's* (now stale) selection didn't include - i.e. the entire freshly loaded
        // park's scenery - leaving the first-loaded landscape showing naked terrain. ForceClose()
        // bypasses the canClose() veto (it sets the window's override-checks flag, then closes), so the
        // unselected-object cleanup runs now, against the pre-load editor state, where it is harmless;
        // the subsequent park load repopulates whatever it needs. Must precede CloseAll() so the
        // window is gone before the park is imported.
        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->ForceClose(WindowClass::editorObjectSelection);
        windowMgr->CloseAll();

        // [DIAG] temporary - the #4996 CloseAll above is supposed to dismiss the object-selection
        // window before the park loads so its onClose can't unload the new park's objects. Close()
        // honours canClose(), and that window's canClose()==EditorObjectSelectionWindowCheck()
        // returns false whenever a required object type is unselected - so the window can SURVIVE
        // this CloseAll and fire onClose later, mid-load. Log whether it's still alive here.
        LOG_INFO(
            "[DIAG] LoadLandscape post-CloseAll: editorObjectSelection window still open=%d",
            windowMgr->FindByClass(WindowClass::editorObjectSelection) != nullptr);

        if (!GetContext()->LoadParkFromFile(path))
            return false;

        auto extension = Path::GetExtension(path);
        bool loadedFromSave = !ParkImporter::ExtensionIsScenario(extension);

        AfterLoadCleanup(loadedFromSave);
        return true;
    }

    static void ClearMapForEditing(bool fromSave)
    {
        MapRemoveAllRides();
        UnlinkAllRideBanners();

        RideInitAll();

        for (auto* guest : EntityList<Guest>())
        {
            guest->SetName({});
        }
        for (auto* staff : EntityList<Staff>())
        {
            staff->SetName({});
        }

        getGameState().entities.ResetAllEntities();
        UpdateConsolidatedPatrolAreas();

        auto& gameState = getGameState();
        auto& park = gameState.park;
        auto& scenarioOptions = gameState.scenarioOptions;

        park.numGuestsInPark = 0;
        park.numGuestsHeadingForPark = 0;
        park.numGuestsInParkLastWeek = 0;
        park.guestChangeModifier = 0;

        if (fromSave)
        {
            park.flags |= PARK_FLAGS_NO_MONEY;

            if (park.entranceFee == 0)
            {
                park.flags |= PARK_FLAGS_PARK_FREE_ENTRY;
            }
            else
            {
                park.flags &= ~PARK_FLAGS_PARK_FREE_ENTRY;
            }

            park.flags &= ~PARK_FLAGS_SPRITES_INITIALISED;

            scenarioOptions.guestInitialCash = std::clamp(scenarioOptions.guestInitialCash, 10.00_GBP, kMaxEntranceFee);
            scenarioOptions.initialCash = std::min<money64>(scenarioOptions.initialCash, 100000);
            FinanceResetCashToInitial();

            park.bankLoan = std::clamp<money64>(park.bankLoan, 0.00_GBP, 5000000.00_GBP);
            park.maxBankLoan = std::clamp<money64>(park.maxBankLoan, 0.00_GBP, 5000000.00_GBP);
            park.bankLoanInterestRate = std::clamp<uint8_t>(park.bankLoanInterestRate, 5, kMaxBankLoanInterestRate);
        }

        Weather::reset();

        News::InitQueue(gameState);
    }

    /**
     *
     *  rct2: 0x0067009A
     */
    void OpenWindowsForCurrentStep()
    {
        if (!isInEditorMode())
        {
            return;
        }

        auto* windowMgr = Ui::GetWindowManager();

        switch (getGameState().editorStep)
        {
            case EditorStep::objectSelection:
                if (windowMgr->FindByClass(WindowClass::editorObjectSelection) != nullptr)
                {
                    return;
                }

                if (windowMgr->FindByClass(WindowClass::installTrack) != nullptr)
                {
                    return;
                }

                if (gLegacyScene == LegacyScene::trackDesignsManager)
                {
                    ObjectManagerUnloadAllObjects();
                }

                ContextOpenWindow(WindowClass::editorObjectSelection);
                break;
            case EditorStep::inventionsListSetUp:
                if (windowMgr->FindByClass(WindowClass::editorInventionList) != nullptr)
                {
                    return;
                }

                ContextOpenWindow(WindowClass::editorInventionList);
                break;
            case EditorStep::optionsSelection:
            case EditorStep::objectiveSelection:
            case EditorStep::scenarioDetails:
                if (windowMgr->FindByClass(WindowClass::editorScenarioOptions) != nullptr)
                {
                    return;
                }

                ContextOpenWindow(WindowClass::editorScenarioOptions);
                break;
            case EditorStep::landscapeEditor:
            case EditorStep::saveScenario:
            case EditorStep::rollerCoasterDesigner:
            case EditorStep::designsManager:
            case EditorStep::invalid:
                break;
        }
    }

    static void FinaliseMainView()
    {
        auto windowManager = Ui::GetWindowManager();
        auto& gameState = getGameState();
        windowManager->SetMainView(gameState.savedView, gameState.savedViewZoom, gameState.savedViewRotation);

        ResetAllSpriteQuadrantPlacements();

        windowManager->BroadcastIntent(Intent(INTENT_ACTION_SET_DEFAULT_SCENERY_CONFIG));
        windowManager->BroadcastIntent(Intent(INTENT_ACTION_REFRESH_NEW_RIDES));
        windowManager->BroadcastIntent(Intent(INTENT_ACTION_CLEAR_TILE_INSPECTOR_CLIPBOARD));

        gWindowUpdateTicks = 0;
        LoadPalette();
    }

    /**
     *
     *  rct2: 0x006AB9B8
     */
    std::pair<ObjectType, StringId> CheckObjectSelection()
    {
        constexpr std::pair<ObjectType, StringId> kBasicCheckPairs[] = {
            { ObjectType::ride, STR_AT_LEAST_ONE_RIDE_OBJECT_MUST_BE_SELECTED },
            { ObjectType::station, STR_AT_LEAST_ONE_STATION_OBJECT_MUST_BE_SELECTED },
            { ObjectType::terrainSurface, STR_AT_LEAST_ONE_TERRAIN_SURFACE_OBJECT_MUST_BE_SELECTED },
            { ObjectType::terrainEdge, STR_AT_LEAST_ONE_TERRAIN_EDGE_OBJECT_MUST_BE_SELECTED },
        };

        for (auto& pair : kBasicCheckPairs)
        {
            if (!EditorCheckObjectGroupAtLeastOneSelected(pair.first))
            {
                return { pair.first, pair.second };
            }
        }

        // No checks beyond this point apply to the track designer or track designs manager.
        const bool isTrackDesignerManager = isInTrackDesignerOrManager();
        if (isTrackDesignerManager)
        {
            return { ObjectType::none, kStringIdNone };
        }

        if (!EditorCheckObjectGroupAtLeastOneSurfaceSelected(false))
        {
            return { ObjectType::footpathSurface, STR_AT_LEAST_ONE_FOOTPATH_NON_QUEUE_SURFACE_OBJECT_MUST_BE_SELECTED };
        }
        if (!EditorCheckObjectGroupAtLeastOneSurfaceSelected(true))
        {
            return { ObjectType::footpathSurface, STR_AT_LEAST_ONE_FOOTPATH_QUEUE_SURFACE_OBJECT_MUST_BE_SELECTED };
        }

        constexpr std::pair<ObjectType, StringId> kParkCheckPairs[] = {
            { ObjectType::footpathRailings, STR_AT_LEAST_ONE_FOOTPATH_RAILING_OBJECT_MUST_BE_SELECTED },
            { ObjectType::parkEntrance, STR_PARK_ENTRANCE_TYPE_MUST_BE_SELECTED },
            { ObjectType::water, STR_WATER_TYPE_MUST_BE_SELECTED },
            { ObjectType::peepNames, STR_AT_LEAST_ONE_PEEP_NAMES_OBJECT_MUST_BE_SELECTED },
        };

        for (auto& pair : kParkCheckPairs)
        {
            if (!EditorCheckObjectGroupAtLeastOneSelected(pair.first))
            {
                return { pair.first, pair.second };
            }
        }

        using OpenRCT2::AnimationPeepType;
        constexpr std::pair<AnimationPeepType, StringId> kPeepCheckPairs[] = {
            { AnimationPeepType::guest, STR_AT_LEAST_ONE_GUEST_PEEP_ANIMATIONS_OBJECT_MUST_BE_SELECTED },
            { AnimationPeepType::handyman, STR_AT_LEAST_ONE_HANDYMAN_PEEP_ANIMATIONS_OBJECT_MUST_BE_SELECTED },
            { AnimationPeepType::mechanic, STR_AT_LEAST_ONE_MECHANIC_PEEP_ANIMATIONS_OBJECT_MUST_BE_SELECTED },
            { AnimationPeepType::security, STR_AT_LEAST_ONE_SECURITY_PEEP_ANIMATIONS_OBJECT_MUST_BE_SELECTED },
            { AnimationPeepType::entertainer, STR_AT_LEAST_ONE_ENTERTAINER_PEEP_ANIMATIONS_OBJECT_MUST_BE_SELECTED },
        };

        for (auto& pair : kPeepCheckPairs)
        {
            if (!EditorCheckObjectGroupAtLeastOneOfPeepTypeSelected(EnumValue(pair.first)))
            {
                return { ObjectType::peepAnimations, pair.second };
            }
        }

        return { ObjectType::none, kStringIdNone };
    }

    /**
     *
     *  rct2: 0x0066FEAC
     */
    ResultWithMessage CheckPark()
    {
        auto& gameState = getGameState();
        auto& park = gameState.park;
        int32_t parkSize = Park::UpdateSize(park);
        if (parkSize == 0)
        {
            return { false, STR_PARK_MUST_OWN_SOME_LAND };
        }

        if (gameState.park.entrances.empty())
        {
            return { false, STR_NO_PARK_ENTRANCES };
        }

        for (const auto& parkEntrance : gameState.park.entrances)
        {
            int32_t direction = DirectionReverse(parkEntrance.direction);

            switch (FootpathIsConnectedToMapEdge(parkEntrance, direction, 0))
            {
                case FOOTPATH_SEARCH_NOT_FOUND:
                    return { false, STR_PARK_ENTRANCE_WRONG_DIRECTION_OR_NO_PATH };
                case FOOTPATH_SEARCH_INCOMPLETE:
                case FOOTPATH_SEARCH_TOO_COMPLEX:
                    return { false, STR_PARK_ENTRANCE_PATH_INCOMPLETE_OR_COMPLEX };
                case FOOTPATH_SEARCH_SUCCESS:
                    // Run the search again and unown the path
                    FootpathIsConnectedToMapEdge(parkEntrance, direction, (1 << 5));
                    break;
            }
        }

        if (gameState.peepSpawns.empty())
        {
            return { false, STR_PEEP_SPAWNS_NOT_SET };
        }

        return { true, kStringIdNone };
    }

    uint8_t GetSelectedObjectFlags(ObjectType objectType, size_t index)
    {
        uint8_t result = 0;
        auto& list = _editorSelectedObjectFlags[EnumValue(objectType)];
        if (list.size() > index)
        {
            result = list[index];
        }
        return result;
    }

    void ClearSelectedObject(ObjectType objectType, size_t index, uint32_t flags)
    {
        auto& list = _editorSelectedObjectFlags[EnumValue(objectType)];
        if (list.size() <= index)
        {
            list.resize(index + 1);
        }
        list[index] &= ~flags;
    }

    void SetSelectedObject(ObjectType objectType, size_t index, uint32_t flags)
    {
        if (index != kObjectEntryIndexNull)
        {
            assert(static_cast<size_t>(objectType) < getObjectEntryGroupCount(ObjectType::paths));
            auto& list = _editorSelectedObjectFlags[EnumValue(objectType)];
            if (list.size() <= index)
            {
                list.resize(index + 1);
            }
            list[index] |= flags;
        }
    }
} // namespace OpenRCT2::Editor

void EditorOpenWindowsForCurrentStep()
{
    Editor::OpenWindowsForCurrentStep();
}
