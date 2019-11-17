#ifndef __DEFINITIONS_H
#define __DEFINITIONS_H

#include <string>
#include <map>

using namespace std;

typedef pair<string,string> stringpair;
typedef map<string,string> stringmap;

#define ATTR_UNKNOWN (-1)
#define ATTR_COND    (-2)

/******************************************************************
* Views
*******************************************************************/
enum class eViewType {
    DisplayChannel,
    DisplayMenu,
    DisplayReplay,
    DisplayVolume,
    DisplayTracks,
    DisplayMessage,
    DisplayPlugin
};
/******************************************************************
* ViewElements DisplayChannel
*******************************************************************/
enum class eVeDisplayChannel {
    background = 0,
    datetime,
    time,
    message,
    devices,
    currentweather,
    customtokens,
    channelinfo,
    channelgroup,
    epginfo,
    progressbar,
    statusinfo,
    audioinfo,
    screenresolution,
    signalquality,
    scrapercontent,
    ecminfo,
    channelhints,
    channeldetail,
    channellistback,
    channellistdetail,
    grouplistback,
    groupchannellistback,
    groupchannellistdetail,
    count
};
/******************************************************************
* ViewElements DisplayMenu
*******************************************************************/
enum class eVeDisplayMenu {
    background = 0,
    header,
    datetime,
    time,
    sortmode,
    colorbuttons,
    message,
    scrollbar,
    count
};

enum class eVeDisplayMenuMain {
    timers = (int)eVeDisplayMenu::count,
    devices,
    currentweather,
    currentschedule,
    discusage,
    systemload,
    systemmemory,
    temperatures,
    vdrstatistics,
    lastrecordings,
    customtokens,
    count
};

enum class eVeDisplayDetailedMenu {
    detailheader = (int)eVeDisplayMenu::count,
    tablabels,
    count
};
/******************************************************************
* Subviews DisplayMenu
*******************************************************************/
enum class eSvDisplayMenu {
    menudefault = 0,
    main,
    setup,
    schedules,
    channels,
    timers,
    recordings,
    detailepg,
    detailrec,
    detailtext,
    count
};

/******************************************************************
* DisplayMessage
*******************************************************************/
enum class eVeDisplayMessage {
    background = 0,
    message,
    count
};

/******************************************************************
* DisplayReplay
*******************************************************************/
enum class eVeDisplayReplay {
    background = 0,
    backgroundmodeonly,
    datetime,
    time,
    scrapercontent,
    currentweather,
    rectitle,
    recinfo,
    currenttime,
    totaltime,
    endtime,
    progressbar,
    cutmarks,
    controlicons,
    controliconsmodeonly,
    progressmodeonly,
    jump,
    message,
    onpause,
    onpausemodeonly,
    customtokens,
    count
};

/******************************************************************
* DisplayVolume
*******************************************************************/
enum class eVeDisplayVolume {
    background = 0,
    volume,
    count
};

/******************************************************************
* DisplayTracks
*******************************************************************/
enum class eVeDisplayTracks {
    background = 0,
    header,
    count
};

/******************************************************************
* Tokens common viewelements
*******************************************************************/
enum class eBackgroundST {
    count
};

enum class eBackgroundIT {
    count
};

enum class eDateTimeST {
    time = 0,
    dayleadingzero,
    dayname,
    daynameshort,
    month,
    monthname,
    monthnameshort,
    count
};

enum class eDateTimeIT {
    day = 0,
    year,
    count
};

enum class eTimeST {
    count = 0
};

enum class eTimeIT {
    sec = 0,
    min,
    hour,
    hmins,
    count
};

enum class eMessageST {
    text = 0,
    count
};

enum class eMessageIT {
    status = 0,
    info,
    warning,
    error,
    count
};

enum class eDevicesLT {
    num = 0,
    type,
    istuned,
    livetv,
    recording,
    hascam,
    cam,
    signalstrength,
    signalquality,
    channelnumber,
    channelname,
    channellogoexists, 
    channelid,
    source,
    count
};

enum class eDevicesIT {
    numdevices = 0,
    count
};

enum class eCurrentWeatherST {
    timestamp = 0,
    summary,
    icon,
    precipitationtype,
    windbearingstring,
    temperature,
    apparenttemperature,
    mintemperature,
    maxtemperature,
    precipitationintensity,
    windspeed,
    visibility,
    pressure,
    ozone,
    count
};

enum class eCurrentWeatherIT {
    precipitationprobability = 0,
    humidity,
    windbearing,
    cloudcover,
    count
};

enum class eScraperPosterBannerST {
    posterpath = 0,
    bannerpath,
    mediapath,
    count
};

enum class eScraperPosterBannerIT {
    posterwidth = 0,
    posterheight,
    hasposter,
    bannerwidth,
    bannerheight,
    hasbanner,
    mediawidth,
    mediaheight,
    isbanner,
    count
};

enum class eScraperST {
    movietitle = 0,
    movieoriginalTitle,
    movietagline,
    movieoverview,
    moviegenres,
    moviehomepage,
    moviereleasedate,
    moviepopularity,
    movievoteaverage,
    posterpath,
    fanartpath,
    moviecollectionName,
    collectionposterpath,
    collectionfanartpath,
    seriesname,
    seriesoverview,
    seriesfirstaired,
    seriesnetwork,
    seriesgenre,
    seriesrating,
    seriesstatus,
    episodetitle,
    episodefirstaired,
    episodegueststars,
    episodeoverview,
    episoderating,
    episodeimagepath,
    seasonposterpath,
    seriesposter1path,
    seriesposter2path,
    seriesposter3path,
    seriesfanart1path,
    seriesfanart2path,
    seriesfanart3path,
    seriesbanner1path,
    seriesbanner2path,
    seriesbanner3path,
    count
};

enum class eScraperIT {
    ismovie = 0,
    moviebudget,
    movierevenue,
    movieadult,
    movieruntime,
    isseries,
    posterwidth,
    posterheight,
    fanartwidth,
    fanartheight,
    movieiscollection,
    collectionposterwidth,
    collectionposterheight,
    collectionfanartwidth,
    collectionfanartheight,
    epgpicavailable,
    episodenumber,
    episodeseason,
    episodeimagewidth,
    episodeimageheight,
    seasonposterwidth,
    seasonposterheight,
    seriesposter1width,
    seriesposter1height,
    seriesposter2width,
    seriesposter2height,
    seriesposter3width,
    seriesposter3height,
    seriesfanart1width,
    seriesfanart1height,
    seriesfanart2width,
    seriesfanart2height,
    seriesfanart3width,
    seriesfanart3height,
    seriesbanner1width,
    seriesbanner1height,
    seriesbanner2width,
    seriesbanner2height,
    seriesbanner3width,
    seriesbanner3height,
    count
};

enum class eScraperLT {
    //actors
    name = 0,
    role,
    thumb,
    thumbwidth,
    thumbheight,
    count
};

/******************************************************************
* Tokens displaychannel viewelements
*******************************************************************/
enum class eDCChannelInfoST {
    channelnumber = 0,
    channelname,
    channelid,
    count
};

enum class eDCChannelInfoIT {
    channellogoexists = 0,
    switching,
    count
};

enum class eDCEpgInfoST {
    currenttitle = 0,
    currentsubtitle,
    currentstart,
    currentstop,
    nexttitle,
    nextsubtitle,
    nextstart,
    nextstop,
    count
};

enum class eDCEpgInfoIT {
    currentduration = 0,
    currentdurationhours,
    currentdurationminutes,
    currentelapsed,
    currentremaining,
    nextduration,
    nextdurationhours,
    nextdurationminutes,
    nextrecording,
    currentrecording,
    hasVPS,
    count
};

enum class eDCProgressBarST {
    start = 0,
    stop,
    count
};

enum class eDCProgressBarIT {
    duration = 0,
    elapsed,
    remaining,
    permashift,
    livebuffer,
    count
};

enum class eDCStatusInfoST {
    count
};

enum class eDCStatusInfoIT {
    isRadio = 0,
    hasVT,
    isStereo,
    isDolby,
    isEncrypted,
    isRecording,
    newmails,
    count
};

enum class eDCAudioInfoST {
    trackdesc = 0,
    tracklang,
    count
};

enum class eDCAudioInfoIT {
    numaudiotracks = 0,
    audiochannel,
    count
};

enum class eDCEcmInfoST {
    cardsystem = 0,
    reader,
    from,
    protocol,
    count
};

enum class eDCEcmInfoIT {
    caid = 0,
    pid,
    prid,
    ecmtime,
    hops,
    count
};

enum class eDCScreenResolutionST {
    resolution = 0,
    aspect,
    count
};

enum class eDCScreenResolutionIT {
    screenwidth = 0,
    screenheight,
    isHD,
    isWideScreen,
    count
};

enum class eDCChannelGroupST {
    group = 0,
    nextgroup,
    prevgroup,
    seppath,
    count
};

enum class eDCChannelGroupIT {
    prevAvailable = 0,
    nextAvailable,
    sepexists,
    count
};

enum class eDCSignalQualityST {
    count = 0
};

enum class eDCSignalQualityIT {
    signalstrength = 0,
    signalquality,
    count
};

enum class eDCChannelHintsIT {
    numhints = 0,
    count
};

enum class eDCChannelHintsLT {
    channelnumber = 0,
    channelname,
    channelid,
    channellogoexists,
    count
};

enum class eDCChannelDetailST {
    channelname = (int)eScraperST::count,
    currenttitle,
    currentshorttext,
    currentdescription,
    currentstart,
    currentstop,
    currentdurationminutes,
    nexttitle,
    nextshorttext,
    nextdescription,
    nextstart,
    nextstop,
    nextdurationminutes,
    count
};

enum class eDCChannelDetailIT {
    channelnumber = (int)eScraperIT::count,
    currentduration,
    currentdurationhours,
    currentelapsed,
    currentremaining,
    nextduration,
    nextdurationhours,
    count
};
/******************************************************************
* Tokens displaymenu viewelements
*******************************************************************/
enum class eDMHeaderST {
    title = 0,
    vdrversion,
    icon,
    freetime,
    vdrusagestring,
    channelname,
    channelid,
    count
};

enum class eDMHeaderIT {
    hasicon = 0,
    freepercent,
    usedpercent,
    freegb,
    discalert,
    channelnumber,
    channellogoexists,
    whatson,
    whatsonnow,
    whatsonnext,
    whatsonfavorites,
    count
};

enum class eDMSortmodeST {
    count = 0
};

enum class eDMSortmodeIT {
    sortnumber = 0,
    sortname,
    sorttime,
    sortprovider,
    count
};

enum class eDMColorbuttonsST {
    red = 0,
    green,
    yellow,
    blue,
    count
};

enum class eDMColorbuttonsIT {
    red1 = 0, red2, red3, red4,
    green1, green2, green3, green4,
    yellow1, yellow2, yellow3, yellow4,
    blue1, blue2, blue3, blue4,
    count
};

enum class eDMScrollbarST {
    count = 0
};

enum class eDMScrollbarIT {
    height = 0,
    offset,
    hasprev,
    hasnext,
    count
};

enum class eDMTimersIT {
    numtimers = 0,
    numtimerconflicts,
    timer1exists,  timer2exists,  timer3exists,  timer4exists,  timer5exists,
    timer6exists,  timer7exists,  timer8exists,  timer9exists,  timer10exists,
    timer11exists, timer12exists, timer13exists, timer14exists, timer15exists,
    count
};

enum class eDMTimersLT {
    title = 0,
    datetime,
    recording,
    channelname,
    channelnumber,
    channelid,
    channellogoexists,
    isremotetimer,
    remotehost,
    count
};

enum class eDMCurrentscheduleIT {
    islivetv = (int)eScraperPosterBannerIT::count,
    duration,
    durationhours,
    minutes,
    elapsed,
    remaining,
    count
};

enum class eDMCurrentscheduleST {
    title = (int)eScraperPosterBannerST::count,
    subtitle,
    start,
    stop,
    durationminutes,
    count
};

enum class eDMLastrecordingsIT {
    numtimers = 0,
    count
};

enum class eDMLastrecordingsLT {
    name = 0,
    seriesname,
    date,
    time,
    duration,
    hasposter,
    posterpath,
    posterwidth,
    posterheight,
    count
};

enum class eDMDiscusageST {
    freetime = 0,
    vdrusagestring,
    count
};

enum class eDMDiscusageIT {
    freepercent = 0,
    usedpercent,
    freegb,
    discalert,
    count
};

enum class eDMSystemloadST {
    load = 0,
    count
};

enum class eDMSystemloadIT {
    loadhand = 0,
    count
};

enum class eDMSystemmemoryST {
    count = 0
};

enum class eDMSystemmemoryIT {
    totalmem = 0,
    usedmem,
    usedmempercent,
    count
};

enum class eDMVdrstatisticsST {
    vdrcpu = 0,
    vdrmem,
    count
};

enum class eDMVdrstatisticsIT {
    count = 0
};

enum class eDMTemperaturesST {
    count = 0
};

enum class eDMTemperaturesIT {
    cputemp = 0,
    gputemp,
    count
};

/******************************************************************
* Tokens listelements
*******************************************************************/
enum class eLeMenuDefaultST {
    column1 = 0,
    column2,
    column3,
    column4,
    column5,
    column6,
    count
};

enum class eLeMenuDefaultIT {
    nummenuitem = 0,
    current,
    separator,
    column2set,
    column3set,
    column4set,
    column5set,
    column6set,
    column1x,
    column2x,
    column3x,
    column4x,
    column5x,
    column6x,
    column1width,
    column2width,
    column3width,
    column4width,
    column5width,
    column6width,
    column1pb,
    column2pb,
    column3pb,
    column4pb,
    column5pb,
    column6pb,
    column1pbtotalsize,
    column2pbtotalsize,
    column3pbtotalsize,
    column4pbtotalsize,
    column5pbtotalsize,
    column6pbtotalsize,
    column1pbsize,
    column2pbsize,
    column3pbsize,
    column4pbsize,
    column5pbsize,
    column6pbsize,
    unknown,
    channeledit,
    timeredit,
    recordinginfo,
    recordingedit,
    setup,
    commands,
    folder,
    cam,
    fritzbox,
    systeminfo,
    mailbox,
    neutrinoepg,
    remotetimers,
    zaphistory,
    remoteosd,
    filebrowser,
    epgsearch,
    count
};

enum class eLeMenuMainST {
    label = 0,
    number,
    icon,
    count
};

enum class eLeMenuMainIT {
    nummenuitem = 0,
    current,
    separator,
    count
};

enum class eCeMenuMainST {
    label = 0,
    number,
    icon,
    count
};

enum class eCeMenuMainIT {
    menuitemx = 0,
    menuitemy,
    menuitemwidth,
    menuitemheight,
    numitem,
    count
};

enum class eLeMenuSchedulesST {
    title = 0,
    shorttext,
    start,
    stop,
    day,
    date,
    durationminutes,
    channelname,
    channelid,
    count
};

enum class eLeMenuSchedulesIT {
    nummenuitem = 0,
    current,
    separator,
    daynumeric,
    month,
    year,
    istoday,
    running,
    elapsed,
    startsin,
    duration,
    durationhours,
    channelnumber,
    channellogoexists,
    whatson,
    whatsonnow,
    whatsonnext,
    whatsonfavorites,
    timerpartitial,
    timerfull,
    hasVPS,
    count
};

enum class eCeMenuSchedulesST {
    title = 0,
    shorttext,
    description,
    start,
    stop,
    day,
    date,
    channelname,
    channelid,
    posterpath,
    bannerpath,
    count
};

enum class eCeMenuSchedulesLT {
    title = 0,
    shorttext,
    start,
    stop,
    count
};

enum class eCeMenuSchedulesIT {
    menuitemx = 0,
    menuitemy,
    menuitemwidth,
    menuitemheight,
    numitem,
    daynumeric,
    month,
    year,
    running,
    elapsed,
    duration,
    durationhours,
    durationminutes,
    channelnumber,
    channellogoexists,
    hasposter,
    posterwidth,
    posterheight,
    hasbanner,
    bannerwidth,
    bannerheight,
    whatson,
    whatsonnow,
    whatsonnext,
    whatsonfavorites,
    timerpartitial,
    timerfull,
    count
};

enum class eLeMenuChannelsST {
    name = 0,
    channelid,
    source,
    sourcedescription,
    position,
    presenteventtitle,
    presenteventstart,
    presenteventstop,
    count
};

enum class eLeMenuChannelsIT {
    nummenuitem = 0,
    current,
    separator,
    number,
    transponder,
    frequency,
    channellogoexists,
    isAtsc,
    isCable,
    isSat,
    isTerr,
    isEncrypted,
    count
};

enum class eCeMenuChannelsST {
    name = 0,
    channelid,
    source,
    sourcedescription,
    position,
    presenteventtitle,
    presenteventstart,
    presenteventstop,
    presenteventshorttext,
    presenteventdescription,
    presenteventday,
    presenteventdate,
    presenteventdurationminutes,
    posterpath,
    bannerpath,
    nexteventtitle,
    nexteventstart,
    nexteventstop,
    nexteventshorttext,
    nexteventdescription,
    nexteventday,
    nexteventdate,
    nexteventdurationminutes,
    count
};

enum class eCeMenuChannelsIT {
    menuitemx = 0,
    menuitemy,
    menuitemwidth,
    menuitemheight,
    numitem,
    number,
    transponder,
    frequency,
    channellogoexists,
    isAtsc,
    isCable,
    isSat,
    isTerr,
    isEncrypted,
    presenteventduration,
    presenteventdurationhours,
    presenteventelapsed,
    hasposter,
    posterwidth,
    posterheight,
    hasbanner,
    bannerwidth,
    bannerheight,
    nexteventduration,
    nexteventdurationhours,
    count
};

enum class eCeMenuChannelsLT {
    title = 0,
    shorttext,
    start,
    stop,
    count
};

enum class eLeMenuTimersST {
    title = 0,
    timerstart,
    timerstop,
    day,
    dayname,
    monthname,
    channelname,
    channelid,
    eventtitle,
    eventstart,
    eventstop,
    state,
    stateinfo,
    action,
    vdrname,
    count
};

enum class eLeMenuTimersIT {
    nummenuitem = 0,
    current,
    separator,
    month,
    year,
    channellogoexists,
    channelnumber,
    flagactive,
    flaginstant,
    flagvps,
    flagrecording,
    flagpending,
    isvdrrunning,
    isremote,
    count
};

enum class eCeMenuTimersST {
    title = 0,
    timerstart,
    timerstop,
    day,
    dayname,
    monthname,
    channelname,
    channelid,
    eventtitle,
    eventstart,
    eventstop,
    eventshorttext,
    eventdescription,
    posterpath,
    bannerpath,
    state,
    stateinfo,
    action,
    vdrname,
    count
};

enum class eCeMenuTimersIT {
    menuitemx = 0,
    menuitemy,
    menuitemwidth,
    menuitemheight,
    numitem,
    month,
    year,
    channellogoexists,
    channelnumber,
    flagactive,
    flaginstant,
    flagvps,
    flagrecording,
    flagpending,
    hasposter,
    posterwidth,
    posterheight,
    hasbanner,
    bannerwidth,
    bannerheight,
    isvdrrunning,
    isremote,
    count
};

enum class eLeMenuRecordingsST {
    name = 0,
    epgname,
    shorttext,
    description,
    date,
    time,
    durationminutes,
    durationeventminutes,
    thumbnailpath,
    posterpath,
    count
};

enum class eLeMenuRecordingsIT {
    nummenuitem = 0,
    current,
    separator,
    isnew,
    percentseen,
    watched,
    cutted,
    folder,
    daynumeric,
    month,
    year,
    duration,
    durationhours,
    durationevent,
    durationeventhours,
    numrecordingsfolder,
    newrecordingsfolder,
    hasposterthumbnail,
    thumbnailwidth,
    thumbnailheight,
    hasposter,
    posterwidth,
    posterheight,
    count
};

enum class eCeMenuRecordingsST {
    name = 0,
    epgname,
    shorttext,
    description,
    date,
    time,
    durationminutes,
    durationeventminutes,
    thumbnailpath,
    posterpath,
    count
};

enum class eCeMenuRecordingsIT {
    menuitemx = 0,
    menuitemy,
    menuitemwidth,
    menuitemheight,
    numitem,
    isnew,
    percentseen,
    watched,
    cutted,
    folder,
    daynumeric,
    month,
    year,
    duration,
    durationhours,
    durationevent,
    durationeventhours,
    numrecordingsfolder,
    newrecordingsfolder,
    hasposterthumbnail,
    thumbnailwidth,
    thumbnailheight,
    hasposter,
    posterwidth,
    posterheight,
    count
};

enum class eLeChannelListST {
    name = 0,
    channelid,
    presenteventtitle,
    presenteventstart,
    presenteventstop,
    nexteventtitle,
    nexteventstart,
    nexteventstop,
    count
};

enum class eLeChannelListIT {
    nummenuitem = 0,
    current,
    number,
    channellogoexists,
    presenteventelapsed,
    presenteventremaining,
    count
};

enum class eLeGroupListST {
    groupname = 0,
    count
};

enum class eLeGroupListIT {
    nummenuitem = 0,
    current,
    numchannels,
    count
};
/******************************************************************
* Tokens displaymenu detailed views
*******************************************************************/
enum class eScraperHeaderST {
    posterpath = 0,
    bannerpath,
    count
};

enum class eScraperHeaderIT {
    ismovie = 0,
    isseries,
    posteravailable,
    posterwidth,
    posterheight,
    banneravailable,
    bannerwidth,
    bannerheight,
    count
};

enum class eDmDetailedHeaderEpgST {
    title = (int)eScraperHeaderST::count,
    shorttext,
    start,
    stop,
    day,
    date,
    durationminutes,
    vps,
    channelname,
    channelid,
    epgpicpath,
    count
};

enum class eDmDetailedHeaderEpgIT {
    daynumeric = (int)eScraperHeaderIT::count,
    month,
    year,
    running,
    elapsed,
    duration,
    durationhours,
    channelnumber,
    channellogoexists,
    epgpicavailable,
    count
};

enum class eDmDetailedHeaderRecST {
    name = (int)eScraperHeaderST::count,
    epgname,
    shorttext,
    date,
    time,
    recimgpath,
    recchannelname,
    recchannelid,
    durationminutes,
    durationeventminutes,
    count
};

enum class eDmDetailedHeaderRecIT {
    daynumeric = (int)eScraperHeaderIT::count,
    month,
    year,
    duration,
    durationhours,
    durationevent,
    durationeventhours,
    recimgavailable,
    recchannelnumber,
    count
};

enum class eDmTabsIT {
    count = 0,
};

enum class eDmTabsST {
    currenttab = 0,
    prevtab,
    nexttab,
    count
};

enum class eDmTabsLT {
    title = 0,
    current,
    count
};

enum class eDmDetailedEpgST {
    title = (int)eScraperST::count,
    shorttext,
    description,
    start,
    stop,
    day,
    date,
    durationminutes,
    vps,
    channelname,
    channelid,
    epgpic1path,
    epgpic2path,
    epgpic3path,
    count
};

enum class eDmDetailedEpgIT {
    daynumeric = (int)eScraperIT::count,
    month,
    year,
    running,
    elapsed,
    duration,
    durationhours,
    channelnumber,
    channellogoexists,
    hasreruns,
    epgpic1available,
    epgpic2available,
    epgpic3available,
    count
};

enum class eRerunsLT {
    title = 0,
    shorttext,
    date,
    day,
    start,
    stop,
    channelname,
    channelnumber,
    channelid,
    channellogoexists,
    count
};

enum class eDmDetailedRecST {
    name = (int)eScraperST::count,
    epgname,
    shorttext,
    description,
    day,
    date,
    time,
    durationminutes,
    durationeventminutes,
    recordingsize,
    recordingsizecutted,
    recordinglength,
    recordinglengthcutted,
    recordingbitrate,
    recordingformat,
    recchannelname,
    recchannelid,
    searchtimer,
    recimg1path,
    recimg2path,
    recimg3path,
    count
};

enum class eDmDetailedRecIT {
    daynumeric = (int)eScraperIT::count,
    month,
    year,
    duration,
    durationhours,
    durationevent,
    durationeventhours,
    cutted,
    recchannelnumber,
    recimg1avaialble,
    recimg2avaialble,
    recimg3avaialble,
    count
};

enum class eDmDetailedTextST {
    text = 0,
    count
};

enum class eDmSkinPreviewST {
    menuheader = 0,
    skinname,
    author,
    count
};

enum class eDmSkinPreviewFontsLT {
    name = 0,
    installed,
    count
};

enum class eDmSkinPreviewPluginsLT {
    name = 0,
    count
};

enum class eDmSkinPreviewScreenshotsLT {
    desc = 0,
    path,
    count
};
/******************************************************************
* Tokens DisplayReplay Viewelements
*******************************************************************/
enum class eDRRecTitleST {
    rectitle = 0,
    recsubtitle,
    recdate,
    rectime,
    count
};

enum class eDRRecInfoST {
    resolution = 0,
    aspect,
    count
};

enum class eDRRecInfoIT {
    screenwidth = 0,
    screenheight,
    isHD,
    isWideScreen,
    count
};

enum class eDRCurrentTimeST {
    reccurrent = 0,
    count
};

enum class eDRTotalTimeST {
    rectotal = 0,
    timeshifttotal,
    count
};

enum class eDRTotalTimeIT {
    timeshift = 0,
    count
};

enum class eDREndTimeST {
    recend = 0,
    count
};

enum class eDRProgressbarIT {
    current = 0,
    total,
    timeshift,
    timeshifttotal,
    count
};

enum class eDRCutmarksIT {
    timeshift = 0,
    count
};

enum class eDRCutmarksLT {
    position = 0,
    endposition,
    total,
    timeshifttotal,
    active,
    startmark,
    count
};

enum class eDRControlIconsIT {
    play = 0,
    pause,
    forward,
    slowforward,
    forward1x,
    forward2x,
    forward3x,
    slowforward1x,
    slowforward2x,
    slowforward3x,
    rewind,
    slowrewind,
    rewind1x,
    rewind2x,
    rewind3x,
    slowrewind1x,
    slowrewind2x,
    slowrewind3x,
    count
};

enum class eDRProgressModeonlyIT {
    current = 0,
    total,
    count
};

enum class eDRProgressModeonlyST {
    timecurrent = 0,
    timetotal,
    count
};

enum class eDRJumpST {
    jump = 0,
    count
};

enum class eDROnpauseST {
    name = (int)eScraperST::count,
    shorttext,
    description,
    date,
    time,
    durationminutes,
    durationeventminutes,
    count
};

enum class eDROnpauseIT {
    daynumeric = (int)eScraperIT::count,
    month,
    year,
    duration,
    durationhours,
    durationevent,
    durationeventhours,
    count
};

/******************************************************************
* Tokens DisplayVolume Viewelements
*******************************************************************/
enum class eDVVolumeIT {
    volume = 0,
    volpercent,
    maxvolume,
    mute,
    count
};

/******************************************************************
* Tokens DisplayTracks Viewelements
*******************************************************************/
enum class eDTBackgroundIT {
    numtracks = 0,
    count
};

enum class eDTHeaderIT {
    numtracks = 0,
    isstereo,
    isac3,
    count
};

enum class eDTHeaderST {
    title = 0,
    count
};

enum class eLeDisplayTracksIT {
    nummenuitem = 0,
    current,
    count
};

enum class eLeDisplayTracksST {
    title = 0,
    count
};
/******************************************************************
* Attributes
*******************************************************************/
enum class eCommonAttribs {
    x = 0,
    y,
    width, 
    height,
    debug,
    count
};

enum class eViewAttribs {
    fadetime = 0,
    shifttime, 
    shifttype,
    shiftmode,
    startx,
    starty,
    scaletvx,
    scaletvy,
    scaletvwidth,
    scaletvheight,
    orientation,
    debuggrid,
    hideroot,
    count
};

enum class eViewElementAttribs {
    delay = 0,
    fadetime,
    shifttime, 
    shifttype,
    shiftmode,
    startx,
    starty,
    mode,
    orientation,
    name,
    clearondisplay,
    count
};

enum class eViewListAttribs {
    align = 0,
    menuitemwidth,
    determinatefont,
    numlistelements, 
    orientation,
    fadetime,
    shifttime, 
    shifttype,
    shiftmode,
    startx,
    starty,
    condition,
    button,
    count
};

enum class eAreaAttribs {
    layer = 0,
    transparency,
    mode,
    orientation, 
    scrollelement,
    scrollspeed,
    delay,
    background,
    indicator,
    name,
    scrollheight,
    count
};

enum class eAreaContainerAttribs {
    count = 0
};

/******************************************************************
* Functions
*******************************************************************/
enum class eFillAttribs {
    color = 0,
    count
};

enum class eDrawRectangleAttribs {
    align = 0,
    valign,
    color,
    name,
    animtype,
    animfreq,
    count
};

enum class eDrawEllipseAttribs {
    align = 0,
    valign,
    color,
    name,
    quadrant,
    animtype,
    animfreq,
    count
};

enum class eDrawSlopeAttribs {
    align = 0,
    valign,
    color,
    name,
    type,
    animtype,
    animfreq,
    count
};

enum class eDrawTextAttribs {
    align = 0,
    valign,
    color,
    font,
    fontsize,
    name,
    text,
    animtype,
    animfreq,
    count
};

enum class eDrawTextAttribsVertical {
    align = 0,
    valign,
    direction,
    color,
    font,
    fontsize,
    name,
    text,
    animtype,
    animfreq,
    count
};

enum class eDrawTextBoxAttribs {
    align = 0,
    valign,
    maxlines,
    floatwidth,
    floatheight,
    floatmode,
    color,
    font,
    fontsize,
    name,
    text,
    count
};

enum class eDrawImageAttribs {
    align = 0,
    valign,
    imagetype,
    name,
    cache,
    path,
    animtype,
    animfreq,
    count
};

enum class eLoopAttribs {
    columnwidth = 0,
    rowheight,
    name,
    orientation,
    valign,
    overflow,
    maxitems,
    count
};

/******************************************************************
* Common Parameters
*******************************************************************/
enum class eViewElementMode {
    regular,
    light
};

enum class eShiftType {
    none = -1,
    left,
    right,
    top,
    bottom
};

enum class eShiftMode {
    linear,
    slowedDown
};

enum class eScrollMode {
    none,
    carriagereturn,
    forthandback
};

enum class eScrollSpeed {
    none,
    slow,
    medium,
    fast
};

enum class eOrientation {
    none,
    horizontal,
    vertical,
    absolute
};

enum class eAlign {
    left,
    center,
    right,
    top,
    bottom
};

enum class eAnimType {
    none,
    blink,
    animated
};

enum class eImageType {
    channellogo,
    seplogo,
    skinpart,
    menuicon,
    icon,
    image
};

enum class eOverflowType {
    none,
    wrap,
    cut
};

enum class eFloatMode {
    none = -1,
    topleft,
    topright
};

enum class eDirection {
    none = -1,
    bottomup,
    topdown
};

enum class eButtonType {
    none = -1,
    left,
    right
};

#endif //__DEFINITIONS_H
