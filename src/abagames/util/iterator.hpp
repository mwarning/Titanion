/*
 * $Id: iterator.d,v 1.1.1.1 2006/11/19 07:54:55 kenta Exp $
 *
 * Copyright 2004 Kenta Cho. Some rights reserved.
 */

#ifndef ITERATOR_H
#define ITERATOR_H

#include <stdexcept>
#include <vector>

class NoMoreItemsException : public virtual std::runtime_error {
  using std::runtime_error::runtime_error;
};

/**
 * Simple iterator for an array.
 */
template <typename T> class ArrayIterator {
protected:
  std::vector<T> array;
  int idx;

public:
  ArrayIterator(const std::vector<T> &a) {
    array = a;
    idx = 0;
  }

  bool hasNext() {
    if (idx >= array.length)
      return false;
    else
      return true;
  }

  T next() {
    if (idx >= array.length)
      throw NoMoreItemsException("No more items");
    T result = array[idx];
    idx++;
    return result;
  }
};

typedef ArrayIterator<std::string> StringIterator;

#endif // ITERATOR_H
