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
 *         Foren6 Main
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#include <QApplication>
#include "MainWindow.h"

interface_t *telos_interface;

int
main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationName("Foren6");
    a.setApplicationVersion("1.0.0");
    a.setOrganizationDomain("cetic.be");
    a.setOrganizationName("CETIC");


    MainWindow m;

    m.show();

    return a.exec();
}
