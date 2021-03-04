
#ifndef effect_arbiter_h_
#define effect_arbiter_h_
#include "Arduino.h"
#include "AudioStream.h"

class AudioEffectArbiter : public AudioStream
{
#define NVOX 4
public:
 AudioEffectArbiter() : AudioStream(NVOX, inputQueueArray), active_vox(0) { }
  virtual void update(void);
  void define_vox(int index, int _vox, int _priority) {
    if (index >= 0 && index < NVOX) {
      vox[index] = _vox; priority[index] = _priority;
    }
  }
  uint8_t get_active_vox(void) { return active_vox; }
private:
  uint8_t active_vox;
  uint16_t active_tail, ptt_tail;
  uint8_t vox[NVOX], priority[NVOX];
  audio_block_t *inputQueueArray[NVOX];
};

#endif
