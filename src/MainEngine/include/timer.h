
#include <time.h>
#include <chrono>

using namespace std::chrono;
class Timer
{
private:
  time_point<high_resolution_clock>lastFrame;
  double ticksPerSecond = 60;
  double tickRate = 1.0/60.0;


public:
  Timer(double TicksPerSecond=60)
  {
    ticksPerSecond = TicksPerSecond;
    tickRate = 1.0/ticksPerSecond;
    lastFrame = high_resolution_clock::now();
  }

  void updateTickRate(double TicksPerSecond)
  {
    ticksPerSecond = TicksPerSecond;
  }

  void wait()
  {
    auto currentFrame = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(currentFrame - lastFrame);
    double deltaFrame = time_span.count();

    int waitTime = (tickRate-deltaFrame)*1000;
    std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));
    lastFrame = high_resolution_clock::now();
  }



};
