/*
** ring buffer of int16_t
*/
class RingBufferInt16 {
public:
  RingBufferInt16(uint16_t size) : SIZE(size) { }
  void reset(void) { wptr = rptr = 0; }
  bool can_get(void) { return rptr!=wptr; }
  bool can_put(void) { return (wptr+1)%SIZE != rptr; }
  int16_t peek(void) { return buff[rptr]; }
  int16_t get(void) { int16_t val = buff[rptr++]; rptr %= SIZE; return val; }
  void put(int16_t val) { buff[wptr++] = val; wptr %= SIZE; }
  
private:
  const uint16_t SIZE;
  uint16_t wptr = 0, rptr = 0;
  int16_t buff[SIZE];
};

