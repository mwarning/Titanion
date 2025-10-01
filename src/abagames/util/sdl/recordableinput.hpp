/*
 * $Id: recordableinput.d,v 1.1.1.1 2006/11/19 07:54:55 kenta Exp $
 *
 * Copyright 2005 Kenta Cho. Some rights reserved.
 */

#ifndef RECORDABLEINPUT_H
#define RECORDABLEINPUT_H

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "abagames/util/iterator.hpp"

class NoRecordDataException : public virtual std::runtime_error {
  using std::runtime_error::runtime_error;
};

template <typename T> class InputRecord {
private:
  struct Record {
    int series;
    T *data;
  };
  std::vector<Record> record;
  int idx, series;
  T *replayData;

public:
  InputRecord() { replayData = T::newInstance(); }

  void clear() { record.clear(); }

  void add(T *d) {
    if (record.size() > 0 && record[record.size() - 1].data->equals(d)) {
      record[record.size() - 1].series++;
    } else {
      Record r;
      r.series = 1;
      r.data = T::newInstance(d);
      record.push_back(r);
    }
  }

  void reset() {
    idx = 0;
    series = 0;
  }

  bool hasNext() {
    if (idx >= (int)record.size())
      return false;
    else
      return true;
  }

  T *next() {
    if (idx >= (int)record.size())
      throw NoRecordDataException("No more items");
    if (series <= 0)
      series = record[idx].series;
    replayData->set(*record[idx].data);
    series--;
    if (series <= 0)
      idx++;
    return replayData;
  }

  void save(std::ofstream &fd) {
    fd << record.size();
    for (auto &r : record) {
      fd << r.series;
      r.data->write(fd);
    }
  }

  void load(std::ifstream &fd) {
    clear();
    int l, s;
    T *d;
    fd >> l;
    for (int i = 0; i < l; i++) {
      fd >> s;
      d = T::newInstance();
      d->read(fd);
      Record r;
      r.series = s;
      r.data = d;
      record.push_back(r);
    }
  }
};

/**
 * Record an input for a replay.
 * T represents a data structure of a specific device input.
 */
template <typename T> class RecordableInput {
public:
  InputRecord<T> *inputRecord;

public:
  void startRecord() {
    inputRecord = new InputRecord<T>();
    inputRecord->clear();
  }

  void record(T *d) { inputRecord->add(d); }

  void startReplay(InputRecord<T> *pr) {
    inputRecord = pr;
    inputRecord->reset();
  }

  T *replay() {
    if (!inputRecord->hasNext())
      throw NoRecordDataException("No record data.");
    return inputRecord->next();
  }
};

#endif
