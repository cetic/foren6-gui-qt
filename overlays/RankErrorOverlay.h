#ifndef RANKERROROVERLAY_H
#define RANKERROROVERLAY_H

#include "IOverlayModel.h"

class RankErrorOverlay : public IOverlayModel
{
public:
	RankErrorOverlay();
	virtual int neededPasses() { return 2; }
	virtual void initPasses() { maxErrorCount = 0; }
	virtual bool nodeCirclePen(rpl::Node *node, QPen *newPen, QBrush *newBrush);
	virtual bool nodeTextPen(rpl::Node *node, QFont *newFont, QColor *newColor);
	virtual bool linkPen(rpl::Link *link, QPen *newPen);

private:
	int maxErrorCount;
};

#endif // RANKERROROVERLAY_H
