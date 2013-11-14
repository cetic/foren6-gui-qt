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
 *         History navigation with horizontal slider
 * \author
 *         Foren6 Team <foren6@cetic.be>
 *         http://cetic.github.io/foren6/credits.html
 */

#ifndef VERSIONSLIDER_H
#define VERSIONSLIDER_H

#include <QWidget>

namespace Ui {
    class VersionSlider;
} class VersionSlider:public QWidget {
  Q_OBJECT public:
    explicit VersionSlider(QWidget * parent = 0);
    ~VersionSlider();

    void setMaximum(int max);
    void setValue(int version);
    void setRealtime();
    int maximum();
    int value();
    bool realtime();
    void clearTime();
    void startTime();
    void stopTime();

    public slots:void onChangeCurrentVersion(int newVersion);
    void onUpdateVersionCount(int versionCount);
    void onTimeTick(int msec);

      signals:void changeWsnVersion(int version);

  protected:
    void updateTime(void);

    protected slots:void onVersionSliderChange(int value);
    void onVersionSpinChange(int value);

  private:
      Ui::VersionSlider * ui;
    bool showUpdate;
    bool incrTime;
    double currentTimestamp;
    double currentMaxTimestamp;
    double maxTimestamp;
    bool settingCurrentVersion; //avoid recursive loop setValue -> onVersion*Change -> setValue ...
};

#endif // VERSIONSLIDER_H
