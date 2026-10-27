#!/bin/sh

echo "Executing kt_Kick.sh"
sleep 15
FILE=/usr/bin/KT_Kick
if test -f "$FILE"; then
	chmod 777 /usr/bin/KT_Kick
	/usr/bin/KT_Kick
	echo "$FILE Executing KT_Kick bin."
else 
	echo "$FILE KT_Kick Bin missing"
fi
