#!/bin/bash

### BEGIN INIT INFO
# Provides:		run_lab2
# Required-Start:	$all
# Required-Stop:	
# Default-Start: 	2 3 4 5
# Default-Stop:		0 1 6
# Short-Description:	run_lab2 start script
# Description:		Start run_lab2 priority queue
### END INIT INFO


# Program: 
# 	This program auto run preprogress and lab2 on boot.
# History:
# 	2017/09/27 HSINTING CHUNG


/lab2/preprocess
/lab2/lab2
