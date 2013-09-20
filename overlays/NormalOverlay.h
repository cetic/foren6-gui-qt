#ifndef NORMALOVERLAY_H
#define NORMALOVERLAY_H

#include "ErrorsOverlay.h"

class NormalOverlay : public ErrorsOverlay
{
public:
    virtual int getErrorCount(rpl::Node *node);
};

#endif // NORMALOVERLAY_H
