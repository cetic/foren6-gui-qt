#ifndef PACKETWIDGET_H
#define PACKETWIDGET_H

#include <QDockWidget>
#include <QModelIndex>
#include <QProcess>
#include <QTreeView>
#include <stdint.h>
#include "rpl/rplNetworkInfoManager.h"

#include "SimpleTreeModel.h"

namespace Ui {
class PacketWidget;
}

class PacketWidget : public QDockWidget
{
	Q_OBJECT
	
public:
	explicit PacketWidget(QWidget *parent = 0);
	~PacketWidget();

public slots:
    void showPacket(rpl::Event *newMsg);

signals:

protected:
  void storeExpandState();
  void applyExpandState();
  void applyExpandState_sub(QStringList& expandedItems,
                          QTreeView* treeView,
                          QAbstractItemModel* model,
                          QModelIndex startIndex);

protected slots:
	
private:
	Ui::PacketWidget *ui;
	TreeModel *model;
	QStringList expandedItems;

};

#endif // PACKETWIDGET_H
