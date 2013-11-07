#include "IOverlayModel.h"

#include "rpl/rplNode.h"
#include "rpl/rplLink.h"
#include <QApplication>

bool IOverlayModel::nodeCirclePen(rpl::Node *node, QPen *newPen, QBrush *newBrush) {
    if(!newPen || !newBrush)
        return false;

    if(!node->getNodeData())
        return false;

    if(node->isSelected())
        *newPen = QPen(QColor(Qt::darkBlue));
    else {
        *newPen = QPen(QColor(Qt::black));
    }

    if(node_get_rank(node->getNodeData()) == 256) {
        newPen->setWidth(3);
    } else {
        newPen->setWidth(2);
    }

    newBrush->setStyle(Qt::SolidPattern);
    newBrush->setColor(Qt::white);

    return true;
}

bool IOverlayModel::nodeTextPen(rpl::Node *node, QFont *newFont, QColor *newColor) {
    Q_UNUSED(node);
    Q_UNUSED(newColor);

    if(newFont) {
        *newFont = QApplication::font();
        newFont->setPointSize(8);
    }

    return true;
}

bool IOverlayModel::linkPen(rpl::Link *link, QPen *newPen) {
    if (link_get_deprecated(link->getLinkData()) ) {
        *newPen = QPen(Qt::lightGray, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    } else {
        *newPen = QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    }
    return true;
}

bool IOverlayModel::nodeInfoText(rpl::Node *node, QString *  infoText) {
    Q_UNUSED(node);
    Q_UNUSED(infoText);
    return false;
}
