#!/bin/sh
# Copyright (C) 2012-2013 KappaRock LLC

# first kill the running process to prevent from send serial messages
/etc/init.d/kappaio-watch stop
# check if kappaio daemon was killed, if it wasn't, kill it again
[ -f /var/run/kappaio-watch.pid ] && kill -9 $(cat /var/run/kappaio-watch.pid)
# if still alive then something must wrong, so exit
[ -f /var/run/kappaio-watch.pid ] && (exit 1)
# now start flashing
kappaio -w ${1}
rc=$?
/etc/init.d/kappaio-watch start
(exit $rc)
