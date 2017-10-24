#ifndef FREQLIB_FREQ_PLAIN_EXPRESSION_H_
#define FREQLIB_FREQ_PLAIN_EXPRESSION_H_

#include <string>
#include "expression.h"
#include "log.h"

namespace libwxfreq {

class PlainExpression: public Expression {
 public:
  PlainExpression() { }

  bool ParseItem(const std::string& item);
  bool IsMatch(const FreqItem& freqitem) const;
  std::string DebugString() const;
  Expression* Clone() const;

 private:
  typedef bool (*cmp)(const FreqItem&, const unsigned int, const unsigned int);

  PlainExpression(const cmp func, const unsigned int threshold,
                  const unsigned char key_index)
    : cmp_(func),
      threshold_(threshold),
      key_index_(key_index) { }

  // parse item
  unsigned int threshold_;
  uint8_t key_index_;
  cmp cmp_;
};

}  // namespace libwxfreq

#endif  // FREQLIB_FREQ_PLAIN_EXPRESSION_H_
