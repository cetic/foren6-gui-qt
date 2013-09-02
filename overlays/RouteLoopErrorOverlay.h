#ifndef ROUTELOOPERROROVERLAY_H
#define ROUTELOOPERROROVERLAY_H

#include "IOverlayModel.h"

class RouteLoopErrorOverlay : public IOverlayModel
{
public:
	RouteLoopErrorOverlay();
	virtual bool nodeCirclePen(rpl::Node *node, QPen *newPen, QBrush *newBrush);
	virtual bool nodeTextPen(rpl::Node *node, QFont *newFont, QColor *newColor);
	virtual bool linkPen(rpl::Link *link, QPen *newPen);

private:
	int maxErrorCount;
};

#endif // ROUTELOOPERROROVERLAY_H
