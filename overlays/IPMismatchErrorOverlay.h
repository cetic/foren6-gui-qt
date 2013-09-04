#ifndef IPMISMATCHERROROVERLAY_H
#define IPMISMATCHERROROVERLAY_H

#include "ErrorsOverlay.h"

class IPMismatchErrorOverlay : public ErrorsOverlay
{
public:
	virtual int getErrorCount(rpl::Node *node);
};

#endif // IPMISMATCHERROROVERLAY_H
