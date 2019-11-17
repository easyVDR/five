#include "displaymenu.h"

cSDDisplayMenu::cSDDisplayMenu(cViewMenu *menuView) {
    view = menuView;
    bool ok = false;
    if (view)
        ok = view->Init();
    if (ok) {
        SetCurrentRecording();
    } else {
        esyslog("skindesigner: Error initiating displaymenu view - aborting");
    }
}

cSDDisplayMenu::~cSDDisplayMenu() {
    if (view)
        view->Close();
}

void cSDDisplayMenu::Scroll(bool Up, bool Page) {
    if (view)
        view->KeyDetailView(Up, Page);
}

int cSDDisplayMenu::MaxItems(void) {
    if (view)
        return view->NumListItems();
    return 0;
}

void cSDDisplayMenu::Clear(void) {
    if (view)
        view->Clear();
}

void cSDDisplayMenu::SetMenuCategory(eMenuCategory MenuCat) {
    if (view)
        view->SetSubView(MenuCat);
}

void cSDDisplayMenu::SetMenuSortMode(eMenuSortMode MenuSortMode) {
    if (view)
        view->SetSortMode(MenuSortMode);
}

eMenuOrientation cSDDisplayMenu::MenuOrientation(void) {
    if (view)
        return view->MenuOrientation();
    return moVertical;
}

void cSDDisplayMenu::SetPluginMenu(int plugId, int menuId, int type, bool init) {
    if (view)
        view->SetPluginMenu(plugId, menuId);
}

void cSDDisplayMenu::SetTitle(const char *Title) {
    if (view)
        view->SetTitleHeader(Title);
}

void cSDDisplayMenu::SetButtons(const char *Red, const char *Green, const char *Yellow, const char *Blue) {
    if (view)
        view->SetMenuButtons(Red, Green, Yellow, Blue);
}

void cSDDisplayMenu::SetMessage(eMessageType Type, const char *Text) {
    if (view)
        view->SetMessage(Type, Text);
}

bool cSDDisplayMenu::SetItemEvent(const cEvent *Event, int Index, bool Current, bool Selectable, const cChannel *Channel, bool WithDate, eTimerMatch TimerMatch) {
    if (!view)
        return false;
    if (Index == 0) {
        view->SetChannelHeader(Event);
    }
    return view->SetItemEvent(Event, Index, Current, Selectable, Channel, WithDate, TimerMatch);
}

bool cSDDisplayMenu::SetItemTimer(const cTimer *Timer, int Index, bool Current, bool Selectable) {
    if (!view)
        return false;
    return view->SetItemTimer(Timer, Index, Current, Selectable);
}

bool cSDDisplayMenu::SetItemChannel(const cChannel *Channel, int Index, bool Current, bool Selectable, bool WithProvider) {
    if (!view)
        return false;
    return view->SetItemChannel(Channel, Index, Current, Selectable, WithProvider);
}

bool cSDDisplayMenu::SetItemRecording(const cRecording *Recording, int Index, bool Current, bool Selectable, int Level, int Total, int New) {
    if (!view)
        return false;
    return view->SetItemRecording(Recording, Index, Current, Selectable, Level, Total, New);
}

void cSDDisplayMenu::SetItem(const char *Text, int Index, bool Current, bool Selectable) {
    if (!view)
        return;
    view->SetItem(Text, Index, Current, Selectable);
    SetEditableWidth(view->GetListWidth() / 2);
}

bool cSDDisplayMenu::SetItemPlugin(skindesignerapi::cTokenContainer *tk, int Index, bool Current, bool Selectable) {
    if (!view)
        return false;
    bool ok = view->SetItemPlugin(tk, Index, Current, Selectable);
    return ok;
}

void cSDDisplayMenu::SetTabs(int Tab1, int Tab2, int Tab3, int Tab4, int Tab5) {
    if (view)
        view->SetTabs(Tab1, Tab2, Tab3, Tab4, Tab5);
}

int cSDDisplayMenu::GetTextAreaWidth(void) const {
    if (view)
        return view->GetTextAreaWidth();
    return 0;
}

const cFont *cSDDisplayMenu::GetTextAreaFont(bool FixedFont) const {
    if (view)
        return view->GetTextAreaFont();
    return NULL;
}

void cSDDisplayMenu::SetScrollbar(int Total, int Offset) {
    if (view)
        view->SetScrollbar(Total, Offset);
}

void cSDDisplayMenu::SetEvent(const cEvent *Event) {
    if (view) {
        if (view->MenuCat() != mcEvent)
            view->SetSubView(mcEvent);
        view->SetEvent(Event);
    }
}

void cSDDisplayMenu::SetRecording(const cRecording *Recording) {
    if (view) {
        if (view->MenuCat() != mcRecordingInfo)
            view->SetSubView(mcRecordingInfo);
        view->SetRecording(Recording);
    }
}

void cSDDisplayMenu::SetText(const char *Text, bool FixedFont) {
    if (view) {
        if (view->MenuCat() != mcText)
            view->SetSubView(mcText);
        view->SetText(Text);
    }
}

bool cSDDisplayMenu::SetPluginText(skindesignerapi::cTokenContainer *tk) {
    bool ok = false;
    if (view)
        ok = view->SetPluginText(tk);
    return ok;
}

void cSDDisplayMenu::Flush(void) {
    if (view)
        view->Flush();
}

void cSDDisplayMenu::SetCurrentRecording(void) {
    cControl *control = cControl::Control();
    if (!control) {
        view->SetCurrentRecording(NULL);
        return;
    }
    const cRecording *recording = control->GetRecording();
    if (!recording) {
        view->SetCurrentRecording(NULL);
        return;
    }
    view->SetCurrentRecording(recording->FileName());
}
