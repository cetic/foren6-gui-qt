#ifndef TRAFFICOVERLAY_H
#define TRAFFICOVERLAY_H

#include "IOverlayModel.h"

class TrafficOverlay : public IOverlayModel
{
public:
	TrafficOverlay();
	virtual int neededPasses() { return 2; }
	virtual void initPasses() { max_packets_encountered = 0; }
	virtual bool nodeCirclePen(rpl::Node *node, QPen *newPen, QBrush *newBrush);
    virtual bool nodeInfoText(rpl::Node *node, QString *  infoText);

private:
	int max_packets_encountered;
};

#endif // TRAFFICOVERLAY_H
