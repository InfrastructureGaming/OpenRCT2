/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

extern const uint8_t* kTwistTimeToSpriteMaps[];
extern const uint8_t* kEnterpriseTimeToSpriteMaps[];
extern const uint8_t* kMerryGoRoundTimeToSpriteMaps[];

struct TopSpinTimeToSpriteMap
{
    uint8_t arm_rotation;
    uint8_t bank_rotation;
};

extern const TopSpinTimeToSpriteMap* kTopSpinTimeToSpriteMaps[];

// A phase: a time-indexed sequence of raw flatRideAnimationFrame values into the
// ride's combined FlatRideRotationDescriptor::FramesPerDir sprite sheet, terminated
// by 0xFFFF. Used by Vehicle::UpdateFlatRideGeneric() for rides whose
// FlatRideRotationDescriptor::Programs is non-null.
struct FlatRideAnimationPhase
{
    const uint16_t* TimeToSpriteMap = nullptr;
    uint8_t NextPhase = 0;
    bool RepeatUntilRotationsComplete = false;
    bool IsFinalPhase = false;
    // Zero NumRotations when entering this phase, giving it an independent repeat
    // budget against ride.rotations. Needed when a program has more than one
    // RepeatUntilRotationsComplete phase (NumRotations is otherwise a single counter
    // shared across the whole program).
    bool ResetRotationsOnEntry = false;
};

// A selectable animation program: an ordered/looping graph of phases.
struct FlatRideAnimationProgram
{
    const FlatRideAnimationPhase* Phases = nullptr;
    uint8_t NumPhases = 0;
};


extern const uint8_t MotionSimulatorTimeToSpriteMap[];
extern const int32_t MotionSimulatorTimeToSpriteMapCount;

extern const uint8_t kSpaceRingsTimeToSpriteMap[];

/** rct2: 0x0099F9D0 */
extern const int8_t* kSwingingTimeToSpriteMaps[];
