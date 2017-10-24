#ifndef FREQLIB_FREQ_EXPRESSION_H_
#define FREQLIB_FREQ_EXPRESSION_H_
#include <string>
#include "freq_item.h"

namespace libwxfreq {

class Expression {
 public:
  virtual bool ParseItem(const std::string& item) = 0;
  virtual bool IsMatch(const FreqItem& freqitem) const = 0;
  virtual std::string DebugString() const = 0;
  virtual Expression* Clone() const = 0;
};


}  // namespace libwxfreq


#endif  // FREQLIB_FREQ_EXPRESSION_H_
