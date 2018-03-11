#include "animation.h"


int32_t speculativeProgressForTimeDelta(struct AnimationTimingModel * model, uint32_t timeDelta);


void AnimationTimingModel::increment(uint32_t timeDelta) {
  // Skip immediately on some cases.
  // Assumes timeDelta is left unsigned and > 0.
  if (
    (rate100 > 0 && progress == ANIMATION_PRORGESS_MAX)
    || (rate100 < 0 && progress == 0)
  ) return;

  int32_t nextProgressSpeculative = speculativeProgressForTimeDelta(this, timeDelta);

  // This is the non-cyclic version, so we need to stop at either end.
  if (nextProgressSpeculative > ANIMATION_PRORGESS_MAX) {
    // NOTE: Non-cyclic progress _can_ be the max value!
    progress = ANIMATION_PRORGESS_MAX;
  }
  else if (nextProgressSpeculative < 0) {
    progress = 0;
  }
  else {
    progress = nextProgressSpeculative;
  }
}

void AnimationTimingModel::incrementCyclic(uint32_t timeDelta) {
  int32_t nextProgressSpeculative = speculativeProgressForTimeDelta(this, timeDelta);

  // Put things into positive territory before cycling via mod.
  if (nextProgressSpeculative < 0) {
    int32_t howFarDown = (int32_t)((nextProgressSpeculative * -1) / ANIMATION_PRORGESS_MAX_UL);
    // Add one to put it into positive territory.
    nextProgressSpeculative += ANIMATION_PRORGESS_MAX * (howFarDown + 1);
  }

  progress = nextProgressSpeculative % ANIMATION_PRORGESS_MAX;
}


/**
 * Calculates the speculative next progress for a given time delta,
 * upgrading progress to an int32_t (signed!) to account for over/underflows.
 */
int32_t speculativeProgressForTimeDelta(struct AnimationTimingModel * model, uint32_t timeDelta) {
  int16_t normalProgressIcrement = (int16_t)(ANIMATION_PRORGESS_MAX_UL * timeDelta / model->normalDuration);
  int32_t progressIncrement = (
    (int32_t)normalProgressIcrement
    * (int32_t)model->rate100
    / 100
  );
  int32_t speculativeProgress = (int32_t)(model->progress) + progressIncrement;

  return speculativeProgress;
}
