#ifndef MAXDIOINTERVALOVERLAY_H
#define MAXDIOINTERVALOVERLAY_H

#include "IOverlayModel.h"

class MaxDioIntervalOverlay : public IOverlayModel
{
public:
	MaxDioIntervalOverlay();
	virtual bool nodeCirclePen(rpl::Node *node, QPen *newPen, QBrush *newBrush);
	virtual bool nodeTextPen(rpl::Node *node, QFont *newFont, QColor *newColor);
	virtual bool linkPen(rpl::Link *link, QPen *newPen);

private:
	double max_dio_interval;
};

#endif // MAXDIOINTERVALOVERLAY_H
