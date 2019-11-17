#!/bin/bash

echo "*********************************"
echo "* theme generator version 0.0.2 *"
echo "* for skinflat version = 0.0.2  *"
echo "*********************************"

echo ""
echo "all colors must be of the form AARRGGBB"
echo ""

echo "simple mode: "
echo "  you only need 4 colors"
echo "    + Backround "
echo "    + Foreground (Font, Scrollbar, Progressbar) "
echo "    + Current (current menu item background) "
echo "    + Nonselable (nonselable menu item background) "
echo ""
echo "normale mode: "
echo "   you need 10 colors"

while [ 1 ]
do
    echo ""
    echo -n "which mode you want s (simple) / n (normal): "
    read Mode

    if [ "$Mode" != "s" ] && [ "$Mode" != "n" ]
    then
        echo "wrong answer, just type \"s\" or \"n\"!"
    else
        break
    fi
done

if [ "$Mode" == "s" ]
then
    echo -n "Name: "
    read ThemeName
    echo -n "Background color: "
    read Background
    echo -n "Foreground color: "
    read Foreground
    echo -n "Menu item current background color: "
    read ItemCurBG
    echo -n "Menu item not selectable background color: "
    read ItemBG
    
    Font=$Foreground
    TitleFont=$Foreground
    ProgressBarFG=$Foreground
    ProgressFG=$Foreground
    ScrollbarBarFG=$Foreground
    ScrollbarFG=$Foreground
    ItemSelableBG=$Background

else
    echo -n "Name: "
    read ThemeName
    echo -n "Background color: "
    read Background
    echo -n "Font color: "
    read Font
    echo -n "Title font color: "
    read TitleFont
    echo -n "Menu item (not selectable) background color: "
    read ItemBG
    echo -n "Menu item current/selected background color: "
    read ItemCurBG
    echo -n "Menu item selectable background color: "
    read ItemSelableBG
    echo -n "Progressbar bar foreground color: "
    read ProgressBarFG
    echo -n "Progressbar foreground color: "
    read ProgressFG
    echo -n "Scrollbar bar foreground color: "
    read ScrollbarBarFG
    echo -n "Scrollbar foreground color: "
    read ScrollbarFG
fi

FILE=flat-${ThemeName}.theme

echo "Description = $ThemeName" > $FILE
echo "clrTopBarBg = $Background" >> $FILE
echo "clrTopBarFont = $Font" >> $FILE
echo "clrTopBarTimeFont = $Font" >> $FILE
echo "clrTopBarDateFont = $Font" >> $FILE
echo "clrButtonBg = $Background" >> $FILE
echo "clrButtonFont = $Font" >> $FILE
echo "clrButtonRed = FFBB0000" >> $FILE
echo "clrButtonGreen = FF00BB00" >> $FILE
echo "clrButtonYellow = FFBBBB00" >> $FILE
echo "clrButtonBlue = FF0000BB" >> $FILE
echo "clrMessageBg = $Background" >> $FILE
echo "clrMessageFont = $Font" >> $FILE
echo "clrMessageStatus = FF0000FF" >> $FILE
echo "clrMessageInfo = FF009900" >> $FILE
echo "clrMessageWarning = FFBBBB00" >> $FILE
echo "clrMessageError = FFBB0000" >> $FILE
echo "clrChannelBg = $Background" >> $FILE
echo "clrChannelFontTitle = $TitleFont" >> $FILE
echo "clrChannelFontEpg = $Font" >> $FILE
echo "clrChannelFontEpgFollow = $Font" >> $FILE
echo "clrChannelProgressFg = $ProgressFG" >> $FILE
echo "clrChannelProgressBarFg = $ProgressBarFG" >> $FILE
echo "clrChannelProgressBg = $Background" >> $FILE
echo "clrItemBg = $ItemBG" >> $FILE
echo "clrItemFont = $Font" >> $FILE
echo "clrItemCurrentBg = $ItemCurBG" >> $FILE
echo "clrItemCurrentFont = $Font" >> $FILE
echo "clrItemSelableBg = $ItemSelableBG" >> $FILE
echo "clrItemSelableFont = $Font" >> $FILE
echo "clrScrollbarFg = $ScrollbarFG" >> $FILE
echo "clrScrollbarBarFg = $ScrollbarBarFG" >> $FILE
echo "clrScrollbarBg = $Background" >> $FILE
echo "clrMenuEventBg = $Background" >> $FILE
echo "clrMenuEventFontTitle = $TitleFont" >> $FILE
echo "clrMenuEventFontInfo = $Font" >> $FILE
echo "clrMenuRecBg = $Background" >> $FILE
echo "clrMenuRecFontTitle = $TitleFont" >> $FILE
echo "clrMenuRecFontInfo = $Font" >> $FILE
echo "clrMenuTextBg = $Background" >> $FILE
echo "clrMenuTextFont = $Font" >> $FILE
echo "clrReplayBg = $Background" >> $FILE
echo "clrReplayFont = $Font" >> $FILE
echo "clrReplayFontSpeed = $Font" >> $FILE
echo "clrReplayProgressFg = $ProgressFG" >> $FILE
echo "clrReplayProgressBarFg = $ProgressBarFG" >> $FILE
echo "clrReplayProgressBarCurFg = $ProgressBarFG" >> $FILE
echo "clrReplayProgressBg = $Background" >> $FILE
echo "clrReplayMarkFg = $ProgressFG" >> $FILE
echo "clrReplayMarkCurrentFg = $ProgressFG" >> $FILE
echo "clrTrackItemBg = $ItemBG" >> $FILE
echo "clrTrackItemFont = $Font" >> $FILE
echo "clrTrackItemCurrentBg = $ItemCurBG" >> $FILE
echo "clrTrackItemCurrentFont = $Font" >> $FILE
echo "clrVolumeBg = $Background" >> $FILE
echo "clrVolumeFont = $Font" >> $FILE
echo "clrVolumeProgressFg = $ProgressFG" >> $FILE
echo "clrVolumeProgressBarFg = $ProgressBarFG" >> $FILE
echo "clrVolumeProgressBg = $Background" >> $FILE

echo ""
echo "Theme <${FILE}> saved"
echo "Feel free to adjust single colors in the file"
echo "you must manually move it to your vdr config to test it"
echo "for example: mv ${FILE} /etc/vdr/themes/"
echo ""
echo "Remember, You must provide icons for your theme"
echo "look at <vdrconfig>/plugins/skinflat/icons/default/"
