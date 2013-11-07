#ifndef MAXDAOINTERVALOVERLAY_H
#define MAXDAOINTERVALOVERLAY_H

#include "IOverlayModel.h"

class MaxDaoIntervalOverlay : public IOverlayModel
{
public:
	MaxDaoIntervalOverlay();
	virtual int neededPasses() { return 2; }
	virtual void initPasses() { max_dao_interval = 0; }
	virtual bool nodeCirclePen(rpl::Node *node, QPen *newPen, QBrush *newBrush);
    virtual bool nodeInfoText(rpl::Node *node, QString *  infoText);

private:
	double max_dao_interval;
};

#endif // MAXDAOINTERVALOVERLAY_H
