#ifndef RPLINSTANCEOVERLAY_H
#define RPLINSTANCEOVERLAY_H

#include "IOverlayModel.h"

class RplInstanceOverlay : public IOverlayModel
{
public:
	virtual bool nodeCirclePen(rpl::Node *node, QPen *newPen, QBrush *newBrush);

protected:
	QColor rplInstanceToColor(int16_t instance);
};

#endif // RPLINSTANCEOVERLAY_H
