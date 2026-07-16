/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/StringTypes.h"
#include "ConfigTypes.h"

#include <atomic>

// windows.h defines an interface keyword
#ifdef interface
    #undef interface
#endif

enum class RideInspection : uint8_t;

namespace OpenRCT2::Config
{
    struct General
    {
        // Paths
        u8string rct1Path;
        u8string rct2Path;

        // Display
        int32_t defaultDisplay;
        int32_t windowWidth;
        int32_t windowHeight;
        int32_t fullscreenMode;
        int32_t fullscreenWidth;
        int32_t fullscreenHeight;
        float windowScale;
        bool inferDisplayDPI;
        ::DrawingEngine drawingEngine;
        bool uncapFPS;
        bool useVSync;
        bool showFPS;
        std::atomic_uint8_t multiThreading;
        bool minimizeFullscreenFocusLoss;
        bool disableScreensaver;

        // Map rendering
        bool landscapeSmoothing;
        bool alwaysShowGridlines;
        VirtualFloorStyles virtualFloorStyle;
        bool dayNightCycle;
        bool enableLightFx;
        bool enableLightFxForVehicles;
        bool upperCaseBanners;
        bool renderWeatherEffects;
        bool renderWeatherGloom;
        bool disableLightningEffect;
        bool showGuestPurchases;
        bool transparentScreenshot;
        bool transparentWater;

        bool invisibleRides;
        bool invisibleVehicles;
        bool invisibleTrees;
        bool invisibleScenery;
        bool invisiblePaths;
        bool invisibleSupports;

        // Localisation
        int32_t language;
        MeasurementFormat measurementFormat;
        TemperatureUnit temperatureFormat;
        bool showHeightAsUnits;
        int32_t dateFormat;
        CurrencyType currencyFormat;
        int32_t customCurrencyRate;
        CurrencyAffix customCurrencyAffix;
        u8string customCurrencySymbol;

        // Controls
        bool edgeScrolling;
        int32_t edgeScrollingSpeed;
        bool trapCursor;
        bool invertViewportDrag;
        bool zoomToCursor;

        // Gamepad
        int32_t gamepadDeadzone;
        float gamepadSensitivity;

        // Miscellaneous
        bool playIntro;
        int32_t windowSnapProximity;
        bool savePluginData;
        bool debuggingTools;
        int32_t autosaveFrequency;
        int32_t autosaveAmount;
        bool autoStaffPlacement;
        bool handymenMowByDefault;
        bool autoOpenShops;
        RideInspection defaultInspectionInterval;
        int32_t windowLimit;
        bool scenarioUnlockingEnabled;
        bool scenarioHideMegaPark;
        bool showRealNamesOfGuests;
        bool showRealNamesOfStaff;
        bool allowEarlyCompletion;
        u8string assetPackOrder;
        u8string enabledAssetPacks;

        // Loading and saving
        bool confirmationPrompt;
        FileBrowserSort loadSaveSort;
        u8string lastSaveGameDirectory;
        u8string lastSaveLandscapeDirectory;
        u8string lastSaveScenarioDirectory;
        u8string lastSaveTrackDirectory;
        u8string lastRunVersion;
        bool useNativeBrowseDialog;
        int64_t lastVersionCheckTime;
        int16_t fileBrowserWidth;
        int16_t fileBrowserHeight;
        bool fileBrowserShowSizeColumn;
        bool fileBrowserShowDateColumn;
        ParkPreviewPref fileBrowserPreviewType;
    };

    struct Interface
    {
        bool toolbarButtonsCentred;
        bool toolbarShowFinances;
        bool toolbarShowResearch;
        bool toolbarShowCheats;
        bool toolbarShowNews;
        bool toolbarShowMute;
        bool toolbarShowChat;
        bool toolbarShowZoom;
        bool toolbarShowRotateAnticlockwise;
        bool consoleSmallFont;
        bool randomTitleSequence;
        u8string currentThemePreset;
        u8string currentTitleSequencePreset;
        int32_t objectSelectionFilterFlags;
        int32_t scenarioSelectLastTab;
        bool scenarioPreviewScreenshots;
        bool listRideVehiclesSeparately;
        bool windowButtonsOnTheLeft;
        bool enlargedUi;
        bool touchEnhancements;
    };

    struct Sound
    {
        u8string device;
        bool masterSoundEnabled;
        uint8_t masterVolume;
        TitleMusicKind titleMusic;
        bool soundEnabled;
        uint8_t soundVolume;
        bool rideMusicEnabled;
        uint8_t rideMusicVolume;
        bool audioFocus;
    };

    struct Network
    {
        u8string playerName;
        int32_t defaultPort;
        u8string listenAddress;
        u8string defaultPassword;
        bool stayConnected;
        bool advertise;
        u8string advertiseAddress;
        int32_t maxplayers;
        u8string serverName;
        u8string serverDescription;
        u8string serverGreeting;
        u8string masterServerUrl;
        u8string providerName;
        u8string providerEmail;
        u8string providerWebsite;
        bool knownKeysOnly;
        bool logChat;
        bool logServerActions;
        bool pauseServerIfNoClients;
        bool desyncDebugging;
    };

    struct Notification
    {
        bool parkAward;
        bool parkMarketingCampaignFinished;
        bool parkWarnings;
        bool parkRatingWarnings;
        bool rideBrokenDown;
        bool rideCrashed;
        bool rideCasualties;
        bool rideWarnings;
        bool rideResearched;
        bool rideStalledVehicles;
        bool guestWarnings;
        bool guestLeftPark;
        bool guestQueuingForRide;
        bool guestOnRide;
        bool guestLeftRide;
        bool guestBoughtItem;
        bool guestUsedFacility;
        bool guestDied;
    };

    struct Font
    {
        u8string fileName;
        u8string fontName;
        int32_t offsetX;
        int32_t offsetY;
        int32_t sizeTiny;
        int32_t sizeSmall;
        int32_t sizeMedium;
        int32_t sizeBig;
        int32_t heightTiny;
        int32_t heightSmall;
        int32_t heightMedium;
        int32_t heightBig;
        bool enableHinting;
        int32_t hintingThreshold;
    };

    struct Plugin
    {
        bool enableHotReloading;
        u8string allowedHosts;
    };

    // Opt-in adjustments to guest decision-making and behaviour (our "Updated Guest Logic" subsystem,
    // Options > Miscellaneous > Guest Logic). Every field defaults to a value that reproduces vanilla
    // behaviour, so the whole group is a no-op until the player dials one in.
    struct GuestLogic
    {
        // Multiplier on how long guests will tolerate a queue before complaining ("I've been queuing
        // for ages") and eventually giving up. 1.0 = vanilla; higher = more patient, for the longer
        // ride durations that realistic-scale custom rides run.
        float queueToleranceMultiplier;

        // When choosing their next ride, guests normally pick the single most exciting one they can
        // reach, park-wide - so a distant coaster beats every nearby gentle ride. This discounts a
        // ride's excitement by how far the guest would have to travel to it (rating-hundredths lost
        // per tile of Manhattan distance). 0.0 = vanilla (pure max-excitement); higher lets a nearby
        // flat ride out-score a far-off coaster, spreading guests across the park and cutting the
        // long treks that strand guests mid-journey.
        float rideChoiceDistanceWeight;

        // Amount by which to lower a newly-spawned guest's natural minimum-intensity preference. Guests
        // spawn with an intensity floor (roughly 0-4) that makes many of them reject anything gentle;
        // this softens that floor so more guests are willing to ride calmer attractions. Only lowers the
        // floor (never the ceiling), so nervous guests still avoid coasters. 0 = vanilla; range 0-3.
        // Applies only to guests spawned while the setting is active.
        uint8_t intensityFloorReduction;

        // Vanilla guests will never re-ride a ride they have already been on - once ridden, a ride is
        // permanently struck from their consideration. In a park of custom flat/gentle rides that means
        // a guest rides each one once and then has nothing left to do. This replaces that hard exclusion
        // with a flat desirability penalty (in excitement points) applied to already-ridden rides, so a
        // guest may choose to re-ride a nearby ride when nothing fresh is nearby, while still preferring
        // rides they haven't been on. 0.0 = vanilla (already-ridden rides are excluded outright); higher
        // = a bigger penalty, i.e. more reluctant to repeat. Best paired with ride distance weighting so
        // repeats stay local rather than sending guests back across the park to a favourite coaster.
        float rideMemoryPenalty;

        // Separate from rideMemoryPenalty (which governs choosing an already-ridden ride as a fresh
        // destination): this governs the SPONTANEOUS re-ride - a happy guest standing by a ride deciding
        // to go straight round again. Vanilla gates that on RtdFlag::guestsWillRideAgain, which coasters
        // and a few circuit rides have but flat/spinning/most gentle rides do not, so guests never hop
        // back on your flat rides no matter how much they enjoyed them. When true, the flag requirement
        // is waived for rides that lack it, so any ride becomes eligible for a spontaneous repeat - but
        // only through the unchanged happiness/energy/nausea/hunger/thirst/randomised gates below it, so
        // repeats still happen only when the guest is genuinely having a great time. false = vanilla.
        bool allowRepeatRides;

        // Vanilla only lets a guest look for a toilet/food/drink stall within a 10-tile box, and only when
        // a random thought roll (fired roughly every 1000 ticks) happens to pick that need - so in a big or
        // busy park a desperate guest can walk right past the problem and never divert, flooding the park
        // with "I need the toilet" and tanking the rating. When true, a guest whose need crosses a critical
        // threshold is redirected to the nearest matching facility PARK-WIDE and deterministically (no dice
        // roll), even mid-trip to a ride, checked every 128-tick guest update. Toilet takes priority (the
        // worst rating-killer), then hunger, then thirst; hunger/thirst are skipped if the guest is already
        // carrying food/drink. false = vanilla. Note a park-wide facility search is actually CHEAPER than
        // the 10-tile tile scan, so this is not a performance cost.
        bool needsInterrupt;

        // The park rating counts every guest who is leaving the park AND whose guestIsLostCountdown has
        // dropped below 90 as "lost", with an UNBOUNDED penalty of (lost-25)*7. But that countdown just
        // decrements on a timer while a guest walks to the exit (checkCantFindExit) - it never checks
        // whether the guest is actually making progress. So in a large park EVERY departing guest is
        // eventually tallied as lost simply because the walk to a gate takes a while, which can drive the
        // rating to zero even though nobody is truly stuck. When true, a leaving guest that the pathfinder
        // successfully routes toward its chosen exit (a valid directed step in GuestPathFindParkEntrance
        // Leaving) has its countdown refreshed, so only guests the pathfinder genuinely CANNOT route - a
        // real dead-end or maze - ever count as lost. This corrects the rating's input, not the rating
        // formula (which stays vanilla). false = vanilla (in-transit leavers count as lost).
        bool smartLeaverLostTracking;
    };

    struct Config
    {
        Config() = default;

        // Prevent accidental copies
        Config(const Config&) = delete;

        General general;
        Interface interface;
        Sound sound;
        Network network;
        Notification notifications;
        Font fonts;
        Plugin plugin;
        GuestLogic guestLogic;
    };

    Config& Get();
    bool OpenFromPath(u8string_view path);
    bool SaveToPath(u8string_view path);
    u8string GetDefaultPath();
    bool SetDefaults();
    bool Save();
    bool FindOrBrowseInstallDirectory();
} // namespace OpenRCT2::Config
