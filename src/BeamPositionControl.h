#ifndef BEAMPOSITIONCONTROL_H_
#define BEAMPOSITIONCONTROL_H_

#include "PositionMotionMatrices.h"

void autoAlignPause();
void autoAlignRun();
void initBeamPositionControl(::QueueHandle_t *_psd_queue);
void moveBeamToPosition(const FloatVector4 *dest_p);

#endif
