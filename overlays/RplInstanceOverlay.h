#ifndef RPLINSTANCEOVERLAY_H
#define RPLINSTANCEOVERLAY_H

#include "IOverlayModel.h"

class RplInstanceOverlay : public IOverlayModel
{
public:
	virtual bool nodeCirclePen(rpl::Node *node, QPen *newPen, QBrush *newBrush);
	virtual bool nodeTextPen(rpl::Node *node, QFont *newFont, QColor *newColor);
	virtual bool linkPen(rpl::Link *link, QPen *newPen);

protected:
	QColor rplInstanceToColor(int16_t instance);
};

#endif // RPLINSTANCEOVERLAY_H
