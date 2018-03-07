/*
 * Copyright (C) 2015 ~ 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "dfilechooseredit.h"
#include "dthememanager.h"
#include "private/dfilechooseredit_p.h"

#include <QDebug>
#include <QFileDialog>
#include <QScreen>
#include <QGuiApplication>

DWIDGET_BEGIN_NAMESPACE

DFileChooserEdit::DFileChooserEdit(QWidget *parent)
    : DLineEdit(*new DFileChooserEditPrivate(this), parent)
{
    DThemeManager::registerWidget(this);
    D_D(DFileChooserEdit);

    d->init();
}

DFileChooserEdit::DialogDisplayPosition DFileChooserEdit::dialogDisplayPosition() const
{
    D_DC(DFileChooserEdit);

    return d->dialogDisplayPosition;
}

void DFileChooserEdit::setDialogDisplayPosition(DFileChooserEdit::DialogDisplayPosition dialogDisplayPosition)
{
    D_D(DFileChooserEdit);

    d->dialogDisplayPosition = dialogDisplayPosition;
}


void  DFileChooserEdit::setFileMode(QFileDialog::FileMode mode)
{
    D_D(DFileChooserEdit);
    d->fileMode = mode;
}

QFileDialog::FileMode DFileChooserEdit::fileMode() const
{
    D_DC(DFileChooserEdit);
    return d->fileMode;
}

void  DFileChooserEdit::setNameFilters(const QStringList &filters)
{
    D_D(DFileChooserEdit);
    d->nameFilters = filters;
}

QStringList DFileChooserEdit::nameFilters() const
{
    D_DC(DFileChooserEdit);
    return d->nameFilters;
}


DFileChooserEditPrivate::DFileChooserEditPrivate(DFileChooserEdit *q)
    : DLineEditPrivate(q)
{
}

void DFileChooserEditPrivate::init()
{
    D_Q(DFileChooserEdit);

    q->setTextMargins(0, 0, 24, 0);
    q->setReadOnly(true);
    q->setIconVisible(true);
    q->connect(q, SIGNAL(iconClicked()), q, SLOT(_q_showFileChooserDialog()));
}

void DFileChooserEditPrivate::_q_showFileChooserDialog()
{
    D_Q(DFileChooserEdit);

    QFileDialog dialog(q);

    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(fileMode);
    dialog.setNameFilters(nameFilters);

    if (dialogDisplayPosition == DFileChooserEdit::CurrentMonitorCenter) {
        QPoint pos = QCursor::pos();

        for (QScreen *screen : qApp->screens()) {
            if (screen->geometry().contains(pos)) {
                QRect rect = dialog.geometry();
                rect.moveCenter(screen->geometry().center());
                dialog.move(rect.topLeft());
            }
        }
    }

    q->dialogOpened();

    int code = dialog.exec();


    if (code == QDialog::Accepted && !dialog.selectedFiles().isEmpty()) {
        const QString fileName = dialog.selectedFiles().first();

        q->setText(fileName);
        Q_EMIT q->fileChoosed(fileName);
    }

    q->dialogClosed(code);
}

DWIDGET_END_NAMESPACE

#include "moc_dfilechooseredit.cpp"
