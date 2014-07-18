#!/bin/bash
DMG=$PWD/dmg
VOLNAME=KiloGUI
DMGNAME=kilogui.dmg
SIZE=20M

if [ "$(uname)" != "Darwin" ]; then
    echo "Must use OSX to create DMG."
fi

rm -fR kilogui.app
qmake
make
macdeployqt src/kilogui.app

rm -fR $DMG
rm -f pack.temp.dmg
rm -f $DMGNAME
mkdir -p $DMG
mkdir -p $DMG/.background
cp src/images/background.png $DMG/.background
cp -a src/kilogui.app $DMG/kilogui.app

osascript<<END
tell application "Finder"
    make new alias file to POSIX file "/Applications" at POSIX file "$DMG" with properties {name: "Applications"}                                                                                
end tell
END
# ln -s /Applications $DMG/Applications

if [ -f "dmg_DS_Store" ]; then
    cp dmg_DS_Store $DMG/.DS_Store
fi

hdiutil create -srcfolder "${DMG}" -volname "${VOLNAME}" -fs HFS+ -fsargs "-c c=64,a=16,e=16" -format UDRW -size ${SIZE} pack.temp.dmg

if [ ! -f "dmg_DS_Store" ]; then
    HDIOUT=$(hdiutil attach -readwrite -noverify -noautoopen "pack.temp.dmg")
    MOUNT_DIR= $(echo $HDIOUT | grep ^Volumes)
    DEVICE=$(echo $HDIOUT | grep ^dev)

    # chmod -Rf go-w ${MOUNT_DIR}
    # bless --folder ${MOUNT_DIR} --openfolder ${MOUNT_DIR}
    # open ${MOUNT_DIR}
    # open ${MOUNT_DIR}/.background
    echo "Setup the view options of the folders, and eject the volume before continuing."
    read
    # sync
    # cp ${MOUNT_DIR}/.DS_Store dmg_DS_Store
    # hdiutil detach ${DEVICE}
fi
hdiutil convert "pack.temp.dmg" -format UDZO -imagekey zlib-level=9 -o ${DMGNAME}
rm -f pack.temp.dmg
rm -fr $DMG
