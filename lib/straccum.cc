/*
 * straccum.{cc,hh} -- build up strings with operator<<
 * Eddie Kohler
 *
 * Copyright (c) 1999-2000 Massachusetts Institute of Technology
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * Further elaboration of this license, including a DISCLAIMER OF ANY
 * WARRANTY, EXPRESS OR IMPLIED, is provided in the LICENSE file, which is
 * also accessible at http://www.pdos.lcs.mit.edu/click/license.html
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <click/straccum.hh>
#include <click/string.hh>
#include <click/confparse.hh>
#include <click/glue.hh>

bool
StringAccum::grow(int want)
{
  int ncap = (_cap ? _cap * 2 : 128);
  while (ncap <= want)
    ncap *= 2;
  
  unsigned char *n = new unsigned char[ncap];
  if (!n)
    return false;
  
  if (_s)
    memcpy(n, _s, _cap);
  delete[] _s;
  _s = n;
  _cap = ncap;
  return true;
}

const char *
StringAccum::c_str()
{
  append('\0');
  pop_back();
  return reinterpret_cast<char *>(_s);
}

String
StringAccum::take_string()
{
  int len = length();
  return String::claim_string(take(), len);
}

StringAccum &
operator<<(StringAccum &sa, const char *s)
{
  sa.append(s, strlen(s));
  return sa;
}

StringAccum &
operator<<(StringAccum &sa, long i)
{
  if (char *x = sa.reserve(256)) {
    int len;
    sprintf(x, "%ld%n", i, &len);
    sa.forward(len);
  }
  return sa;
}

StringAccum &
operator<<(StringAccum &sa, unsigned long u)
{
  if (char *x = sa.reserve(256)) {
    int len;
    sprintf(x, "%lu%n", u, &len);
    sa.forward(len);
  }
  return sa;
}

StringAccum &
operator<<(StringAccum &sa, long long q)
{
  if (q < 0)
    sa << '-';
  String qstr = cp_unparse_ulonglong(-q, 10, false);
  return sa << qstr;
}

StringAccum &
operator<<(StringAccum &sa, unsigned long long q)
{
  String qstr = cp_unparse_ulonglong(q, 10, false);
  return sa << qstr;
}

#ifndef __KERNEL__
StringAccum &
operator<<(StringAccum &sa, double d)
{
  if (char *x = sa.reserve(256)) {
    int len;
    sprintf(x, "%g%n", d, &len);
    sa.forward(len);
  }
  return sa;
}
#endif

StringAccum &
operator<<(StringAccum &sa, const struct timeval &tv)
{
  if (char *x = sa.reserve(30)) {
    int len;
    sprintf(x, "%ld.%06ld%n", (long)tv.tv_sec, (long)tv.tv_usec, &len);
    sa.forward(len);
  }
  return sa;
}
