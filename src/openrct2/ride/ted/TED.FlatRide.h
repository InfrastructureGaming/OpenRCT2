/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../localisation/StringIds.h"
#include "TrackElementDescriptor.h"

#include <algorithm>

namespace OpenRCT2::TrackMetadata
{
    using PS = PaintSegment;

    static constexpr SequenceDescriptor kFlatTrack1x4ASeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin,
                   SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4ASeq1 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4ASeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4ASeq3 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, { ClearanceFlag::flag1 } },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x2Seq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin,
                   SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x2Seq1 = {
        .clearance = { 0, 32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x2Seq2 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x2Seq3 = {
        .clearance = { 32, 32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin,
                   SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq1 = {
        .clearance = { 0, 32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq2 = {
        .clearance = { 0, 64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq3 = {
        .clearance = { 0, 96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq4 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq5 = {
        .clearance = { 32, 32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq6 = {
        .clearance = { 32, 64, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq7 = {
        .clearance = { 32, 96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq8 = {
        .clearance = { 64, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq9 = {
        .clearance = { 64, 32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq10 = {
        .clearance = { 64, 64, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq11 = {
        .clearance = { 64, 96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq12 = {
        .clearance = { 96, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq13 = {
        .clearance = { 96, 32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq14 = {
        .clearance = { 96, 64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq15 = {
        .clearance = { 96, 96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    // 5×5 flat ride base — 25 tiles. All clearance offsets are relative to seq12 (trackOrigin),
    // the geometric center of the platform. Entrance arches connect to the NE edge (seq0–seq4);
    // guests walk from the arch to the center before being placed on the vehicle.
    // Offsets are symmetric: range (–64,–64) to (+64,+64) relative to seq12. This symmetry is
    // load-bearing: it ensures that the 25 tiles' collective dirty rects cover the full sprite
    // extent during placement-ghost invalidation. An asymmetric origin (e.g., NE edge) causes
    // ghost sprites to persist on screen because the sprite extends beyond the dirty coverage.
    static constexpr SequenceDescriptor kFlatTrack5x5Seq0 = {
        .clearance = { -64, -64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
    };
    static constexpr SequenceDescriptor kFlatTrack5x5Seq1 = {
        .clearance = { -64, -32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE },
    };
    static constexpr SequenceDescriptor kFlatTrack5x5Seq2 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE },
    };
    static constexpr SequenceDescriptor kFlatTrack5x5Seq3 = {
        .clearance = { -64, 32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE },
    };
    static constexpr SequenceDescriptor kFlatTrack5x5Seq4 = {
        .clearance = { -64, 64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::hasHeightMarker },
    };
    static constexpr SequenceDescriptor kFlatTrack5x5Seq5 = {
        .clearance = { -32, -64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNW },
    };
    static constexpr SequenceDescriptor kFlatTrack5x5Seq6 = {
        .clearance = { -32, -32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack5x5Seq7 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack5x5Seq8 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack5x5Seq9 = {
        .clearance = { -32, 64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE },
    };
    static constexpr SequenceDescriptor kFlatTrack5x5Seq10 = {
        .clearance = { 0, -64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNW },
    };
    static constexpr SequenceDescriptor kFlatTrack5x5Seq11 = {
        .clearance = { 0, -32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack5x5Seq12 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::trackOrigin },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack5x5Seq13 = {
        .clearance = { 0, 32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack5x5Seq14 = {
        .clearance = { 0, 64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE },
    };
    static constexpr SequenceDescriptor kFlatTrack5x5Seq15 = {
        .clearance = { 32, -64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNW },
    };
    static constexpr SequenceDescriptor kFlatTrack5x5Seq16 = {
        .clearance = { 32, -32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack5x5Seq17 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack5x5Seq18 = {
        .clearance = { 32, 32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack5x5Seq19 = {
        .clearance = { 32, 64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE },
    };
    static constexpr SequenceDescriptor kFlatTrack5x5Seq20 = {
        .clearance = { 64, -64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
    };
    static constexpr SequenceDescriptor kFlatTrack5x5Seq21 = {
        .clearance = { 64, -32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW },
    };
    static constexpr SequenceDescriptor kFlatTrack5x5Seq22 = {
        .clearance = { 64, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW },
    };
    static constexpr SequenceDescriptor kFlatTrack5x5Seq23 = {
        .clearance = { 64, 32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW },
    };
    static constexpr SequenceDescriptor kFlatTrack5x5Seq24 = {
        .clearance = { 64, 64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW, SequenceFlag::hasHeightMarker },
    };

    // 6×6 flat ride base — 36 tiles. Direct generalisation of the 5×5 grid above: rows/cols
    // 0-5, clearance = ((row-2)*32, (col-2)*32). seq14 (row2,col2) is the cursor/trackOrigin
    // tile. Unlike 5×5, this grid is even and has no true geometric center — seq14 is the
    // tile nearest center, one tile-width off from the actual center point. The paint
    // function (PaintFlatRideGenericAnySize, GenericFlatRide.cpp) computes per-tile
    // rotation/edges analytically from clearance rather than via a fixed lookup table.
    static constexpr SequenceDescriptor kFlatTrack6x6Seq0 = {
        .clearance = { -64, -64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq1 = {
        .clearance = { -64, -32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq2 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq3 = {
        .clearance = { -64, 32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq4 = {
        .clearance = { -64, 64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq5 = {
        .clearance = { -64, 96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::hasHeightMarker },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq6 = {
        .clearance = { -32, -64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNW },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq7 = {
        .clearance = { -32, -32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq8 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq9 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq10 = {
        .clearance = { -32, 64, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq11 = {
        .clearance = { -32, 96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq12 = {
        .clearance = { 0, -64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNW },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq13 = {
        .clearance = { 0, -32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq14 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::trackOrigin },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq15 = {
        .clearance = { 0, 32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq16 = {
        .clearance = { 0, 64, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq17 = {
        .clearance = { 0, 96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq18 = {
        .clearance = { 32, -64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNW },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq19 = {
        .clearance = { 32, -32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq20 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq21 = {
        .clearance = { 32, 32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq22 = {
        .clearance = { 32, 64, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq23 = {
        .clearance = { 32, 96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq24 = {
        .clearance = { 64, -64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNW },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq25 = {
        .clearance = { 64, -32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq26 = {
        .clearance = { 64, 0, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq27 = {
        .clearance = { 64, 32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq28 = {
        .clearance = { 64, 64, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq29 = {
        .clearance = { 64, 96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq30 = {
        .clearance = { 96, -64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq31 = {
        .clearance = { 96, -32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq32 = {
        .clearance = { 96, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq33 = {
        .clearance = { 96, 32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq34 = {
        .clearance = { 96, 64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW },
    };
    static constexpr SequenceDescriptor kFlatTrack6x6Seq35 = {
        .clearance = { 96, 96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW, SequenceFlag::hasHeightMarker },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin,
                   SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq1 = {
        .clearance = { 0, 32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq2 = {
        .clearance = { 0, 64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq3 = {
        .clearance = { 0, 96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq4 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq5 = {
        .clearance = { 32, 32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq6 = {
        .clearance = { 32, 64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq7 = {
        .clearance = { 32, 96, 0, 0, { 0b1111, 0 }, { ClearanceFlag::flag1 } },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x5Seq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x5Seq1 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x5Seq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x5Seq3 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x5Seq4 = {
        .clearance = { 64, 0, 0, 0, { 0b1111, 0 }, { ClearanceFlag::flag1 } },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x1ASeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::trackOrigin, SequenceFlag::connectsToPath,
                   SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4BSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::trackOrigin, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4BSeq1 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4BSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4BSeq3 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, { ClearanceFlag::flag1 } },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x1BSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin, SequenceFlag::connectsToPath,
                   SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4CSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4CSeq1 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW,
                   SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4CSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4CSeq3 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, { ClearanceFlag::flag1 } },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW, SequenceFlag::entranceConnectionNW,
                   SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::trackOrigin },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq1 = {
        .clearance = { -32, -32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq4 = {
        .clearance = { 0, -32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq5 = {
        .clearance = { 0, 32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq6 = {
        .clearance = { 32, -32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq7 = {
        .clearance = { 32, 32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq8 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, { ClearanceFlag::flag1 } },
        .flags = { SequenceFlag::entranceConnectionSW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    constexpr auto kTEDFlatTrack1x4A = TrackElementDescriptor{
        .coordinates = { 0, 2, 0, 0, 0, 32 },
        .pieceLength = 0,
        .curveChain = { TrackElemType::none, TrackElemType::none },
        .priceModifier = 262144,
        .mirrorElement = TrackElemType::flatTrack1x4A,
        .flags = {},
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 4, { kFlatTrack1x4ASeq0, kFlatTrack1x4ASeq1, kFlatTrack1x4ASeq2, kFlatTrack1x4ASeq3 } },
    };

    constexpr auto kTEDFlatTrack2x2 = TrackElementDescriptor{
        .description = STR_BASE_SIZE_2_X_2,
        .coordinates = { 0, 2, 0, 0, 0, 32 },
        .pieceLength = 0,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 262144,
        .mirrorElement = TrackElemType::flatTrack2x2,
        .flags = {},
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 4, { kFlatTrack2x2Seq0, kFlatTrack2x2Seq1, kFlatTrack2x2Seq2, kFlatTrack2x2Seq3 } },
    };

    constexpr auto kTEDFlatTrack4x4 = TrackElementDescriptor{
        .description = STR_BASE_SIZE_4_X_4,
        .coordinates = { 0, 2, 0, 0, 0, 32 },
        .pieceLength = 0,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 1048576, // TrackElemType::flatTrack4x4
        .mirrorElement = TrackElemType::flatTrack4x4,
        .flags = {},
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 16,
                          { kFlatTrack4x4Seq0, kFlatTrack4x4Seq1, kFlatTrack4x4Seq2, kFlatTrack4x4Seq3, kFlatTrack4x4Seq4,
                            kFlatTrack4x4Seq5, kFlatTrack4x4Seq6, kFlatTrack4x4Seq7, kFlatTrack4x4Seq8, kFlatTrack4x4Seq9,
                            kFlatTrack4x4Seq10, kFlatTrack4x4Seq11, kFlatTrack4x4Seq12, kFlatTrack4x4Seq13, kFlatTrack4x4Seq14,
                            kFlatTrack4x4Seq15 } },
    };

    // Must be static const (not constexpr) — 351 entries × 25-slot SequenceTables exceeds
    // MSVC's constexpr evaluation step limit when kTrackElementDescriptors is constexpr.
    static const TrackElementDescriptor kTEDFlatTrack5x5 = TrackElementDescriptor{
        .description = STR_BASE_SIZE_5_X_5,
        .coordinates = { 0, 2, 0, 0, 0, 32 },
        .pieceLength = 0,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 1638400,
        .mirrorElement = TrackElemType::flatTrack5x5,
        .flags = {},
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        // IMPORTANT: sequences[0] MUST be the cursor tile (clearance 0,0) with trackOrigin flag.
        // trackBlockGetNextFromZero() hard-codes GetSequenceIndex()==0 as its match criterion;
        // it searches at the cursor position for index-0. If index-0 is not the cursor tile,
        // ghost removal silently no-ops and ghosts accumulate on every cursor move.
        // All other sequence indices affect kTrackMap5x5 in TrackPaint.h — keep them in sync.
        .sequenceData = { 25,
                          { kFlatTrack5x5Seq12,                                                    // idx  0: center (0,0) = cursor tile
                            kFlatTrack5x5Seq0,  kFlatTrack5x5Seq1,  kFlatTrack5x5Seq2,            // idx  1-3 : NE row
                            kFlatTrack5x5Seq3,  kFlatTrack5x5Seq4,                                // idx  4-5 : NE row cont.
                            kFlatTrack5x5Seq5,  kFlatTrack5x5Seq6,  kFlatTrack5x5Seq7,            // idx  6-8 : row 2
                            kFlatTrack5x5Seq8,  kFlatTrack5x5Seq9,                                // idx  9-10: row 2 cont.
                            kFlatTrack5x5Seq10, kFlatTrack5x5Seq11,                               // idx 11-12: middle row left half
                            kFlatTrack5x5Seq13, kFlatTrack5x5Seq14,                               // idx 13-14: middle row right half
                            kFlatTrack5x5Seq15, kFlatTrack5x5Seq16, kFlatTrack5x5Seq17,           // idx 15-17: row 4
                            kFlatTrack5x5Seq18, kFlatTrack5x5Seq19,                               // idx 18-19: row 4 cont.
                            kFlatTrack5x5Seq20, kFlatTrack5x5Seq21, kFlatTrack5x5Seq22,           // idx 20-22: SW row
                            kFlatTrack5x5Seq23, kFlatTrack5x5Seq24 } },                           // idx 23-24: SW row cont.
    };

    // Must be static const (not constexpr) — see kTEDFlatTrack5x5 comment above; the same
    // MSVC constexpr-evaluation-depth limit applies here with a 36-slot SequenceTable.
    static const TrackElementDescriptor kTEDFlatTrack6x6 = TrackElementDescriptor{
        .description = STR_BASE_SIZE_6_X_6,
        .coordinates = { 0, 2, 0, 0, 0, 32 },
        .pieceLength = 0,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 1638400,
        .mirrorElement = TrackElemType::flatTrack6x6,
        .flags = {},
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        // IMPORTANT: sequences[0] MUST be the cursor tile (clearance 0,0) with trackOrigin flag —
        // see the ghost-removal explanation on kTEDFlatTrack5x5. Unlike 5x5, the remaining
        // sequence ordering carries no lookup-table dependency (PaintFlatRideGenericAnySize
        // computes everything analytically from each sequence's own clearance), so idx 1-35
        // simply follow row-major order with idx 14 (the cursor tile) removed.
        .sequenceData = { 36,
                          { kFlatTrack6x6Seq14,                                                     // idx  0: (row2,col2) = cursor tile
                            kFlatTrack6x6Seq0,  kFlatTrack6x6Seq1,  kFlatTrack6x6Seq2,
                            kFlatTrack6x6Seq3,  kFlatTrack6x6Seq4,  kFlatTrack6x6Seq5,              // idx  1-6 : row 0
                            kFlatTrack6x6Seq6,  kFlatTrack6x6Seq7,  kFlatTrack6x6Seq8,
                            kFlatTrack6x6Seq9,  kFlatTrack6x6Seq10, kFlatTrack6x6Seq11,             // idx  7-12: row 1
                            kFlatTrack6x6Seq12, kFlatTrack6x6Seq13,                                 // idx 13-14: row 2 left half
                            kFlatTrack6x6Seq15, kFlatTrack6x6Seq16, kFlatTrack6x6Seq17,             // idx 15-17: row 2 right half
                            kFlatTrack6x6Seq18, kFlatTrack6x6Seq19, kFlatTrack6x6Seq20,
                            kFlatTrack6x6Seq21, kFlatTrack6x6Seq22, kFlatTrack6x6Seq23,             // idx 18-23: row 3
                            kFlatTrack6x6Seq24, kFlatTrack6x6Seq25, kFlatTrack6x6Seq26,
                            kFlatTrack6x6Seq27, kFlatTrack6x6Seq28, kFlatTrack6x6Seq29,             // idx 24-29: row 4
                            kFlatTrack6x6Seq30, kFlatTrack6x6Seq31, kFlatTrack6x6Seq32,
                            kFlatTrack6x6Seq33, kFlatTrack6x6Seq34, kFlatTrack6x6Seq35 } },         // idx 30-35: row 5
    };

    // 7×7 flat ride base — 49 tiles. Odd dimension with true geometric center at seq24 (row3,col3).
    // Clearance: rows 0-6, x = (row-3)*32 → [-96,+96]; cols 0-6, y = (col-3)*32 → [-96,+96].
    // The cursor tile (0,0) IS the geometric center — no half-tile shift needed in the paint function.
    // Paint function (PaintFlatRideGenericAnySize) computes rotation/edges analytically.
    static constexpr SequenceDescriptor kFlatTrack7x7Seq0 = {
        .clearance = { -96, -96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq1 = {
        .clearance = { -96, -64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq2 = {
        .clearance = { -96, -32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq3 = {
        .clearance = { -96, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq4 = {
        .clearance = { -96, 32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq5 = {
        .clearance = { -96, 64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq6 = {
        .clearance = { -96, 96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::hasHeightMarker },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq7 = {
        .clearance = { -64, -96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNW },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq8 = {
        .clearance = { -64, -64, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq9 = {
        .clearance = { -64, -32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq10 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq11 = {
        .clearance = { -64, 32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq12 = {
        .clearance = { -64, 64, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq13 = {
        .clearance = { -64, 96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq14 = {
        .clearance = { -32, -96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNW },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq15 = {
        .clearance = { -32, -64, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq16 = {
        .clearance = { -32, -32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq17 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq18 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq19 = {
        .clearance = { -32, 64, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq20 = {
        .clearance = { -32, 96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq21 = {
        .clearance = { 0, -96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNW },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq22 = {
        .clearance = { 0, -64, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq23 = {
        .clearance = { 0, -32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq24 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::trackOrigin },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq25 = {
        .clearance = { 0, 32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq26 = {
        .clearance = { 0, 64, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq27 = {
        .clearance = { 0, 96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq28 = {
        .clearance = { 32, -96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNW },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq29 = {
        .clearance = { 32, -64, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq30 = {
        .clearance = { 32, -32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq31 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq32 = {
        .clearance = { 32, 32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq33 = {
        .clearance = { 32, 64, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq34 = {
        .clearance = { 32, 96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq35 = {
        .clearance = { 64, -96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNW },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq36 = {
        .clearance = { 64, -64, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq37 = {
        .clearance = { 64, -32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq38 = {
        .clearance = { 64, 0, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq39 = {
        .clearance = { 64, 32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq40 = {
        .clearance = { 64, 64, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq41 = {
        .clearance = { 64, 96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq42 = {
        .clearance = { 96, -96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq43 = {
        .clearance = { 96, -64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq44 = {
        .clearance = { 96, -32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq45 = {
        .clearance = { 96, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq46 = {
        .clearance = { 96, 32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq47 = {
        .clearance = { 96, 64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW },
    };
    static constexpr SequenceDescriptor kFlatTrack7x7Seq48 = {
        .clearance = { 96, 96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW, SequenceFlag::hasHeightMarker },
    };

    // Must be static const (not constexpr) — see kTEDFlatTrack5x5 comment; same MSVC constexpr
    // depth limit applies with a 64-slot SequenceTable (kMaxSequencesPerPiece).
    static const TrackElementDescriptor kTEDFlatTrack7x7 = TrackElementDescriptor{
        .description = STR_BASE_SIZE_7_X_7,
        .coordinates = { 0, 2, 0, 0, 0, 32 },
        .pieceLength = 0,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 1638400,
        .mirrorElement = TrackElemType::flatTrack7x7,
        .flags = {},
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        // sequences[0] MUST be the cursor tile (clearance 0,0) with trackOrigin — see kTEDFlatTrack5x5.
        // The remaining indices follow row-major order with Seq24 (the cursor) removed.
        .sequenceData = { 49,
                          { kFlatTrack7x7Seq24,                                                      // idx  0: (row3,col3) = cursor tile
                            kFlatTrack7x7Seq0,  kFlatTrack7x7Seq1,  kFlatTrack7x7Seq2,
                            kFlatTrack7x7Seq3,  kFlatTrack7x7Seq4,  kFlatTrack7x7Seq5,
                            kFlatTrack7x7Seq6,                                                       // idx  1-7 : row 0
                            kFlatTrack7x7Seq7,  kFlatTrack7x7Seq8,  kFlatTrack7x7Seq9,
                            kFlatTrack7x7Seq10, kFlatTrack7x7Seq11, kFlatTrack7x7Seq12,
                            kFlatTrack7x7Seq13,                                                      // idx  8-14: row 1
                            kFlatTrack7x7Seq14, kFlatTrack7x7Seq15, kFlatTrack7x7Seq16,
                            kFlatTrack7x7Seq17, kFlatTrack7x7Seq18, kFlatTrack7x7Seq19,
                            kFlatTrack7x7Seq20,                                                      // idx 15-21: row 2
                            kFlatTrack7x7Seq21, kFlatTrack7x7Seq22, kFlatTrack7x7Seq23,             // idx 22-24: row 3 left half
                            kFlatTrack7x7Seq25, kFlatTrack7x7Seq26, kFlatTrack7x7Seq27,             // idx 25-27: row 3 right half
                            kFlatTrack7x7Seq28, kFlatTrack7x7Seq29, kFlatTrack7x7Seq30,
                            kFlatTrack7x7Seq31, kFlatTrack7x7Seq32, kFlatTrack7x7Seq33,
                            kFlatTrack7x7Seq34,                                                      // idx 28-34: row 4
                            kFlatTrack7x7Seq35, kFlatTrack7x7Seq36, kFlatTrack7x7Seq37,
                            kFlatTrack7x7Seq38, kFlatTrack7x7Seq39, kFlatTrack7x7Seq40,
                            kFlatTrack7x7Seq41,                                                      // idx 35-41: row 5
                            kFlatTrack7x7Seq42, kFlatTrack7x7Seq43, kFlatTrack7x7Seq44,
                            kFlatTrack7x7Seq45, kFlatTrack7x7Seq46, kFlatTrack7x7Seq47,
                            kFlatTrack7x7Seq48 } },                                                  // idx 42-48: row 6
    };

    // 8×8 flat ride base — 64 tiles. Even dimension; cursor at row3,col3 (0,0), one tile
    // NE/NW of the geometric center — same asymmetry as 6×6. Clearance: rows 0-7,
    // x = (row-3)*32 → [-96,+128]; cols 0-7, y = (col-3)*32 → [-96,+128].
    // The paint function (PaintFlatRideGenericAnySize) derives the equivalent of a +16
    // shift automatically from this table's own clearance range, to center the model
    // on the 8×8 plot.
    static constexpr SequenceDescriptor kFlatTrack8x8Seq0 = {
        .clearance = { -96, -96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq1 = {
        .clearance = { -96, -64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq2 = {
        .clearance = { -96, -32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq3 = {
        .clearance = { -96, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq4 = {
        .clearance = { -96, 32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq5 = {
        .clearance = { -96, 64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq6 = {
        .clearance = { -96, 96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq7 = {
        .clearance = { -96, 128, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::hasHeightMarker },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq8 = {
        .clearance = { -64, -96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNW },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq9 = {
        .clearance = { -64, -64, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq10 = {
        .clearance = { -64, -32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq11 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq12 = {
        .clearance = { -64, 32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq13 = {
        .clearance = { -64, 64, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq14 = {
        .clearance = { -64, 96, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq15 = {
        .clearance = { -64, 128, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq16 = {
        .clearance = { -32, -96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNW },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq17 = {
        .clearance = { -32, -64, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq18 = {
        .clearance = { -32, -32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq19 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq20 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq21 = {
        .clearance = { -32, 64, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq22 = {
        .clearance = { -32, 96, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq23 = {
        .clearance = { -32, 128, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq24 = {
        .clearance = { 0, -96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNW },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq25 = {
        .clearance = { 0, -64, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq26 = {
        .clearance = { 0, -32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq27 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::trackOrigin },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq28 = {
        .clearance = { 0, 32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq29 = {
        .clearance = { 0, 64, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq30 = {
        .clearance = { 0, 96, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq31 = {
        .clearance = { 0, 128, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq32 = {
        .clearance = { 32, -96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNW },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq33 = {
        .clearance = { 32, -64, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq34 = {
        .clearance = { 32, -32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq35 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq36 = {
        .clearance = { 32, 32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq37 = {
        .clearance = { 32, 64, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq38 = {
        .clearance = { 32, 96, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq39 = {
        .clearance = { 32, 128, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq40 = {
        .clearance = { 64, -96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNW },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq41 = {
        .clearance = { 64, -64, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq42 = {
        .clearance = { 64, -32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq43 = {
        .clearance = { 64, 0, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq44 = {
        .clearance = { 64, 32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq45 = {
        .clearance = { 64, 64, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq46 = {
        .clearance = { 64, 96, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq47 = {
        .clearance = { 64, 128, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq48 = {
        .clearance = { 96, -96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionNW },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq49 = {
        .clearance = { 96, -64, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq50 = {
        .clearance = { 96, -32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq51 = {
        .clearance = { 96, 0, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq52 = {
        .clearance = { 96, 32, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq53 = {
        .clearance = { 96, 64, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq54 = {
        .clearance = { 96, 96, 0, 0, { 0b1111, 0 }, {} },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq55 = {
        .clearance = { 96, 128, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq56 = {
        .clearance = { 128, -96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq57 = {
        .clearance = { 128, -64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq58 = {
        .clearance = { 128, -32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq59 = {
        .clearance = { 128, 0, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq60 = {
        .clearance = { 128, 32, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq61 = {
        .clearance = { 128, 64, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq62 = {
        .clearance = { 128, 96, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSW },
    };
    static constexpr SequenceDescriptor kFlatTrack8x8Seq63 = {
        .clearance = { 128, 128, 0, 0, { 0b1111, 0 }, {} },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW, SequenceFlag::hasHeightMarker },
    };

    // Must be static const — see kTEDFlatTrack5x5 comment.
    static const TrackElementDescriptor kTEDFlatTrack8x8 = TrackElementDescriptor{
        .description = STR_BASE_SIZE_8_X_8,
        .coordinates = { 0, 2, 0, 0, 0, 32 },
        .pieceLength = 0,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 1638400,
        .mirrorElement = TrackElemType::flatTrack8x8,
        .flags = {},
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        // sequences[0] MUST be the cursor tile (clearance 0,0) with trackOrigin — see kTEDFlatTrack5x5.
        // Even 8×8 grid; cursor at row3,col3 = Seq27. Remaining indices in row-major order with Seq27 removed.
        .sequenceData = { 64,
                          { kFlatTrack8x8Seq27,                                                      // idx  0: (row3,col3) = cursor tile
                            kFlatTrack8x8Seq0,  kFlatTrack8x8Seq1,  kFlatTrack8x8Seq2,
                            kFlatTrack8x8Seq3,  kFlatTrack8x8Seq4,  kFlatTrack8x8Seq5,
                            kFlatTrack8x8Seq6,  kFlatTrack8x8Seq7,                                  // idx  1-8 : row 0
                            kFlatTrack8x8Seq8,  kFlatTrack8x8Seq9,  kFlatTrack8x8Seq10,
                            kFlatTrack8x8Seq11, kFlatTrack8x8Seq12, kFlatTrack8x8Seq13,
                            kFlatTrack8x8Seq14, kFlatTrack8x8Seq15,                                 // idx  9-16: row 1
                            kFlatTrack8x8Seq16, kFlatTrack8x8Seq17, kFlatTrack8x8Seq18,
                            kFlatTrack8x8Seq19, kFlatTrack8x8Seq20, kFlatTrack8x8Seq21,
                            kFlatTrack8x8Seq22, kFlatTrack8x8Seq23,                                 // idx 17-24: row 2
                            kFlatTrack8x8Seq24, kFlatTrack8x8Seq25, kFlatTrack8x8Seq26,             // idx 25-27: row 3 left half
                            kFlatTrack8x8Seq28, kFlatTrack8x8Seq29, kFlatTrack8x8Seq30,
                            kFlatTrack8x8Seq31,                                                      // idx 28-31: row 3 right half
                            kFlatTrack8x8Seq32, kFlatTrack8x8Seq33, kFlatTrack8x8Seq34,
                            kFlatTrack8x8Seq35, kFlatTrack8x8Seq36, kFlatTrack8x8Seq37,
                            kFlatTrack8x8Seq38, kFlatTrack8x8Seq39,                                 // idx 32-39: row 4
                            kFlatTrack8x8Seq40, kFlatTrack8x8Seq41, kFlatTrack8x8Seq42,
                            kFlatTrack8x8Seq43, kFlatTrack8x8Seq44, kFlatTrack8x8Seq45,
                            kFlatTrack8x8Seq46, kFlatTrack8x8Seq47,                                 // idx 40-47: row 5
                            kFlatTrack8x8Seq48, kFlatTrack8x8Seq49, kFlatTrack8x8Seq50,
                            kFlatTrack8x8Seq51, kFlatTrack8x8Seq52, kFlatTrack8x8Seq53,
                            kFlatTrack8x8Seq54, kFlatTrack8x8Seq55,                                 // idx 48-55: row 6
                            kFlatTrack8x8Seq56, kFlatTrack8x8Seq57, kFlatTrack8x8Seq58,
                            kFlatTrack8x8Seq59, kFlatTrack8x8Seq60, kFlatTrack8x8Seq61,
                            kFlatTrack8x8Seq62, kFlatTrack8x8Seq63 } },                             // idx 56-63: row 7
    };

    constexpr auto kTEDFlatTrack2x4 = TrackElementDescriptor{
        .description = STR_BASE_SIZE_2_X_4,
        .coordinates = { 0, 2, 0, 0, 0, 32 },
        .pieceLength = 0,
        .curveChain = { TrackElemType::none, TrackElemType::none },
        .priceModifier = 524288,
        .mirrorElement = TrackElemType::flatTrack2x4,
        .flags = { TrackElementFlag::onlyAboveGround },
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 8,
                          { kFlatTrack2x4Seq0, kFlatTrack2x4Seq1, kFlatTrack2x4Seq2, kFlatTrack2x4Seq3, kFlatTrack2x4Seq4,
                            kFlatTrack2x4Seq5, kFlatTrack2x4Seq6, kFlatTrack2x4Seq7 } },
    };

    constexpr auto kTEDFlatTrack1x5 = TrackElementDescriptor{
        .description = STR_BASE_SIZE_5_X_1,
        .coordinates = { 0, 2, 0, 0, 0, 32 },
        .pieceLength = 0,
        .curveChain = { TrackElemType::none, TrackElemType::none },
        .priceModifier = 327680,
        .mirrorElement = TrackElemType::flatTrack1x5,
        .flags = {},
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 5,
                          { kFlatTrack1x5Seq0, kFlatTrack1x5Seq1, kFlatTrack1x5Seq2, kFlatTrack1x5Seq3, kFlatTrack1x5Seq4 } },
    };

    constexpr auto kTEDFlatTrack1x1A = TrackElementDescriptor{
        .coordinates = { 0, 2, 0, 0, 0, 32 },
        .pieceLength = 0,
        .curveChain = { TrackElemType::none, TrackElemType::none },
        .priceModifier = 65536,
        .mirrorElement = TrackElemType::flatTrack1x1A,
        .flags = {},
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kFlatTrack1x1ASeq0 } },
    };

    constexpr auto kTEDFlatTrack1x4B = TrackElementDescriptor{
        .coordinates = { 0, 2, 0, 0, 0, 32 },
        .pieceLength = 0,
        .curveChain = { TrackElemType::none, TrackElemType::none },
        .priceModifier = 262144,
        .mirrorElement = TrackElemType::flatTrack1x4B,
        .flags = {},
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 4, { kFlatTrack1x4BSeq0, kFlatTrack1x4BSeq1, kFlatTrack1x4BSeq2, kFlatTrack1x4BSeq3 } },
    };

    constexpr auto kTEDFlatTrack1x1B = TrackElementDescriptor{
        .coordinates = { 0, 2, 0, 0, 0, 32 },
        .pieceLength = 0,
        .curveChain = { TrackElemType::none, TrackElemType::none },
        .priceModifier = 65536,
        .mirrorElement = TrackElemType::flatTrack1x1B,
        .flags = {},
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kFlatTrack1x1BSeq0 } },
    };

    constexpr auto kTEDFlatTrack1x4C = TrackElementDescriptor{
        .coordinates = { 0, 2, 0, 0, 0, 32 },
        .pieceLength = 0,
        .curveChain = { TrackElemType::none, TrackElemType::none },
        .priceModifier = 262144,
        .mirrorElement = TrackElemType::flatTrack1x4C,
        .flags = {},
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, -40 },
        .sequenceData = { 4, { kFlatTrack1x4CSeq0, kFlatTrack1x4CSeq1, kFlatTrack1x4CSeq2, kFlatTrack1x4CSeq3 } },
    };

    constexpr auto kTEDFlatTrack3x3 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 96, 32, 0 },
        .pieceLength = 0,
        .curveChain = { TrackElemType::none, TrackElemType::none },
        .priceModifier = 524288,
        .mirrorElement = TrackElemType::flatTrack3x3,
        .flags = {},
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 9,
                          { kFlatTrack3x3Seq0, kFlatTrack3x3Seq1, kFlatTrack3x3Seq2, kFlatTrack3x3Seq3, kFlatTrack3x3Seq4,
                            kFlatTrack3x3Seq5, kFlatTrack3x3Seq6, kFlatTrack3x3Seq7, kFlatTrack3x3Seq8 } },
    };

    // Custom-ride footprint shapes (TrackElemType.h's customFootprintWxH block) - generated
    // at runtime rather than hand-authored per shape, unlike every table above. A plain
    // (non-constexpr) function avoids the MSVC constexpr-evaluation-step limit that already
    // forces kTEDFlatTrack5x5/6x6/7x7/8x8 above to be `static const` instead of `constexpr`
    // even with hand-written literals (see their own comments) - a compile-time template
    // generator would only make that worse, not better, especially for the largest shapes
    // here (7x8 = 56 tiles). This runs once, at static-init time, like any other function call.
    //
    // Convention (verified against kFlatTrack4x4/kFlatTrack1x4A's own data, tile-by-tile):
    // cursor/origin tile is the (row=0, col=0) corner - NOT the geometric centre some of the
    // larger square pieces above use (kFlatTrack6x6/8x8's own comments document why they
    // instead centre on the cursor: it predates this generator and was a free per-shape
    // choice, not an engine requirement) - clearance is therefore always >= 0 on both axes,
    // matching kFlatTrack4x4/2x2/1x4A exactly. row increases along clearance.x, col along
    // clearance.y. NE/SE/NW/SW entrance-connection flags follow the same geometric edge rule
    // as the paint side's own edge detection (see GetGenericFootprintPaintFunction below):
    // col==0 -> NE, col==length-1 -> SE, row==0 -> NW, row==width-1 -> SW (both flags apply
    // simultaneously to every tile on a 1-wide/1-long axis, the only sensible reading when an
    // axis has no interior). True corners (both axes at an extreme) also get hasHeightMarker.
    // ClearanceFlag::flag1 on the very last sequence re-centres the ride-construction
    // toolbar's piece-selection thumbnail on the origin (RideConstruction.cpp's DrawTrackPiece)
    // - cosmetic only, mirrors every existing multi-tile piece's last entry.
    static TrackElementDescriptor BuildGenericFootprintDescriptor(TrackElemType type, uint8_t width, uint8_t length)
    {
        TrackElementDescriptor ted{};
        ted.coordinates = { 0, 2, 0, 0, 0, 32 };
        ted.pieceLength = 0;
        ted.curveChain = { TrackElemType::none, TrackElemType::none };
        // kTEDFlatTrack5x5/6x6/7x7/8x8 (25/36/49/64 tiles) all share the SAME priceModifier,
        // 1638400 (= 65536*25) - a deliberate ceiling on placement cost for the largest
        // footprints, not a literal per-tile scale past that point (their own values don't
        // continue scaling with tile count). Mirror that ceiling here so a large generated
        // shape's build/placement cost doesn't run away unbounded past what the existing
        // largest hand-authored pieces already settled on.
        ted.priceModifier = std::min<uint32_t>(65536u * width * length, 1638400u);
        ted.mirrorElement = type;
        ted.flags = {};
        ted.definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none,
                            TrackRoll::none,          TrackRoll::none, 0 };

        SequenceTable table{};
        const uint8_t numSequences = width * length;
        uint8_t index = 0;
        for (uint8_t row = 0; row < width; row++)
        {
            for (uint8_t col = 0; col < length; col++)
            {
                SequenceDescriptor seq{};
                seq.clearance.x = row * 32;
                seq.clearance.y = col * 32;
                seq.clearance.quarterTile = { 0b1111, 0 };

                const bool atRowMin = row == 0;
                const bool atRowMax = row == width - 1;
                const bool atColMin = col == 0;
                const bool atColMax = col == length - 1;

                if (atColMin)
                    seq.flags.set(SequenceFlag::entranceConnectionNE);
                if (atColMax)
                    seq.flags.set(SequenceFlag::entranceConnectionSE);
                if (atRowMin)
                    seq.flags.set(SequenceFlag::entranceConnectionNW);
                if (atRowMax)
                    seq.flags.set(SequenceFlag::entranceConnectionSW);

                const bool isCorner = (atRowMin || atRowMax) && (atColMin || atColMax);
                if (isCorner)
                    seq.flags.set(SequenceFlag::hasHeightMarker);

                if (row == 0 && col == 0)
                {
                    seq.flags.set(SequenceFlag::trackOrigin);
                    seq.flags.set(SequenceFlag::hasHeightMarker);
                }

                if (index == numSequences - 1)
                    seq.clearance.flags.set(ClearanceFlag::flag1);

                seq.woodenSupports = { WoodenSupportSubType::neSw };

                table.sequences[index] = seq;
                index++;
            }
        }
        table.numSequences = numSequences;
        ted.sequenceData = table;
        return ted;
    }

    static const TrackElementDescriptor kTEDCustomFootprint1x2 = BuildGenericFootprintDescriptor(
        TrackElemType::customFootprint1x2, 1, 2);
    static const TrackElementDescriptor kTEDCustomFootprint1x3 = BuildGenericFootprintDescriptor(
        TrackElemType::customFootprint1x3, 1, 3);
    static const TrackElementDescriptor kTEDCustomFootprint1x6 = BuildGenericFootprintDescriptor(
        TrackElemType::customFootprint1x6, 1, 6);
    static const TrackElementDescriptor kTEDCustomFootprint1x7 = BuildGenericFootprintDescriptor(
        TrackElemType::customFootprint1x7, 1, 7);
    static const TrackElementDescriptor kTEDCustomFootprint1x8 = BuildGenericFootprintDescriptor(
        TrackElemType::customFootprint1x8, 1, 8);
    static const TrackElementDescriptor kTEDCustomFootprint2x3 = BuildGenericFootprintDescriptor(
        TrackElemType::customFootprint2x3, 2, 3);
    static const TrackElementDescriptor kTEDCustomFootprint2x5 = BuildGenericFootprintDescriptor(
        TrackElemType::customFootprint2x5, 2, 5);
    static const TrackElementDescriptor kTEDCustomFootprint2x6 = BuildGenericFootprintDescriptor(
        TrackElemType::customFootprint2x6, 2, 6);
    static const TrackElementDescriptor kTEDCustomFootprint2x7 = BuildGenericFootprintDescriptor(
        TrackElemType::customFootprint2x7, 2, 7);
    static const TrackElementDescriptor kTEDCustomFootprint2x8 = BuildGenericFootprintDescriptor(
        TrackElemType::customFootprint2x8, 2, 8);
    static const TrackElementDescriptor kTEDCustomFootprint3x4 = BuildGenericFootprintDescriptor(
        TrackElemType::customFootprint3x4, 3, 4);
    static const TrackElementDescriptor kTEDCustomFootprint3x5 = BuildGenericFootprintDescriptor(
        TrackElemType::customFootprint3x5, 3, 5);
    static const TrackElementDescriptor kTEDCustomFootprint3x6 = BuildGenericFootprintDescriptor(
        TrackElemType::customFootprint3x6, 3, 6);
    static const TrackElementDescriptor kTEDCustomFootprint3x7 = BuildGenericFootprintDescriptor(
        TrackElemType::customFootprint3x7, 3, 7);
    static const TrackElementDescriptor kTEDCustomFootprint3x8 = BuildGenericFootprintDescriptor(
        TrackElemType::customFootprint3x8, 3, 8);
    static const TrackElementDescriptor kTEDCustomFootprint4x5 = BuildGenericFootprintDescriptor(
        TrackElemType::customFootprint4x5, 4, 5);
    static const TrackElementDescriptor kTEDCustomFootprint4x6 = BuildGenericFootprintDescriptor(
        TrackElemType::customFootprint4x6, 4, 6);
    static const TrackElementDescriptor kTEDCustomFootprint4x7 = BuildGenericFootprintDescriptor(
        TrackElemType::customFootprint4x7, 4, 7);
    static const TrackElementDescriptor kTEDCustomFootprint4x8 = BuildGenericFootprintDescriptor(
        TrackElemType::customFootprint4x8, 4, 8);
    static const TrackElementDescriptor kTEDCustomFootprint5x6 = BuildGenericFootprintDescriptor(
        TrackElemType::customFootprint5x6, 5, 6);
    static const TrackElementDescriptor kTEDCustomFootprint5x7 = BuildGenericFootprintDescriptor(
        TrackElemType::customFootprint5x7, 5, 7);
    static const TrackElementDescriptor kTEDCustomFootprint5x8 = BuildGenericFootprintDescriptor(
        TrackElemType::customFootprint5x8, 5, 8);
    static const TrackElementDescriptor kTEDCustomFootprint6x7 = BuildGenericFootprintDescriptor(
        TrackElemType::customFootprint6x7, 6, 7);
    static const TrackElementDescriptor kTEDCustomFootprint6x8 = BuildGenericFootprintDescriptor(
        TrackElemType::customFootprint6x8, 6, 8);
    static const TrackElementDescriptor kTEDCustomFootprint7x8 = BuildGenericFootprintDescriptor(
        TrackElemType::customFootprint7x8, 7, 8);

} // namespace OpenRCT2::TrackMetadata
