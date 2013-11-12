/*
 * Copyright (c) 2013, CETIC.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * \file
 *         RPL Instances Overlay
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#include "RplInstanceOverlay.h"
#include "rpl/rplNode.h"
#include "rpl/rplLink.h"
#include "rpl/rplNetworkInfoManager.h"
#include <QApplication>
#include <QCryptographicHash>

QColor RplInstanceOverlay::rplInstanceToColor(int16_t instance) {
	unsigned char color[3] = {0};

	QByteArray md5Hash = QCryptographicHash::hash(QByteArray((const char*)&instance, sizeof(int16_t)), QCryptographicHash::Md5);

	for(int i = 0; i < md5Hash.size(); i++) {
		color[i%3] += md5Hash.at(i);
	}

	return QColor(color[0]/2+64, color[1]/2+64, color[2]/2+64);
}

bool RplInstanceOverlay::nodeCirclePen(rpl::Node *node, QPen *newPen, QBrush *newBrush) {
	if(!newPen || !newBrush)
		return false;

	const di_dodag_ref_t* dodagRef = node_get_dodag(node->getNodeData());
	QColor dodagColor;

	if(dodagRef) {
		const di_dodag_t* dodag = rpl::NetworkInfoManager::getInstance()->getDodag(dodagRef);
		dodagColor = rplInstanceToColor(dodag_get_rpl_instance(dodag)->rpl_instance);
	} else dodagColor = QColor(Qt::black);

	if(node->isSelected())
		*newPen = QPen(QColor(Qt::darkBlue));
	else
		*newPen = QPen(dodagColor);

	if(node_get_rank(node->getNodeData()) == 256) {
		newPen->setWidth(3);
	} else {
		newPen->setWidth(2);
	}

	newBrush->setStyle(Qt::SolidPattern);
	newBrush->setColor(Qt::white);

	return true;
}
