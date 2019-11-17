#ifndef __VIEWDISPLAYREPLAY_H
#define __VIEWDISPLAYREPLAY_H

#include "view.h"

class cViewReplay : public cView {
private:
    cVeMessage             *veMessage;
    cVeCustomTokens        *veCustomTokens;
    cVeDrRecTitle          *veRecTitle;
    cVeDrRecInfo           *veRecInfo;
    cVeDrScraperContent    *veScraperContent;
    cVeDrCurrentTime       *veCurrentTime;
    cVeDrTotalTime         *veTotalTime;
    cVeDrEndTime           *veEndTime;
    cVeDrProgressBar       *veProgressbar;
    cVeDrCutMarks          *veCutMarks;
    cVeDrProgressModeonly  *veProgressModeOnly;
    cVeDrControlIcons      *veControlIcons;
    cVeDrControlIcons      *veControlIconsModeOnly;
    cVeDrJump              *veJump;
    cVeDrOnPause           *veOnPause;
    cVeDrOnPause           *veOnPauseModeOnly;
    bool modeOnly;
    time_t lastFlush;
    time_t lastFlushModeOnly;
    bool message;
    int reclength;
    bool timeShiftActive;
    int timeShiftFramesTotal;
    int timeShiftLength;
    cString timeShiftDuration;
    void SetViewElements(void);
    void ClearVariables(void);
    void SetViewElementObjects(void);
    void SetProgressModeOnly(void);
public:
    cViewReplay(void);
    virtual ~cViewReplay(void);
    void PreCache(void);
    void SetModeOnly(bool modeOnly) { this->modeOnly = modeOnly; };
    void SetRecordingLength(int length) { reclength = length; };
    void SetTimeShift(int framesTotal, int timeShiftLength);
    void SetRecording(const cRecording *recording);
    void SetTitle(const char *title);
    void SetCurrent(const char *current);
    void SetTotal(const char *total);
    void SetEndTime(int current, int total);
    void SetProgressbar(int current, int total);
    void SetMarks(const cMarks *marks, int current, int total);
    void SetControlIcons(bool play, bool forward, int speed);
    void SetJump(const char *jump);
    void SetMessage(eMessageType type, const char *text);
    void StartOnPause(const char *recfilename);
    void ClearOnPause(void);
    void DelayOnPause(void);
    void Flush(void);
};

#endif //__VIEWDISPLAYREPLAY_H1