#ifndef DODAGOVERLAY_H
#define DODAGOVERLAY_H

#include "IOverlayModel.h"

class DodagOverlay : public IOverlayModel
{
public:
	virtual bool nodeCirclePen(rpl::Node *node, QPen *newPen, QBrush *newBrush);
	virtual bool nodeTextPen(rpl::Node *node, QFont *newFont, QColor *newColor);
	virtual bool linkPen(rpl::Link *link, QPen *newPen);

protected:
	QColor dodagToColor(const void* dodagid, int16_t version);

};

#endif // DODAGOVERLAY_H
