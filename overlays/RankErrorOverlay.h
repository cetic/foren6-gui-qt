#ifndef RANKERROROVERLAY_H
#define RANKERROROVERLAY_H

#include "ErrorsOverlay.h"

class RankErrorOverlay : public ErrorsOverlay
{
public:
	virtual int getErrorCount(rpl::Node *node);
};

#endif // RANKERROROVERLAY_H
