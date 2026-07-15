/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Paint.RideStructureSegment.h"

#include "../../drawing/ImageId.hpp"
#include "../../entity/RideStructureSegment.h"
#include "../../profiling/Profiling.h"
#include "../../ride/Ride.h"
#include "../../ride/RideData.h"
#include "../Boundbox.h"
#include "../Paint.h"

using namespace OpenRCT2;

// segmentGroup values set by RideStructureSegmentsEnsure.
static constexpr uint8_t kSegmentPlaceholder = 0;    // console-forced ride: SPR_BALLOON wave
static constexpr uint8_t kSegmentRealStructure = 1;  // descriptor-driven ride: real sliced atlas

void OpenRCT2::PaintRideStructureSegment(
    PaintSession& session, const RideStructureSegment& segment, int32_t imageDirection)
{
    PROFILED_FUNCTION();

    // Draw the slice with a SHORT bbox anchored at the segment's own Z. The entity paint path
    // (EntityPaintSetup) has already set the session origin to the segment's (x,y); the bbox offset
    // carries the Z so a stack of these each sort locally against their true neighbours — instead of
    // one tall sprite sorting its far top against an incomplete set (the flicker mechanism). Keeping
    // bboxSizeZ short (< 128) is the whole point.
    const Ride* ride = segment.rideId.IsNull() ? nullptr : GetRide(segment.rideId);

    ImageId image;
    if (ride != nullptr && segment.segmentGroup == kSegmentRealStructure)
    {
        // REAL sliced structure: index the per-slice sub-image the tool packed, in the SAME
        // (dir, frame, slice) order (sprites/manifest._add_structure_slices):
        //   structureBase + (spriteDirection*FramesPerDir + baseFrame)*sliceCount + sliceIndex.
        // baseImageId = Cars[0].base_image_id (structure block base); baseFrame = the ride's current
        // animation frame (pushed by RideStructureSegmentsUpdate). spriteDirection folds the camera
        // rotation with BaseRotation/symmetry exactly as PaintGenericRotatingStructure does. Recolour
        // uses all three ride colours, matching the monolithic structure draw.
        const auto& desc = GetFlatRideDescriptor(*ride);
        const uint8_t numDirs = desc.SymmetricDirections ? 2 : 4;
        // cameraDir = imageDirection>>3 = session rotation (the segment's entity orientation is 0).
        // NOTE (3a follow-up): this omits the ride's PLACEMENT direction, so a ride rotated at build
        // time picks the wrong structure sprite. Correct for a default-orientation placement (the
        // first-validation case). To fix later, set the segment's entity orientation to
        // placementDir*8 at spawn so EntityPaintSetup folds it into imageDirection automatically.
        const uint8_t cameraDir = static_cast<uint8_t>((imageDirection >> 3) & 3);
        const uint8_t spriteDirection = static_cast<uint8_t>(((cameraDir + desc.BaseRotation) & 3) % numDirs);
        const uint32_t framesPerDir = desc.FramesPerDir > 0 ? desc.FramesPerDir : 1;
        const uint32_t index = segment.baseImageId
            + (static_cast<uint32_t>(spriteDirection) * framesPerDir + segment.baseFrame) * segment.sliceCount
            + segment.sliceIndex;
        image = ImageId(
            index, ride->vehicleColours[0].Body, ride->vehicleColours[0].Trim, ride->vehicleColours[0].Tertiary);
    }
    else if (ride != nullptr)
    {
        // PLACEHOLDER (console-forced ride, segmentGroup == kSegmentPlaceholder): the atlas is the
        // normal unsliced structure sheet, so map the ride's animation frame + slice index onto the 8
        // balloon frames for a visible travelling wave (proof the stack animates + sorts flicker-free).
        const uint32_t placeholderFrame = (segment.baseFrame + segment.sliceIndex) % 8;
        image = ImageId(segment.baseImageId + placeholderFrame, ride->vehicleColours[0].Body);
    }
    else
    {
        // Standalone probe (seg_stack, null rideId): raw placeholder image.
        image = ImageId(segment.baseImageId + segment.baseFrame);
    }

    const CoordsXYZ offset(0, 0, segment.z);
    const BoundBoxXYZ bb = { { 0, 0, segment.z }, { 20, 20, segment.bboxSizeZ } };
    PaintAddImageAsParent(session, image, offset, bb);
}
