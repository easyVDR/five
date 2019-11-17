#!/bin/sh
#
# 2007/04/12 by Marc Wernecke - www.zulu-entertainment.de
#
# music_createtheme.sh - v.0.2
#
##
### Usage:
### ======
### Copy this script to $VDRCONFDIR/plugins/music/scripts
### Then place an entry in $VDRCONFDIR/plugins/music/data/musiccmds.data like this:
### ***********************************************************************************
### Create a new Colortheme? : /var/lib/vdr/plugins/music/scripts/music_createtheme.sh;
### ***********************************************************************************
###
### Supported Skins:
### ================
### EnigmaNG
### soppalusikka
##
#

ScriptDir=$(dirname $0)
cd $ScriptDir

eval $(grep -m 1 "OSDSkin"  ../../../../../setup.conf | sed -e 's/ //g')
eval $(grep -m 1 "OSDTheme" ../../../../../setup.conf | sed -e 's/ //g')

VdrThemeFile=../../../../../themes/${OSDSkin}-${OSDTheme}.theme
MusicThemeFile=../../../themes/osd-${OSDSkin}-${OSDTheme}.theme


cat $VdrThemeFile | while read NewLine; do
    echo $NewLine | sed -e 's/ //g' >> ${VdrThemeFile}.$$
done

. ${VdrThemeFile}.$$
rm -f ${VdrThemeFile}.$$

case $OSDSkin in
    EnigmaNG)
        echo "<SKIN-ID>${OSDSkin}-${OSDTheme}
## LINE \"MUST\" END WITH THE COLORVALUE AND WITHOUT SPACES
## 0xAARRGGBB
## TOP: 3 different colors (one is used for transparence)
<value> clrTopBG1             =0x${clrTitleBg}
<value> clrTopTextFG1         =0x${clrTitleFg}
## BETWEEN TOP AND LIST: 4 different colors
<value> clrTopBG2             =0x${clrAltBackground}
<value> clrTopTextFG2         =0x${clrMenuTxtFg}
<value> clrTopItemBG1         =0x${clrAltBackground}
<value> clrTopItemInactiveFG  =0x${clrBackground}
<value> clrTopItemActiveFG    =0x${clrTitleBg}
## TRACKLIST: 4 different colors
<value> clrListBG1            =0x${clrAltBackground}
<value> clrListBG2            =0x${clrBackground}
<value> clrListTextFG         =0x${clrMenuTxtFg}
<value> clrListTextActiveFG   =0x${clrMenuTxtFg}
<value> clrListTextActiveBG   =0x${clrAltBackground}
####clrListTextScrollFG   =0x${clrBackground}
####clrListTextScrollBG   =0x${clrAltBackground}
<value> clrListRating         =0x${clrButtonGreenBg}
## INFO: 4 different colors
<value> clrInfoBG1            =0x${clrAltBackground}
<value> clrInfoBG2            =0x${clrBackground}
<value> clrInfoTextFG1        =0x${clrMenuTxtFg}
<value> clrInfoTitleFG1       =0x${clrMenuTxtFg}
<value> clrInfoTextFG2        =0x${clrTitleBg}
## PROGRESS: 4 different colors
<value> clrProgressBG1        =0x${clrAltBackground}
<value> clrProgressBG2        =0x${clrBackground}
<value> clrProgressbarFG      =0x${clrTitleBg}
<value> clrProgressbarBG      =0x${clrAltBackground}
## STATUS: 6 different colors
<value> clrStatusBG           =0x${clrBottomBg}
<value> clrStatusRed          =0x${clrButtonRedBg}
<value> clrStatusGreen        =0x${clrButtonGreenBg}
<value> clrStatusYellow       =0x${clrButtonYellowBg}
<value> clrStatusBlue         =0x${clrButtonBlueBg}
<value> clrStatusTextFG       =0x${clrTitleFg}
## FOR MPEGBACKGROUNDCOVER
<value> isMpeg                =0
<value> localbackground       =/music-default-local.mpg
<value> streambackground      =/music-default-stream.mpg" > $MusicThemeFile ;;

    soppalusikka)
        echo "<SKIN-ID>${OSDSkin}-${OSDTheme}
## LINE \"MUST\" END WITH THE COLORVALUE AND WITHOUT SPACES
## 0xAARRGGBB
## TOP: 3 different colors (one is used for transparence)
<value> clrTopBG1             =0x${clrMenuTitleBg}
<value> clrTopTextFG1         =0x${clrMenuTitleFg}
## BETWEEN TOP AND LIST: 4 different colors
<value> clrTopBG2             =0x${clrMenuItemCurrentBg}
<value> clrTopTextFG2         =0x${clrMenuItemCurrentFg}
<value> clrTopItemBG1         =0x${clrMenuItemCurrentBg}
<value> clrTopItemInactiveFG  =0x${clrBackground}
<value> clrTopItemActiveFG    =0x${clrMenuItemSelectable}
## TRACKLIST: 4 different colors
<value> clrListBG1            =0x${clrMenuItemCurrentBg}
<value> clrListBG2            =0x${clrBackground}
<value> clrListTextFG         =0x${clrMenuItemSelectable}
<value> clrListTextActiveFG   =0x${clrMenuItemSelectable}
<value> clrListTextActiveBG   =0x${clrMenuItemCurrentBg}
####clrListTextScrollFG   =0x${clrBackground}
####clrListTextScrollBG   =0x${clrMenuItemCurrentBg}
<value> clrListRating         =0x${clrButtonGreenBg}
## INFO: 4 different colors
<value> clrInfoBG1            =0x${clrMenuItemCurrentBg}
<value> clrInfoBG2            =0x${clrBackground}
<value> clrInfoTextFG1        =0x${clrMenuItemCurrentFg}
<value> clrInfoTitleFG1       =0x${clrMenuItemSelectable}
<value> clrInfoTextFG2        =0x${clrMenuItemCurrentBg}
## PROGRESS: 4 different colors
<value> clrProgressBG1        =0x${clrMenuItemCurrentBg}
<value> clrProgressBG2        =0x${clrBackground}
<value> clrProgressbarFG      =0x${clrReplayProgressSeen}
<value> clrProgressbarBG      =0x${clrReplayProgressRest}
## STATUS: 6 different colors
<value> clrStatusBG           =0x${clrMenuTitleBg}
<value> clrStatusRed          =0x${clrButtonRedBg}
<value> clrStatusGreen        =0x${clrButtonGreenBg}
<value> clrStatusYellow       =0x${clrButtonYellowBg}
<value> clrStatusBlue         =0x${clrButtonBlueBg}
<value> clrStatusTextFG       =0x${clrMenuTitleFg}
## FOR MPEGBACKGROUNDCOVER
<value> isMpeg                =0
<value> localbackground       =/music-default-local.mpg
<value> streambackground      =/music-default-stream.mpg" > $MusicThemeFile ;;

    *) echo "Sorry, the ${OSDSkin}-skin is not supported yet, but feel free to change this..."; exit 1 ;;
esac

# cp -f $MusicThemeFile ../themes/current.colors
echo "osd-${OSDSkin}-${OSDTheme}.theme was successfully created. You can activate the theme from the commands menu..."
exit 0
