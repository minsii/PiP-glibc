# Copyright (C) 2003, 2004, 2006 Free Software Foundation, Inc.
# This file is part of the GNU C Library.

# The GNU C Library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.

# The GNU C Library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.

# You should have received a copy of the GNU Lesser General Public
# License along with the GNU C Library; if not, write to the Free
# Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
# 02111-1307 USA.

#
#	Sub-makefile for real-time kaio portion of the library.
#
subdir	:= rtkaio

aio-routines   := aio_cancel aio_error aio_fsync aio_misc aio_read	\
		  aio_read64 aio_return aio_suspend aio_write		\
		  aio_write64 lio_listio lio_listio64 aio_sigqueue	\
		  aio_notify
clock-routines := get_clockfreq clock_getcpuclockid			\
		  clock_getres clock_gettime clock_settime		\
		  clock_nanosleep
timer-routines := timer_create timer_delete timer_getoverr		\
		  timer_gettime timer_settime
shm-routines   := shm_open shm_unlink
mq-routines    := mq_open mq_close mq_unlink mq_getattr mq_setattr	\
		  mq_notify mq_send mq_receive mq_timedsend		\
		  mq_timedreceive

librtkaio-routines = $(patsubst %,k%,$(aio-routines)) \
		     $(patsubst %,kaio_%,$(clock-routines) $(timer-routines) \
					 $(shm-routines) $(mq-routines))

tests := tst-shm tst-clock tst-clock_nanosleep tst-timer tst-timer2 \
	 tst-aio tst-aio64 tst-aio2 tst-aio3 tst-aio4 tst-aio5 tst-aio6 \
	 tst-aio7 tst-aio8 tst-aio9 tst-aio10 \
	 tst-mqueue1 tst-mqueue2 tst-mqueue3 tst-mqueue4 \
	 tst-mqueue5 tst-mqueue6 tst-mqueue7 tst-mqueue8 tst-mqueue9 \
	 tst-timer3 tst-timer4 tst-timer5 \
	 tst-cpuclock1 tst-cpuclock2 \
	 tst-cputimer1 tst-cputimer2 tst-cputimer3 \
	 tst-clock2 \
	 tst-aiod tst-aiod64 tst-aiod2 tst-aiod3 tst-aiod4 tst-aiod5

extra-libs := librtkaio
extra-libs-others := $(extra-libs)

include $(..)Makeconfig

ifeq (yes,$(build-shared))
generated += librt.so$(librt.so-version)

$(objpfx)librt.so$(librt.so-version): $(objpfx)librtkaio.so; $(make-link)
endif

include $(..)Rules

CFLAGS-kaio_suspend.c = -fexceptions
CFLAGS-kaio_clock_nanosleep.c = -fexceptions -fasynchronous-unwind-tables
CFLAGS-kaio_librt-cancellation.c = -fasynchronous-unwind-tables

LDFLAGS-rtkaio.so = -Wl,-soname=lib$(libprefix)rt.so$(librt.so-version) \
		    -Wl,--enable-new-dtags,-z,nodelete
CPPFLAGS-librtkaio += -DIS_IN_librt=1 -I$(..)rt

rpath-dirs := $(patsubst rt,rtkaio,$(rpath-dirs))

ifeq (yes,$(build-shared))
others: $(objpfx)librt.so$(librt.so-version)
endif

# Depend on libc.so so a DT_NEEDED is generated in the shared objects.
# This ensures they will load libc.so for needed symbols if loaded by
# a statically-linked program that hasn't already loaded it.
$(objpfx)librtkaio.so: $(common-objpfx)libc.so $(common-objpfx)libc_nonshared.a \
		   $(shared-thread-library) $(elfobjdir)/ld.so

ifeq (yes,$(build-shared))
$(addprefix $(objpfx),$(tests)): $(objpfx)librtkaio.so $(shared-thread-library)
else
$(addprefix $(objpfx),$(tests)): $(objpfx)librtkaio.a $(static-thread-library)
endif
ifeq (yes,$(build-bounded))
$(tests:%=$(objpfx)%-bp): $(objpfx)librtkaio_b.a $(bounded-thread-library)
endif

tst-mqueue7-ARGS = -- $(built-program-cmd)

ifeq (yes,$(build-static-nss))
otherlibs += $(nssobjdir)/libnss_files.a $(resolvobjdir)/libnss_dns.a \
             $(resolvobjdir)/libresolv.a
endif
