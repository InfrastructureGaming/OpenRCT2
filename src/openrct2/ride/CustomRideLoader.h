/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

namespace OpenRCT2
{
    struct IPlatformEnvironment;

    namespace CustomRideLoader
    {
        // Scans [UserData]/custom_rides/ for manifest.json packages and registers
        // each valid one with RideTypeRegistry. Called once at startup.
        void LoadAll(const IPlatformEnvironment& env);
    } // namespace CustomRideLoader

} // namespace OpenRCT2
