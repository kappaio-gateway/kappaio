#!/bin/sh
# Copyright (C) 2012-2013 KappaRock LLC

# first kill the running process to prevent from send serial messages
/etc/init.d/rsserial-watch stop
# check if rsserial was killed, if it wasn't, kill it again
[ -f /var/run/rsserial-watch.pid ] && kill -9 $(cat /var/run/rsserial-watch.pid)
# if still alive then something must wrong, so exit
[ -f /var/run/rsserial-watch.pid ] && (exit 1)
# now start flashing
rsserial -w ${1}
rc=$?
/etc/init.d/rsserial-watch start
(exit $rc)
