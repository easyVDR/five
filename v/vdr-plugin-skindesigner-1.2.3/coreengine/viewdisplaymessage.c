#include "viewdisplaymessage.h"
#include "../config.h"

/************************************************************************************
* cViewMessage
************************************************************************************/

cViewMessage::cViewMessage(void) {
    ClearVariables();
    viewId = eViewType::DisplayMessage;
    viewName = strdup("displaymessage");
    numViewElements = (int)eVeDisplayMessage::count;
    viewElements = new cViewElement*[numViewElements];
    for (int i=0; i < numViewElements; i++) {
        viewElements[i] = NULL;
    }
    SetViewElements();
    veMessage = NULL;
}

cViewMessage::~cViewMessage() {
}

void cViewMessage::SetViewElements(void) {
    viewElementNames.insert(pair<string, int>("background", (int)eVeDisplayMessage::background));
    viewElementNames.insert(pair<string, int>("message", (int)eVeDisplayMessage::message));
}

void cViewMessage::SetViewElementObjects(void) {
    if (!viewElements[(int)eVeDisplayMessage::message])
        return;
    veMessage = dynamic_cast<cVeMessage*>(viewElements[(int)eVeDisplayMessage::message]);
}

void cViewMessage::ClearVariables(void) {
    init = true;
}

void cViewMessage::SetMessage(eMessageType type, const char *text) {
    if (!text)
        veMessage->Clear();
    else
        veMessage->Set(type, text);
}

void cViewMessage::Flush(void) {
    if (init) {
        Render((int)eVeDisplayMessage::background);
    }
    Render((int)eVeDisplayMessage::message);
    cView::Flush();
}

