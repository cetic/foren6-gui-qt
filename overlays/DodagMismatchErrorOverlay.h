#ifndef DODAGMISMATCHERROROVERLAY_H
#define DODAGMISMATCHERROROVERLAY_H

#include "ErrorsOverlay.h"

class DodagMismatchErrorOverlay : public ErrorsOverlay
{
public:
	virtual int getErrorCount(rpl::Node *node);
};

#endif // DODAGMISMATCHERROROVERLAY_H
