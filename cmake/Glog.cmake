set(GFLAGS_PREFER_EXPORTED_GFLAGS_CMAKE_CONFIGURATION TRUE)
set(GLOG_PREFER_EXPORTED_GLOG_CMAKE_CONFIGURATION TRUE)
find_package(GLOG REQUIRED)

if(GLOG_FOUND)
    message(STATUS "GLOG FOUND. You need to set the environment variables: GLOG_alsologtostderr=1 or GLOG_logtostderr=1")
    message(WARNING "${GLOG_LIBRARY_DIRS} ${GLOG_INCLUDE_DIRS} ${GLOG_LIBRARYS} ${GLOG_FOUND} ???")
else()
    message(WARNING "GLOG NOT FOUND")
endif()