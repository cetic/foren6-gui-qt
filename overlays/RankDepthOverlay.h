#ifndef RANKDEPTHOVERLAY_H
#define RANKDEPTHOVERLAY_H

#include <IOverlayModel.h>

class RankDepthOverlay : public IOverlayModel
{
public:
	virtual bool nodeCirclePen(rpl::Node *node, QPen *newPen, QBrush *newBrush);
	virtual bool nodeTextPen(rpl::Node *node, QFont *newFont, QColor *newColor);
	virtual bool linkPen(rpl::Link *link, QPen *newPen);
};

#endif // RANKDEPTHOVERLAY_H
