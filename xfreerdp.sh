#!/bin/bash

env > /tmp/env-from-script

user=$XFREERDP_USER
password=$XFREERDP_PASSWORD
server=$XFREERDP_SERVER

/FreeRDP/client/X11/xfreerdp /u:$user /p:$password /v:$server /cert:ignore 
