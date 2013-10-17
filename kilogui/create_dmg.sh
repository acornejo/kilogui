#!/bin/bash
DMG=$PWD/dmg
VOLNAME=KiloGUI
DMGNAME=kilogui.dmg
SIZE=20M

rm -fR $DMG
rm -f pack.temp.dmg
rm -f $DMGNAME
mkdir -p $DMG
mkdir -p $DMG/.background
cp images/background.png $DMG/.background
cp -a kilogui.app $DMG/kilogui.app

osascript<<END
tell application "Finder"
    make new alias file to POSIX file "/Applications" at POSIX file "$DMG" with properties {name: "Applications"}                                                                                
end tell
END

if [ -f "dmg_DS_Store" ]; then
    cp dmg_DS_Store $DMG/.DS_Store
fi

hdiutil create -srcfolder "${DMG}" -volname "${VOLNAME}" -fs HFS+ -format UDRW -size ${SIZE} pack.temp.dmg

if [ ! -f "dmg_DS_Store" ]; then
    device=$(hdiutil attach -readwrite -noverify -noautoopen "pack.temp.dmg" | egrep "^/Volumes" )
    echo $device

    # open $device
    # open $device/.background
    echo "Setup the view options of the folders, and eject the volume before continuing."
    read
    # cp $device/.DS_Store dmg_DS_Store
fi
# hdiutil convert "pack.temp.dmg" -format UDZO -imagekey zlib-level=9 -o ${DMGNAME}
hdiutil convert "pack.temp.dmg" -format UDBZ -o ${DMGNAME}
rm -f pack.temp.dmg
rm -fr $DMG
