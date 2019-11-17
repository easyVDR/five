#include "jsonhelpers.h"

int JsonGetInteger(json_t *jNode, string name) {
	json_t *jInt = json_object_get(jNode, name.c_str());
	if (json_is_integer(jInt)) {
        return json_integer_value(jInt);
    }
    return 0;
}

float JsonGetFloat(json_t *jNode, string name) {
	json_t *jFloat = json_object_get(jNode, name.c_str());
	if (json_is_real(jFloat)) {
        return json_real_value(jFloat);
    }
    if (json_is_integer(jFloat)) {
        return json_integer_value(jFloat);
    }
    return 0.0;
}

string JsonGetString(json_t *jNode, string name) {
	json_t *jString = json_object_get(jNode, name.c_str());
	if (json_is_string(jString)) {
        return json_string_value(jString);
    }
    return "";
}

string FloatToString(float f) {
    stringstream sTemp;
    sTemp << f;
    return sTemp.str();
}

string IntToString(int i) {
    stringstream sTemp;
    sTemp << i;
    return sTemp.str();
}
