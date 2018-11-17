#!/bin/sh

#This script adds kwin rules for qmmp windows

if ! type kreadconfig5 &> /dev/null; then
  exit 1
fi

if ! type kwriteconfig5 &> /dev/null; then
  exit 1
fi

count=`kreadconfig5 --file kwinrulesrc --group General --key count`
i=1
found=0;

while [ $i -le $count ];
do
    desc=`kreadconfig5 --file kwinrulesrc --group $i --key Description`
    if [ $desc = "qmmp" ]; then
        found=1
        break
    fi
    i=$((i+1))
done

if [ $found = "0" ]; then
    group=$((count+1))
    kwriteconfig5 --file kwinrulesrc --group General --key count $((count+1))
    kwriteconfig5 --file kwinrulesrc --group $group --key Description qmmp
    kwriteconfig5 --file kwinrulesrc --group $group --key skipswitcher true
    kwriteconfig5 --file kwinrulesrc --group $group --key skipswitcherrule 2
    kwriteconfig5 --file kwinrulesrc --group $group --key type 5
    kwriteconfig5 --file kwinrulesrc --group $group --key typerule 2
    kwriteconfig5 --file kwinrulesrc --group $group --key types 256
    kwriteconfig5 --file kwinrulesrc --group $group --key wmclass qmmp
    kwriteconfig5 --file kwinrulesrc --group $group --key wmclasscomplete false
    kwriteconfig5 --file kwinrulesrc --group $group --key wmclassmatch 1
    dbus-send --print-reply --dest=org.kde.KWin /KWin org.kde.KWin.reconfigure
fi
