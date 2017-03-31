#ifndef _INT_ITERATOR_H_
#define _INT_ITERATOR_H_

#include <iterator>

class int_iterator : public std::iterator<std::random_access_iterator_tag, int> {
private:
  int value_;

public:
  explicit int_iterator(int value)
    : value_(value) {
    // emtpy
  }

  int_iterator(int_iterator const & other)
    : value_(other.value_) {
    // emtpy
  }

  int_iterator & operator =(int_iterator const & other) {
    value_ = other.value_;
    return *this;
  }

  int_iterator & operator +=(int n) {
    value_ += n;
    return *this;
  }

  int_iterator & operator -=(int n) {
    value_ -= n;
    return *this;
  }

  int_iterator & operator ++() {
    ++value_;
    return *this;
  }

  int_iterator operator ++(int) {
    return int_iterator(value_++);
  }

  int_iterator & operator --() {
    --value_;
    return *this;
  }

  int_iterator operator --(int) {
    return int_iterator(value_--);
  }

  int_iterator operator +(int n) const {
    return int_iterator(value_ + n);
  }

  int operator -(int_iterator const & it) const {
    return value_ - it.value_;
  }

  int_iterator operator -(int n) const {
    return int_iterator(value_ - n);
  }

  bool operator ==(int_iterator const & it) const {
    return value_ == it.value_;
  }

  bool operator !=(int_iterator const & it) const {
    return value_ != it.value_;
  }

  bool operator <(int_iterator const & it) const {
    return value_ < it.value_;
  }

  bool operator <=(int_iterator const & it) const {
    return value_ <= it.value_;
  }

  bool operator >(int_iterator const & it) const {
    return value_ > it.value_;
  }

  bool operator >=(int_iterator const & it) const {
    return value_ >= it.value_;
  }

  int & operator *() {
    return value_;
  }

  int operator *() const {
    return value_;
  }

  int operator [](int n) const {
    return value_ + n;
  }
};

inline int_iterator operator +(int n, int_iterator const & it) {
  return it + n;
}

#endif // _INT_ITERATOR_H_
