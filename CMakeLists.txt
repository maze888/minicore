CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

PROJECT (minicore)

LINK_LIBRARIES (pthread dl)

ADD_COMPILE_OPTIONS (-Wall -g)

ADD_DEFINITIONS (-D_GNU_SOURCE)

SET (SRCS mco.c mco_inter.c mco_util.c mco_thread_test.c mco_encode.c mco_properties.c mco_string.c mco_map.c mco_queue.c mco_system.c mco_thread_pool.c)

ADD_LIBRARY (minicore SHARED ${SRCS})

SET (CMAKE_INSTALL_PREFIX ${KMS_PKG_HOME})

INSTALL (TARGETS minicore DESTINATION lib)
