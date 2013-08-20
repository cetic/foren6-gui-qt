#ifndef PARENTCHILDRENOVERLAY_H
#define PARENTCHILDRENOVERLAY_H

#include "IOverlayModel.h"

class ParentChildrenOverlay : public IOverlayModel
{
public:
	ParentChildrenOverlay();

	virtual bool nodeCirclePen(rpl::Node *node, QPen *newPen, QBrush *newBrush);
	virtual bool nodeTextPen(rpl::Node *node, QFont *newFont, QColor *newColor);
	virtual bool linkPen(rpl::Link *link, QPen *newPen);
};

#endif // PARENTCHILDRENOVERLAY_H
