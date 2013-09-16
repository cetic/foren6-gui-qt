#ifndef ERRORSOVERLAY_H
#define ERRORSOVERLAY_H

#include "IOverlayModel.h"

class ErrorsOverlay : public IOverlayModel
{
public:
	ErrorsOverlay();
	virtual int neededPasses() { return 2; }
	virtual void initPasses() { maxErrorCount = 0; }
	virtual bool nodeCirclePen(rpl::Node *node, QPen *newPen, QBrush *newBrush);
	virtual bool nodeTextPen(rpl::Node *node, QFont *newFont, QColor *newColor);
	virtual bool linkPen(rpl::Link *link, QPen *newPen);
    virtual bool nodeInfoText(rpl::Node *node, QString *  infoText);

	virtual int getErrorCount(rpl::Node *node) = 0;

private:
	int maxErrorCount;
};

#endif // ERRORSOVERLAY_H
