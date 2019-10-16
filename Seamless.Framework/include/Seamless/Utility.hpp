
#include <Seamless/Export.hpp>

#include <vector>

namespace cmls
{
  template <typename T>
  bool isPowerOfTwo(T value)
  {
    return (value != 0) && ((value & (value - 1)) == 0);
  }

  template<typename ElemType>
  std::vector<std::vector<ElemType>> euclideanDivide(std::vector<ElemType> const& dividend, size_t n)
  {
    std::vector<std::vector<ElemType>> returner;

    size_t length = dividend.size() / n;
    size_t remain = dividend.size() % n;

    size_t begin = 0;
    size_t end = 0;

    for (size_t i = 0; i < (std::min)(n, dividend.size()); ++i)
    {
      end += (remain > 0) ? (length + !!(remain--)) : length;

      returner.push_back(std::vector<ElemType>(dividend.begin() + begin, dividend.begin() + end));

      begin = end;
    }

    return returner;
  }

  CMLS_API int64_t randomInt(int64_t min, int64_t max);

  CMLS_API float randomFloat(float min, float max);
}