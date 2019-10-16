
#include "Seamless/Utility.hpp"

#include <random>

int64_t cmls::randomInt(int64_t min, int64_t max)
{
  static std::random_device randomdev;
  static std::default_random_engine generator(randomdev());
  std::uniform_int_distribution<int64_t> distribution(min, max);
  int64_t dice_roll = distribution(generator);
  return dice_roll;
}

float cmls::randomFloat(float min, float max)
{
  static std::random_device randomdev;
  static std::default_random_engine generator(randomdev());
  std::uniform_real_distribution<float> distribution(min, max);
  float dice_roll = distribution(generator);
  return dice_roll;
}

