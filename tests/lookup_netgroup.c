/*
   lookup_netgroup.c - simple lookup code for netgroups

   Copyright (C) 2012 Arthur de Jong

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301 USA
*/

#include "config.h"

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <stdlib.h>

/* the main program... */
int main(int argc,char *argv[])
{
  char *host, *user, *domain;
  /* check arguments */
  if (argc != 2)
  {
    fprintf(stderr, "Usage: %s NETGROUP\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  /* start lookup */
  if (setnetgrent(argv[1]) != 0)
  {
    /* output nothing */
    exit(EXIT_FAILURE);
  }
  fprintf(stdout, "%-20s", argv[1]);
  while (getnetgrent(&host, &user, &domain) != 0)
  {
    fprintf(stdout, " (%s, %s, %s)", host, user, domain);
  }
  fprintf(stdout, "\n");
  endnetgrent();
  return 0;
}
