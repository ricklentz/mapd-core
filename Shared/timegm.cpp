/*
 * Copyright (c) 2001-2006, NLnet Labs. All rights reserved.
 *
 * This software is open source.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of the NLNET LABS nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "TimeGM.h"
#include <boost/algorithm/string.hpp>
#include <cmath>
#include <ctime>

/* Number of days per month (except for February in leap years). */
// static const int monoff[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

int TimeGM::is_leap_year(int year) {
  return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}

int TimeGM::leap_days(int y1, int y2) {
  --y1;
  --y2;
  return (y2 / 4 - y1 / 4) - (y2 / 100 - y1 / 100) + (y2 / 400 - y1 / 400);
}

time_t TimeGM::parse_fractional_seconds(uint64_t sfrac,
                                        const int ntotal,
                                        const SQLTypeInfo& ti) {
  int dimen = ti.get_dimension();
  int nfrac = log10(sfrac) + 1;
  if (ntotal - nfrac > dimen) {
    return 0;
  }
  if (ntotal >= 0 && ntotal < dimen) {
    sfrac *= pow(10, dimen - ntotal);
  } else if (ntotal > dimen) {
    sfrac /= pow(10, ntotal - dimen);
  }
  return sfrac;
}

time_t TimeGM::parse_meridians(const time_t& timeval,
                               const char* p,
                               const uint32_t& hour,
                               const SQLTypeInfo& ti) {
  char meridies[20];
  if (sscanf(p, "%*d %s", meridies) != 1) {
    if (sscanf(p, "%s", meridies) != 1) {
      return timeval;
    };
  }
  if (boost::iequals(std::string(meridies), "pm") ||
      boost::iequals(std::string(meridies), "p.m.") ||
      boost::iequals(std::string(meridies), "p.m")) {
    return hour == 12 ? timeval
                      : timeval + SECSPERHALFDAY *
                                      static_cast<int64_t>(pow(10, ti.get_dimension()));
  } else if (boost::iequals(std::string(meridies), "am") ||
             boost::iequals(std::string(meridies), "a.m.") ||
             boost::iequals(std::string(meridies), "a.m")) {
    return hour == 12 ? timeval - SECSPERHALFDAY *
                                      static_cast<int64_t>(pow(10, ti.get_dimension()))
                      : timeval;
  } else {
    return timeval;
  }
}

/*
 * Code adapted from Python 2.4.1 sources (Lib/calendar.py).
 */
time_t TimeGM::my_timegm_days(const struct tm* tm) {
  int32_t year;
  time_t days;
  year = 1900 + tm->tm_year;
  days = 365 * (year - 1970) + leap_days(1970, year);
  days += monoff[tm->tm_mon];
  if (tm->tm_mon > 1 && is_leap_year(year)) {
    ++days;
  }
  days += tm->tm_mday - 1;
  return days;
}

time_t TimeGM::my_timegm(const struct tm* tm) {
  time_t days;
  time_t hours;
  time_t minutes;
  time_t seconds;

  days = my_timegm_days(tm);
  hours = days * 24 + tm->tm_hour;
  minutes = hours * 60 + tm->tm_min;
  seconds = minutes * 60 + tm->tm_sec;

  seconds -= tm->tm_gmtoff;

  return seconds;
}

time_t TimeGM::my_timegm(const struct tm* tm, const time_t& fsc, const SQLTypeInfo& ti) {
  time_t sec;

  sec = my_timegm(tm) * static_cast<int64_t>(pow(10, ti.get_dimension()));
  sec += fsc;

  return sec;
}
