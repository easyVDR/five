#!/bin/sh
#
# 2008/02/22 by Marc Wernecke - www.zulu-entertainment.de
#
# music_createtheme.sh - 0.4
#
##
### Usage:
### ======
### Copy this script to $VDRCONFDIR/plugins/music/language/*/scripts
### Then place an entry in $VDRCONFDIR/plugins/music/data/musiccmds.data like this:
###
### Create a new Colortheme? : /var/lib/vdr/plugins/music/language/german/scripts/music_createtheme.sh;
###
### ***************************************************************************************************
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
eval $(grep -m 1 "music.UseOneArea" ../../../../../setup.conf | cut -f2 -d "." | sed -e 's/ //g')

# input file
VdrThemeFile=../../../../../themes/${OSDSkin}-${OSDTheme}.theme

cat $VdrThemeFile | while read NewLine; do
    echo $NewLine | sed -e 's/ //g' >> ${VdrThemeFile}.$$
done

. ${VdrThemeFile}.$$
rm -f ${VdrThemeFile}.$$


# output file
if [ "$UseOneArea" = "1" ]; then # for softdecoder or modded ff-cards only
    ThemeName=${OSDSkin}-${OSDTheme}-8bit
    MusicThemeFile=../../../themes/osd-${ThemeName}.theme

    case $OSDSkin in
        EnigmaNG)
echo "<SKIN-ID>${ThemeName}
## LINE \"MUST\" END WITH THE COLORVALUE AND WITHOUT SPACES
## 0xAARRGGBB
#
#
## BACKGROUND
<value> clrBG                 =0x${clrBackground}
#
## TOP
<value> clrTopBG              =0x${clrTitleBg}
<value> clrTopFG              =0x${clrTitleFg}
#
## COVER
<value> clrCoverBG            =0x${clrAltBackground}
<value> clrCoverBar           =0x${clrBackground}
#
## ARTIST,RATING
<value> clrArtistBG           =0x${clrAltBackground}
<value> clrArtistFG           =0x${clrMenuTxtFg}
<value> clrRatingFG           =0x${clrButtonGreenBg}
#
## PLAYSTATUS
<value> clrPlayStatusBG       =0x${clrAltBackground}
<value> clrPlayStatusFG       =0x${clrMenuTxtFg}
<value> clrInfoBG             =0x${clrAltBackground}
<value> clrInfoFG             =0x${clrMenuTxtFg}
<value> clrProgressbarBG      =0x${clrBackground}
<value> clrProgressbarFG      =0x${clrReplayProgressSeen}
#
## TRACKLIST
<value> clrListTitle          =0x${clrReplayCurrent}
<value> clrListBG             =0x${clrAltBackground}
<value> clrListFG             =0x${clrMenuTxtFg}
#
## SYMBOLS
<value> clrSymbolBG           =0x${clrAltBackground}
<value> clrSymbolFG           =0x${clrSymbolInactive}
<value> clrSymbolActive       =0x${clrSymbolActive}
<value> clrRecordingActive    =0x${clrSymbolRecActive}
#
## STATUSBAR
<value> clrStatusBG           =0x${clrBottomBg}
<value> clrStatusFG           =0x${clrMenuTxtFg}
<value> clrStatusRed          =0x${clrButtonRedBg}
<value> clrStatusGreen        =0x${clrButtonGreenBg}
<value> clrStatusYellow       =0x${clrButtonYellowBg}
<value> clrStatusBlue         =0x${clrButtonBlueBg}
#
#
## DEFAULTCOVER
<value> localcover            =/music-local-headphone.png
<value> streamcover           =/music-stream-headphone.png
## FOR MPEGBACKGROUNDCOVER
<value> isMpeg                =0
<value> localbackground       =/themes/mpegs/music-AQUA-local.mpg
<value> streambackground      =/themes/mpegs/music-AQUA-local.mpg
<value> rows                  =7
<value> osdheight             =524
<value> osdwidth              =616
<value> osdtop                =50
<value> osdleft               =48
<value> mpgdif                =28" > $MusicThemeFile ;;

        soppalusikka)
echo "<SKIN-ID>${ThemeName}
## LINE \"MUST\" END WITH THE COLORVALUE AND WITHOUT SPACES
## 0xAARRGGBB
#
#
## BACKGROUND
<value> clrBG                 =0x${clrBackground}
#
## TOP
<value> clrTopBG              =0x${clrMenuTitleBg}
<value> clrTopFG              =0x${clrMenuTitleFg}
#
## COVER
<value> clrCoverBG            =0x${clrMenuTitleBg}
<value> clrCoverBar           =0x${clrBackground}
#
## ARTIST,RATING
<value> clrArtistBG           =0x${clrMenuTitleBg}
<value> clrArtistFG           =0x${clrMenuTitleFg}
<value> clrRatingFG           =0x${clrButtonGreenBg}
#
## PLAYSTATUS
<value> clrPlayStatusBG       =0x${clrMenuTitleBg}
<value> clrPlayStatusFG       =0x${clrMenuTitleFg}
<value> clrInfoBG             =0x${clrMenuTitleBg}
<value> clrInfoFG             =0x${clrMenuTitleFg}
<value> clrProgressbarBG      =0x${clrBackground}
<value> clrProgressbarFG      =0x${clrReplayProgressSeen}
#
## TRACKLIST
<value> clrListTitle          =0x${clrReplayCurrent}
<value> clrListBG             =0x${clrMenuTitleBg}
<value> clrListFG             =0x${clrMenuTitleFg}
#
## SYMBOLS
<value> clrSymbolBG           =0x${clrMenuTitleBg}
<value> clrSymbolFG           =0x${clrChannelSymbolInactive}
<value> clrSymbolActive       =0x${clrChannelSymbolActive}
<value> clrRecordingActive    =0x${clrChannelSymbolRecord}
#
## STATUS
<value> clrStatusBG           =0x${clrMenuTitleBg}
<value> clrStatusFG           =0x${clrMenuTitleFg}
<value> clrStatusRed          =0x${clrButtonRedBg}
<value> clrStatusGreen        =0x${clrButtonGreenBg}
<value> clrStatusYellow       =0x${clrButtonYellowBg}
<value> clrStatusBlue         =0x${clrButtonBlueBg}
#
#
## DEFAULTCOVER
<value> localcover            =/music-local-headphone.png
<value> streamcover           =/music-stream-headphone.png
## FOR MPEGBACKGROUNDCOVER
<value> isMpeg                =0
<value> localbackground       =/themes/mpegs/music-AQUA-local.mpg
<value> streambackground      =/themes/mpegs/music-AQUA-local.mpg
<value> rows                  =7
<value> osdheight             =524
<value> osdwidth              =616
<value> osdtop                =50
<value> osdleft               =48
<value> mpgdif                =28" > $MusicThemeFile ;;

        *) echo "Sorry, der ${OSDSkin}-skin wird bisher nicht unterstützt aber das kann jeder ändern..."; exit 1 ;;
    esac

else # multi area
    ThemeName=${OSDSkin}-${OSDTheme}
    MusicThemeFile=../../../themes/osd-${ThemeName}.theme

    case $OSDSkin in
        EnigmaNG)
echo "<SKIN-ID>${ThemeName}
## LINE \"MUST\" END WITH THE COLORVALUE AND WITHOUT SPACES
## 0xAARRGGBB
#
#
## BACKGROUND
<value> clrBG                 =0x${clrBackground}
#
## TOP : 2 colors
<value> clrTopBG              =0x${clrTitleBg}
<value> clrTopFG              =0x${clrTitleFg}
#
## COVER : 2 colors
<value> clrCoverBG            =0x${clrAltBackground}
<value> clrCoverBar           =0x${clrBackground}
#
## ARTIST,RATING : 3 colors
<value> clrArtistBG           =0x${clrAltBackground}
<value> clrArtistFG           =0x${clrMenuTxtFg}
<value> clrRatingFG           =0x${clrButtonGreenBg}
#
## PLAYSTATUS : 3 colors
<value> clrPlayStatusBG       =0x${clrAltBackground}
<value> clrPlayStatusFG       =0x${clrBackground}
<value> clrInfoBG             =0x${clrAltBackground}
<value> clrInfoFG             =0x${clrBackground}
<value> clrProgressbarBG      =0x${clrBackground}
<value> clrProgressbarFG      =0x${clrReplayProgressSeen}
#
## TRACKLIST : 3 colors
<value> clrListTitle          =0x${clrReplayCurrent}
<value> clrListBG             =0x${clrAltBackground}
<value> clrListFG             =0x${clrMenuTxtFg}
#
## SYMBOLS : 3 colors
<value> clrSymbolBG           =0x${clrAltBackground}
<value> clrSymbolFG           =0x${clrSymbolInactive}
<value> clrSymbolActive       =0x${clrSymbolActive}
<value> clrRecordingActive    =0x${clrSymbolActive}
#
## STATUSBAR : 6 colors
<value> clrStatusBG           =0x${clrBottomBg}
<value> clrStatusFG           =0x${clrMenuTxtFg}
<value> clrStatusRed          =0x${clrButtonRedBg}
<value> clrStatusGreen        =0x${clrButtonGreenBg}
<value> clrStatusYellow       =0x${clrButtonYellowBg}
<value> clrStatusBlue         =0x${clrButtonBlueBg}
#
#
## DEFAULTCOVER
<value> localcover            =/music-local-headphone.png
<value> streamcover           =/music-stream-headphone.png
## FOR MPEGBACKGROUNDCOVER
<value> isMpeg                =0
<value> localbackground       =/themes/mpegs/music-AQUA-local.mpg
<value> streambackground      =/themes/mpegs/music-AQUA-local.mpg
<value> rows                  =7
<value> osdheight             =524
<value> osdwidth              =616
<value> osdtop                =50
<value> osdleft               =48
<value> mpgdif                =28" > $MusicThemeFile ;;

        soppalusikka)
echo "<SKIN-ID>${ThemeName}
## LINE \"MUST\" END WITH THE COLORVALUE AND WITHOUT SPACES
## 0xAARRGGBB
#
#
## BACKGROUND
<value> clrBG                 =0x${clrBackground}
#
## TOP : 2 colors
<value> clrTopBG              =0x${clrMenuTitleBg}
<value> clrTopFG              =0x${clrMenuTitleFg}
#
## COVER : 2 colors
<value> clrCoverBG            =0x${clrMenuTitleBg}
<value> clrCoverBar           =0x${clrBackground}
#
## ARTIST,RATING : 3 colors
<value> clrArtistBG           =0x${clrMenuTitleBg}
<value> clrArtistFG           =0x${clrMenuTitleFg}
<value> clrRatingFG           =0x${clrButtonGreenBg}
#
## PLAYSTATUS : 3 colors
<value> clrPlayStatusBG       =0x${clrMenuTitleBg}
<value> clrPlayStatusFG       =0x${clrBackground}
<value> clrInfoBG             =0x${clrMenuTitleBg}
<value> clrInfoFG             =0x${clrBackground}
<value> clrProgressbarBG      =0x${clrBackground}
<value> clrProgressbarFG      =0x${clrReplayProgressSeen}
#
## TRACKLIST : 3 colors
<value> clrListTitle          =0x${clrReplayCurrent}
<value> clrListBG             =0x${clrMenuTitleBg}
<value> clrListFG             =0x${clrMenuTitleFg}
#
## SYMBOLS : 3 colors
<value> clrSymbolBG           =0x${clrMenuTitleBg}
<value> clrSymbolFG           =0x${clrReplaySymbolInactive}
<value> clrSymbolActive       =0x${clrReplaySymbolActive}
<value> clrRecordingActive    =0x${clrReplaySymbolActive}
#
## STATUS : 6 different colors
<value> clrStatusBG           =0x${clrMenuTitleBg}
<value> clrStatusFG           =0x${clrMenuTitleFg}
<value> clrStatusRed          =0x${clrButtonRedBg}
<value> clrStatusGreen        =0x${clrButtonGreenBg}
<value> clrStatusYellow       =0x${clrButtonYellowBg}
<value> clrStatusBlue         =0x${clrButtonBlueBg}
#
#
## DEFAULTCOVER
<value> localcover            =/music-local-headphone.png
<value> streamcover           =/music-stream-headphone.png
## FOR MPEGBACKGROUNDCOVER
<value> isMpeg                =0
<value> localbackground       =/themes/mpegs/music-AQUA-local.mpg
<value> streambackground      =/themes/mpegs/music-AQUA-local.mpg
<value> rows                  =7
<value> osdheight             =524
<value> osdwidth              =616
<value> osdtop                =50
<value> osdleft               =48
<value> mpgdif                =28" > $MusicThemeFile ;;

        *) echo "Sorry, der ${OSDSkin}-skin wird bisher nicht unterstützt aber das kann jeder ändern..."; exit 1 ;;
    esac
fi

# cp -f $MusicThemeFile ../themes/current.colors
echo "osd-${ThemeName}.theme wurde erfolgreich erstellt.

Der neue Theme kann nun unter 'Befehle -> Aussehen ändern...' aktiviert werden."

exit 0
