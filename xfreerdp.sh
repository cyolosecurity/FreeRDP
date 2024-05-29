#!/bin/bash

if [ ! -e /FreeRDP/con.rdp ]; then

	echo -e $XFREERDP_CONNECTION_FILE_CONTENT_BASE64 | base64 -d > /FreeRDP/con.rdp

	gateway=$(grep '^gatewayhostname:s:' /FreeRDP/con.rdp | awk -F':' '{print $3}')

        host_ip=$(dig +short host.docker.internal)

        echo "$host_ip $gateway" >> /etc/hosts
fi

/FreeRDP/client/X11/xfreerdp con.rdp /u:nouser /p:nopassword /cert:ignore 
