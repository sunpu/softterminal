#!/bin/sh

# start services.
start()
{
    [[ ! -d sourcefile ]] && mkdir -p sourcefile
    [[ ! -d convertfile ]] && mkdir -p convertfile
    [[ ! -d avatar ]] && mkdir -p avatar

    python mdbroker.py &
    python st_message_server.py &
    python st_file_server.py &

    ret=`ps aux | grep "brokerwd.sh" | grep -v grep | wc -l`
    if [ $ret -eq 0 ]; then
        ./brokerwd.sh &
    fi
}

stop()
{
    PIDS=`ps -ef | egrep "mdbroker.py|st_message_server.py|st_file_server.py" | grep -v "grep" | awk '{print $2}'`

    while true; do
        if [ -z "$PIDS" ]
        then
            break
        fi
        for PID in $PIDS; do
            kill -9 $PID 2>/dev/null
        done
        PIDS=`ps -ef | egrep "mdbroker.py|st_message_server.py|st_file_server.py" | grep -v "grep" | awk '{print $2}'`
    done
}

RETVAL=0
# execute parameter:
#  @$1  start|stop|restart|...
#  @$2  shell file name.
execute()
{
    case "$1" in
        start)
        start
        ;;

        stop)
        stop
        ;;

        restart)
        stop
        start
        sleep 1
        ;;

        *)
        echo $"Usage: $2 {start|stop|restart|reload|status}"
        RETVAL=1
        exit $RETVAL
    esac
}

# execute shell file.
execute "$1" "$0"

RETVAL=$?

exit $RETVAL
