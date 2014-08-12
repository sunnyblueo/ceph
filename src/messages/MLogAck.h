// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*- 
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2004-2006 Sage Weil <sage@newdream.net>
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software 
 * Foundation.  See file COPYING.
 * 
 */

#ifndef CEPH_MLOGACK_H
#define CEPH_MLOGACK_H

#include "common/LogEntry.h"
#include <uuid/uuid.h>

class MLogAck : public Message {
public:
  uuid_d fsid;
  version_t last; // keeping it for backward compatibility
  std::map<std::string, version_t> channel_last;

  static const int HEAD_VERSION = 2;
  static const int COMPAT_VERSION = 1;

  MLogAck() : Message(MSG_LOGACK, HEAD_VERSION, COMPAT_VERSION) {}
  MLogAck(uuid_d& f, version_t l, std::map<std::string, version_t> &m)
    : Message(MSG_LOGACK, HEAD_VERSION, COMPAT_VERSION),
      fsid(f), last(v), channel_last(m) {}
private:
  ~MLogAck() {}

public:
  const char *get_type_name() const { return "log_ack"; }
  void print(ostream& out) const {
    out << "log(last " << channel_last << ")";
  }

  void encode_payload(uint64_t features) {
    ::encode(fsid, payload);
    ::encode(channel_last, payload);
  }
  void decode_payload() {
    bufferlist::iterator p = payload.begin();
    ::decode(fsid, p);
    ::decode(last, p);
    if (header.version < 2) {
      channel_last[CLOG_CHANNEL_DEFAULT] = last;
    } else {
      ::decode(channel_last, p);
    }
  }
};

#endif
