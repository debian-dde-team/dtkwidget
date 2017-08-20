/**
 * Copyright (C) 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DMPRISCONTROL_P_H
#define DMPRISCONTROL_P_H

#include <DObjectPrivate>

#include "dmpriscontrol.h"
#include "dimagebutton.h"
#include "private/mpris/dmprismonitor.h"
#include "private/mpris/dbusmpris.h"

DWIDGET_BEGIN_NAMESPACE

class DMPRISControlPrivate : public DTK_CORE_NAMESPACE::DObjectPrivate
{
    D_DECLARE_PUBLIC(DMPRISControl)

public:
    explicit DMPRISControlPrivate(DMPRISControl *q);

    void init();

public Q_SLOTS:
    void _q_onPrevClicked();
    void _q_onPlayClicked();
    void _q_onPauseClicked();
    void _q_onNextClicked();

    void _q_onMetaDataChanged();
    void _q_onPlaybackStatusChanged();

    void _q_loadMPRISPath(const QString &path);
    void _q_removeMPRISPath(const QString &path);

public:
    DMPRISMonitor *m_mprisMonitor;
    DBusMPRIS *m_mprisInter;

    QLabel *m_title;
    QLabel *m_picture;
    DImageButton *m_prevBtn;
    DImageButton *m_playBtn;
    DImageButton *m_pauseBtn;
    DImageButton *m_nextBtn;

    QString m_lastPath;
    QStringList m_mprisPaths;
};

DWIDGET_END_NAMESPACE

#endif // DMPRISCONTROL_P_H
