#ifndef IOVERLAYMODEL_H
#define IOVERLAYMODEL_H

#include <QPen>
#include <QBrush>
#include <QFont>
#include <QColor>
#include <QGraphicsTextItem>

namespace rpl {
class Node;
class Link;
}

class IOverlayModel
{
	//Methods return true if pen, font and/or color contain relevant data or false to keep the current object's style
public:
	virtual ~IOverlayModel() {}

	virtual int neededPasses() { return 1; }

	virtual void initPasses() {};

	virtual bool nodeCirclePen(rpl::Node *node, QPen *newPen, QBrush *newBrush) = 0;
	virtual bool nodeTextPen(rpl::Node *node, QFont *newFont, QColor *newColor) = 0;
	virtual bool linkPen(rpl::Link *link, QPen *newPen) = 0;
    virtual bool nodeInfoText(rpl::Node *node, QString *  infoText) = 0;
};

#endif // IOVERLAYMODEL_H
