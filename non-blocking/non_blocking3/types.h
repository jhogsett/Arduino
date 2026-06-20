
using NonBlockingAction = void (*)(uint32_t data);

class NonBlockingTimer
{
public:
  void start(uint32_t duration)
  {
    _start_time = millis();
    Serial.print(_start_time);
    Serial.print(" ");
    Serial.println(duration);
    _duration = duration;
  }

  bool elapsed()
  {
    // Serial.println(millis());
    return (millis() - _start_time) >= _duration;
  }

private:
  uint32_t _start_time;
  uint32_t _duration;
};

class NonBlockingLoop
{
public:
  void start(uint32_t cycles)
  {
    _cycles = cycles;
    _cycle = 0;
  }

  bool active()
  {
    return _cycle < _cycles;
  }

  bool cycle()
  {
    _cycle += 1;
    return active();
  }

  uint32_t cycle() const { return _cycle; }

private:
  uint32_t _cycles;
  uint32_t _cycle;
};


