#include "displaymessage.h"

cSDDisplayMessage::cSDDisplayMessage(cViewMessage *messageView) {
    view = messageView;
    ok = view->Init();
    if (!ok)
        esyslog("skindesigner: Error initiating displaymessage view - aborting");
}

cSDDisplayMessage::~cSDDisplayMessage() {
    view->Close();
}

void cSDDisplayMessage::SetMessage(eMessageType Type, const char *Text) {
    if (!ok)
        return;
    view->SetMessage(Type, Text);
}


void cSDDisplayMessage::Flush(void) {
    if (!ok)
        return;
    view->Flush();
}
