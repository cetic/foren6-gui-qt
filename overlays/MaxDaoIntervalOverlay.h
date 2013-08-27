#ifndef MAXDAOINTERVALOVERLAY_H
#define MAXDAOINTERVALOVERLAY_H

#include "IOverlayModel.h"

class MaxDaoIntervalOverlay : public IOverlayModel
{
public:
	MaxDaoIntervalOverlay();
	virtual bool nodeCirclePen(rpl::Node *node, QPen *newPen, QBrush *newBrush);
	virtual bool nodeTextPen(rpl::Node *node, QFont *newFont, QColor *newColor);
	virtual bool linkPen(rpl::Link *link, QPen *newPen);

private:
	double max_dao_interval;
};

#endif // MAXDAOINTERVALOVERLAY_H
