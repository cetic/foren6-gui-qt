#ifndef NORMALOVERLAY_H
#define NORMALOVERLAY_H

#include "../IOverlayModel.h"

class NormalOverlay : public IOverlayModel
{
public:
	virtual bool nodeCirclePen(rpl::Node *node, QPen *newPen, QBrush *newBrush);
	virtual bool nodeTextPen(rpl::Node *node, QFont *newFont, QColor *newColor);
	virtual bool linkPen(rpl::Link *link, QPen *newPen);
    virtual bool nodeInfoText(rpl::Node *node, QString *  infoText);
};

#endif // NORMALOVERLAY_H
