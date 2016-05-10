#! /bin/bash
[ -f Makefile ] && (make clean; rm Makefile)
rm -rf CMakeCache.txt CMakeFiles/ cmake_install.cmake build output _CPack_Packages CPackConfig.cmake  CPackSourceConfig.cmake install_manifest* install

