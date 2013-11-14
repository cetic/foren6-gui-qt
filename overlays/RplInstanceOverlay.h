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
 *         RPL Instances Overlay
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#ifndef RPLINSTANCEOVERLAY_H
#define RPLINSTANCEOVERLAY_H

#include "IOverlayModel.h"

class RplInstanceOverlay:public IOverlayModel {
  public:
    virtual bool nodeCirclePen(rpl::Node * node, QPen * newPen, QBrush * newBrush);

  protected:
    QColor rplInstanceToColor(int16_t instance);
};

#endif // RPLINSTANCEOVERLAY_H
