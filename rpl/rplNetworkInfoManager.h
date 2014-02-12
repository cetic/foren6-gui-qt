/*
 * This file is part of Foren6, a 6LoWPAN Diagnosis Tool
 * Copyright (C) 2013, CETIC
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * \file
 *         RPL Information Handling in the GUI
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#ifndef RPLNETWORKINFOMANAGER_H
#define RPLNETWORKINFOMANAGER_H

#include <QHash>
#include <rpl_packet_parser.h>
#include "rplTreeScene.h"
#include <QMutex>
#include <QObject>
#include <QSettings>

namespace rpl {

    struct Event {
        rpl_event_type_e type;
        enum {
            EO_Node,
            EO_Dodag,
            EO_RplInstance,
            EO_Link,
            EO_Packet
        } object;
        union {
            di_node_t *as_node;
            di_dodag_t *as_dodag;
            di_rpl_instance_t *as_instance;
            di_link_t *as_link;
            packet_info_t packet_info;
        };
        int version;
        int packed_id;
    };

    class NetworkInfoManager:public QObject {
      Q_OBJECT public:

      public:
        NetworkInfoManager();
        ~NetworkInfoManager();

        TreeScene *scene() {
            return &_scene;
        } void selectNode(Node * node);
        IOverlayModel *overlay() {
            return _overlay;
        } void changeOverlay(IOverlayModel * newOverlay);

        int getVersion();
        di_node_t *getNode(const di_node_ref_t * node_ref);
        di_dodag_t *getDodag(const di_dodag_ref_t * dodag_ref);
        di_link_t *getLink(const di_link_ref_t * link_ref);
        di_rpl_instance_t *getRplInstance(const di_rpl_instance_ref_t * rpl_instance_ref);
        void setLayout(QString layoutFile);
        void setLinkDialogs(bool state);
        bool getDialogsLinked();

        static NetworkInfoManager *getInstance() {
            return thisInstance;
        }

      public slots:
        void useVersion(int version);
        void updateOverlay();
        void onLoadBackground();
        void onClearBackground();
        void onLoadLayout();
        void onSaveLayout();
        void onClearLayout();

      signals:
        void nodeUpdateSelected(const di_node_t * node, const di_dodag_t * dodag, const di_rpl_instance_t * rpl_instance);
        void updateVersionCount(int versionCount);
        void timeTick(int msec);
        void logMessage(rpl::Event * action);
        void clearMessages();
        void layoutChanged(QString layout);

        protected slots:void updateVersion();
        void updateSelectedNodeInfo();

      protected:
        static void onNodeEvent(di_node_t * node, rpl_event_type_e type);
        static void onDodagEvent(di_dodag_t * dodag, rpl_event_type_e type);
        static void onRplInstanceEvent(di_rpl_instance_t * rpl_instance, rpl_event_type_e type);
        static void onLinkEvent(di_link_t * link, rpl_event_type_e type);
        static void onPacketEvent(int packet_id, packet_info_t packet_info);
        static void onClear();

      private:
        TreeScene _scene;
        IOverlayModel *_overlay;

        QTimer _updateVersionTimer;
        int currentVersion;
        bool realtimeMode;
        Node *selectedNode;
        static NetworkInfoManager *thisInstance;
        QSettings *layout;
        QString layoutFile;
        bool dialogsLinked;
    };

}

Q_DECLARE_METATYPE(rpl::Event *)
#endif // RPLNETWORKINFOMANAGER_H
