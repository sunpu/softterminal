#!/bin/bash
now=`date '+%Y-%m-%d %H:%M:%S'`

grepFlag='mdbroker.py'
thisLog='./logs/watchlog'
sleepTime=5

[[ ! -d logs ]] && mkdir -p logs

while [ 0 -lt 1 ]
do
    now=`date '+%Y-%m-%d %H:%M:%S'`
    ret=`ps aux | grep "$grepFlag" | grep -v grep | wc -l`
    if [ $ret -eq 0 ]; then
        python mdbroker.py &
    else
        echo "$now process exists , sleep $sleepTime seconds " > "$thisLog"
    fi
    sleep $sleepTime
done