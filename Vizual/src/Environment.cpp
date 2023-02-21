#include "Environment.h"

Environment::Environment() {}

Environment::~Environment() {
    for(auto field : fields) {
        delete field;
    }
}
