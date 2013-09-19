#ifndef RPL_NODE_H
#define RPL_NODE_H

#include <QGraphicsItem>
#include <data_info/node.h>
#include <QElapsedTimer>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QColor>

namespace rpl
{

class Link;
class NetworkInfoManager;

class Node : public QGraphicsItemGroup
{
public:
	Node(NetworkInfoManager *networkInfoManager, di_node_t *nodeData, int version);
	~Node();

	void addLink(Link *link) { _links.append(link); }
	void removeLink(Link *link) { _links.removeOne(link); }

	void setCenterPos(QPointF newpos);
	void setCenterPos(qreal x, qreal y);

	QPointF centerPos() const;
	qreal centerX() const { return centerPos().x(); }
	qreal centerY() const { return centerPos().y(); }

	void setPos(qreal x, qreal y);

	void incSpeed(qreal x, qreal y);
	qreal speedX() { return _dx; }
	qreal speedY() { return _dy; }

	qreal radius() { return _ellipse.rect().width() / 2; }

	di_node_t *getNodeData() { return _nodeData; }
	void setNodeData(di_node_t *data, int version);
	QList<Link*> links() { return _links; }
	int getVersion() { return _version; }

	void setPen(QPen pen) { _ellipse.setPen(pen); }
	void setBrush(QBrush brush) { _ellipse.setBrush(brush); }
	void setFont(QFont font) { _label.setFont(font); _infoLabel.setFont(font); }
	void setTextColor(QColor color) {}//{ _label.setDefaultTextColor(color); _infoLabel.setDefaultTextColor(color); }
	void setInfoText(QString infoText);

	bool isSelected() { return _isSelected; }
	void setSelected(bool selected) { _isSelected = selected; }

	void setName(QString const & name);
	QString getName() const { return _friendlyName; }
	void setLocked(bool locked) { _pinned = locked; }
	bool isLocked() const { return _pinned; }

public slots:
	void updatePosition();

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

private:
	NetworkInfoManager *_networkInfoManager;
	QElapsedTimer _timeElapsedMouseMove;    //Since last pos using mouse
	di_node_t *_nodeData;
	QGraphicsEllipseItem _ellipse;
	QGraphicsSimpleTextItem _label;
    QGraphicsSimpleTextItem _infoLabel;
	qreal _dx, _dy;
	QList<Link*> _links;
	qreal _maxSize;

	QString _friendlyName;
	bool _isBeingMoved;
	bool _pinned;
	bool _isSelected;

	//debug
	int _version;  //the version of the node data
	char guard[5];
};

}

#endif // RPL_NODE_H
