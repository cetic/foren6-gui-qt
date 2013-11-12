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
 *         Opened sources handler
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#include "OpenedSniffersModel.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QUrlQuery>
#endif

OpenedSniffersModel::OpenedSniffersModel(QObject *parent) :
	QAbstractTableModel(parent)
{
}

void OpenedSniffersModel::append(QUrl url, interface_t *interface, ifreader_t sniffer_handle) {
	SnifferInterface newInterface;

	newInterface.url = url;
	newInterface.interface = interface;
	newInterface.sniffer_handle = sniffer_handle;

	beginInsertRows(QModelIndex(), openedSniffers.size(), openedSniffers.size());
	openedSniffers.append(newInterface);
	endInsertRows();
}

interface_t *OpenedSniffersModel::getInterface(int index) {
	return openedSniffers.at(index).interface;
}

ifreader_t OpenedSniffersModel::getHandle(int index) {
	return openedSniffers.at(index).sniffer_handle;
}

void OpenedSniffersModel::remove(int index) {
	beginRemoveRows(QModelIndex(), index, index);
	openedSniffers.removeAt(index);
	endRemoveRows();
}


int OpenedSniffersModel::rowCount(const QModelIndex & parent) const {
	if(parent.isValid())
		return 0;
	else return openedSniffers.size();
}

int OpenedSniffersModel::columnCount(const QModelIndex & parent) const {
	if(parent.isValid())
		return 0;
	else return 4;
}

QVariant OpenedSniffersModel::data(const QModelIndex & index, int role ) const {
	if(index.parent().isValid() || role != Qt::DisplayRole)
		return QVariant();

	switch(index.column()) {
		case 0:
			return openedSniffers.at(index.row()).url.scheme();
		case 1:
			return openedSniffers.at(index.row()).url.path();
		case 2:
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
			return QUrlQuery(openedSniffers.at(index.row()).url).queryItemValue("channel");
#else
			return openedSniffers.at(index.row()).url.queryItemValue("channel");
#endif
		case 3:
			return openedSniffers.at(index.row()).url.toString();
		default:
			return QVariant();
	}
}

QVariant OpenedSniffersModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if(orientation != Qt::Horizontal || role != Qt::DisplayRole)
		return QVariant();

	switch(section) {
		case 0:
			return QString("Type");
		case 1:
			return QString("Path");
		case 2:
			return QString("Channel");
		case 3:
			return QString("URL");
		default:
			return QVariant();
	}
}
