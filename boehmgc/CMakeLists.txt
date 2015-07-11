SET(SRCCOMMON src/allchblk.c src/alloc.c src/blacklst.c src/dbg_mlc.c src/dyn_load.c src/finalize.c
	src/gc_cpp.cpp src/headers.c src/mach_dep.c src/malloc.c src/mallocx.c src/mark.c src/mark_rts.c
	src/misc.c src/new_hblk.c src/obj_map.c src/os_dep.c src/ptr_chck.c src/reclaim.c 
	src/stubborn.c src/typd_mlc.c)

IF(WIN32)
	
	SET(SRC ${SRCCOMMON} src/win32_threads.c)
	ADD_DEFINITIONS(-DSILENT -DALL_INTERIOR_POINTERS -DGC_NOT_DLL
		-DGC_WIN32_THREADS -D__STDC__ -DGC_OPERATOR_NEW_ARRAY)
ELSE(WIN32)

IF(HAVE_LIBPTHREAD)
	SET(SRC ${SRCCOMMON} src/pthread_support.c src/pthread_stop_world.c)
	ADD_DEFINITIONS(-DSILENT -DALL_INTERIOR_POINTERS -DGC_NOT_DLL
		-DGC_LINUX_THREADS -D_REENTRANT -DGC_OPERATOR_NEW_ARRAY)
ENDIF(HAVE_LIBPTHREAD)

ENDIF(WIN32)


ADD_LIBRARY(boehmgc ${SRC})