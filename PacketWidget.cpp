#include "PacketWidget.h"
#include "ui_PacketWidget.h"
#include <QProcess>
#include <stdint.h>
#include "SimpleTreeModel.h"

PacketWidget::PacketWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::PacketWidget),
    model(0)
{
	ui->setupUi(this);
	ui->dataTree->setFont(QFont("Courier"));
}

PacketWidget::~PacketWidget()
{
	delete ui;
}

void PacketWidget::applyExpandState_sub(QStringList& expandedItems,
                          QTreeView* treeView,
                          QAbstractItemModel* model,
                          QModelIndex startIndex)
{
    foreach (QString item, expandedItems)
    {
        QModelIndexList matches = model->match( startIndex, Qt::DisplayRole, item );
        foreach (QModelIndex index, matches)
        {
            treeView->setExpanded( index, true );
            applyExpandState_sub(expandedItems,
                                 treeView,
                                 model,
                                 model->index( 0, 0, index ) );
        }
    }
}

void PacketWidget::applyExpandState() {
    ui->dataTree->setUpdatesEnabled(false);

    applyExpandState_sub( expandedItems,
                          ui->dataTree,
                          model,
                          model->index( 0, 0, QModelIndex() ) );

    ui->dataTree->setUpdatesEnabled(true);
}

void PacketWidget::storeExpandState() {
    expandedItems.clear();
    foreach (QModelIndex index, model->getPersistentIndexList())
    {
        if (ui->dataTree->isExpanded(index))
        {
             QString item = index.data(Qt::DisplayRole).toString();
             QString id;
             int colon = item.indexOf(':');
             if ( colon  != -1 ) {
                 id = item.left(colon);
             } else {
               int comma = item.indexOf(',');
               if ( comma != -1 ) {
                 id = item.left(comma);
               } else {
                 id = item;
               }
             }
             expandedItems << id;
        }
    }
}

void PacketWidget::showPacket(rpl::Event *event) {
    if (model) {
        storeExpandState();
    }
    TreeModel * oldModel = model;
    ui->dataTree->setModel(0);
    delete oldModel;
    model = 0;

    if ( event->object != rpl::Event::EO_Packet ) return;

    QProcess tshark;
    tshark.start("tshark", QStringList() << "-V" << "-c" << "1" << "-r" << "out.pcap" << "-R" << (QString("frame.number==") + QString::number(event->packed_id+1)));
    if (!tshark.waitForStarted())
        return;
    if (!tshark.waitForFinished())
        return;

    QByteArray result = tshark.readAll();
    model = new TreeModel(result);
    ui->dataTree->setModel(model);
    applyExpandState();
}
