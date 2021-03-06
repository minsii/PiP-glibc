/* Verify that TLS access in separate thread in a dlopened library does not
   deadlock - the module.
   Copyright (C) 2015 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <atomic.h>

static pthread_t th;
static int running = 1;

static void *
test_run (void *p)
{
  /* Spin on the value of RUNNING till it is 1.  The RHEL-7 version of atomic.h
     does not yet have an atomic_load.  We don't need an acquire/release
     barrier either since there is no ordering to worry about, but again,
     atomic.h does not have relaxed atomic operations.  */
  int oldval;
  do
    oldval = atomic_compare_and_exchange_val_acq (&running, 0, 0);
  while (oldval == 1);
    printf ("Test running\n");
  printf ("Test finished\n");
  return NULL;
}

static void __attribute__ ((constructor))
do_init (void)
{
  int ret = pthread_create (&th, NULL, test_run, NULL);

  if (ret != 0)
    {
      printf ("failed to create thread: %s (%d)\n", strerror (ret), ret);
      exit (1);
    }
}

static void __attribute__ ((destructor))
do_end (void)
{
  atomic_exchange_rel (&running, 0);
  int ret = pthread_join (th, NULL);

  if (ret != 0)
    {
      printf ("pthread_join: %s(%d)\n", strerror (ret), ret);
      exit (1);
    }

  printf ("Thread joined\n");
}
