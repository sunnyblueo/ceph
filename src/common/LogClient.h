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

#ifndef CEPH_LOGCLIENT_H
#define CEPH_LOGCLIENT_H

#include "common/LogEntry.h"
#include "common/Mutex.h"

#include <iosfwd>
#include <sstream>

class LogClient;
class MLog;
class MLogAck;
class Messenger;
class MonMap;
class Message;
struct Connection;

class LogClientTemp
{
public:
  LogClientTemp(clog_type type_, LogClient &parent_);
  LogClientTemp(const LogClientTemp &rhs);
  ~LogClientTemp();

  template<typename T>
  std::ostream& operator<<(const T& rhs)
  {
    return ss << rhs;
  }

private:
  clog_type type;
  LogClient &parent;
  stringstream ss;
};

class LogClient
{
public:
  enum logclient_flag_t {
    NO_FLAGS = 0,
    FLAG_MON = 0x1,
  };

  LogClient(CephContext *cct, Messenger *m, MonMap *mm,
	    enum logclient_flag_t flags);
  LogClient(CephContext *cct, Messenger *m, MonMap *mm,
            enum logclient_flag_t flags,
            const std::string &channel,
            const std::string &facility,
            const std::string &prio);

  bool handle_log_ack(MLogAck *m);

  LogClientTemp debug() {
    return LogClientTemp(CLOG_DEBUG, *this);
  }
  void debug(std::stringstream &s) {
    do_log(CLOG_DEBUG, s);
  }
  LogClientTemp info() {
    return LogClientTemp(CLOG_INFO, *this);
  }
  void info(std::stringstream &s) {
    do_log(CLOG_INFO, s);
  }
  LogClientTemp warn() {
    return LogClientTemp(CLOG_WARN, *this);
  }
  void warn(std::stringstream &s) {
    do_log(CLOG_WARN, s);
  }
  LogClientTemp error() {
    return LogClientTemp(CLOG_ERROR, *this);
  }
  void error(std::stringstream &s) {
    do_log(CLOG_ERROR, s);
  }
  LogClientTemp sec() {
    return LogClientTemp(CLOG_SEC, *this);
  }
  void sec(std::stringstream &s) {
    do_log(CLOG_SEC, s);
  }

  void reset_session();
  Message *get_mon_log_message();
  bool are_pending();

  void set_log_to_monitors(bool v) {
    log_to_monitors = v;
  }
  void set_log_to_syslog(bool v) {
    log_to_syslog = v;
  }
  void set_log_channel(const std::string& v) {
    log_channel = v;
  }
  void set_log_prio(const std::string& v) {
    log_prio = v;
  }
  void set_syslog_facility(const std::string& v) {
    syslog_facility = v;
  }
  std::string get_log_prio() { return log_prio; }
  std::string get_log_channel() { return log_channel; }
  std::string get_syslog_facility() { return syslog_facility; }
  bool must_log_to_syslog() { return log_to_syslog; }
  /**
   * Do we want to log to syslog?
   *
   * @return true if log_to_syslog is true and both channel and prio
   *         are not empty; false otherwise.
   */
  bool do_log_to_syslog() {
    return must_log_to_syslog() &&
          !log_prio.empty() && !log_channel.empty();
  }
  bool must_log_to_monitors() { return log_to_monitors; }
/*
  void add_watched_channel(const std::string& c) {
    watched_channels.insert(c);
  }
  void remove_watched_channel(const std::string& c) {
    watched_channels.erase(c);
  }
  bool is_watched_channel(const std::string &c) {
    return (watched_channels.count(c) > 0);
  }
*/
private:
  void do_log(clog_type prio, std::stringstream& ss);
  void do_log(clog_type prio, const std::string& s);
  Message *_get_mon_log_message();

  CephContext *cct;
  Messenger *messenger;
  MonMap *monmap;
  bool is_mon;
  Mutex log_lock;
  version_t last_log_sent;
  version_t last_log;
  std::deque<LogEntry> log_queue;

  std::string log_channel;
  std::string log_prio;
  std::string syslog_facility;
  bool log_to_syslog;
  bool log_to_monitors;
// std::set<const std::string> watched_channels;

  friend class LogClientTemp;
};

#endif
