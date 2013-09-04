#ifndef ROUTELOOPERROROVERLAY_H
#define ROUTELOOPERROROVERLAY_H

#include "ErrorsOverlay.h"

class RouteLoopErrorOverlay : public ErrorsOverlay
{
public:
	virtual int getErrorCount(rpl::Node *node);
};

#endif // ROUTELOOPERROROVERLAY_H
