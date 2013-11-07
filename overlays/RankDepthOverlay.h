#ifndef RANKDEPTHOVERLAY_H
#define RANKDEPTHOVERLAY_H

#include <IOverlayModel.h>

class RankDepthOverlay : public IOverlayModel
{
public:
	virtual bool nodeCirclePen(rpl::Node *node, QPen *newPen, QBrush *newBrush);
    virtual bool nodeInfoText(rpl::Node *node, QString *  infoText);
};

#endif // RANKDEPTHOVERLAY_H
