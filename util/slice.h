#ifndef FREQLIB_UTIL_SLICE_H_
#define FREQLIB_UTIL_SLICE_H_

#include<string>

namespace libwxfreq {

class Slice {
 public:
  Slice();
  Slice(const char *start, const char *end);
  Slice(const std::string &other_str);

  std::string ToStr() const;
  void StrTrim(const char *delimiter);

  inline const char *start() const { return start_; }
  inline const char *end() const { return end_; }

  bool operator!=(const Slice &other_slice) const;
  bool operator==(const Slice &other_slice) const;
  bool operator==(const std::string &other_str) const;
  bool operator<(const Slice &other_slice) const;

 private:
  const char *start_, *end_;
};


}  //  namespace libwxfreq
#endif  // FREQLIB_UTIL_SLICE_H_
