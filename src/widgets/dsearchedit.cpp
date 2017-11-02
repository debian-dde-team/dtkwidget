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

#include "dsearchedit.h"
#include "dthememanager.h"

#include <QHBoxLayout>
#include <QSpacerItem>
#include <QPropertyAnimation>
#include <QDebug>
#include <QEvent>
#include <QFocusEvent>
#include <QResizeEvent>
#include <QTimer>

DWIDGET_USE_NAMESPACE

DSearchEdit::DSearchEdit(QWidget *parent)
    : QFrame(parent)
{
    D_THEME_INIT_WIDGET(DSearchEdit);

    initInsideFrame();

    m_searchBtn = new QLabel;
    m_searchBtn->setObjectName("SearchIcon");
    m_searchBtn->setFixedSize(16, 16);
    m_clearBtn = new DImageButton;
    m_clearBtn->setObjectName("ClearIcon");
    m_clearBtn->hide();
    m_edt = new QLineEdit;
    m_edt->setObjectName("Edit");
    m_placeHolder = new QLabel;
    m_placeHolder->setObjectName("PlaceHolder");

    m_animation = new QPropertyAnimation(m_edt, "minimumWidth");

    m_size = QSize(m_searchBtn->sizeHint().width() + m_edt->sizeHint().width() + m_clearBtn->sizeHint().width() + 6,
                   qMax(m_searchBtn->sizeHint().height(), m_edt->sizeHint().height()));
    m_edt->setFixedWidth(0);
    m_edt->installEventFilter(this);

    QHBoxLayout *layout = new QHBoxLayout(m_insideFrame);
    layout->addStretch();
    layout->addWidget(m_searchBtn);
    layout->setAlignment(m_searchBtn, Qt::AlignCenter);
    layout->addWidget(m_placeHolder);
    layout->setAlignment(m_placeHolder, Qt::AlignCenter);
    layout->addWidget(m_edt);
    layout->setAlignment(m_edt, Qt::AlignCenter);
    layout->addStretch();
    layout->addWidget(m_clearBtn);
    layout->setAlignment(m_clearBtn, Qt::AlignCenter);
    layout->setSpacing(0);
    layout->setContentsMargins(3, 0, 3, 0);

    setAutoFillBackground(true);
    setFocusPolicy(Qt::StrongFocus);

    connect(m_clearBtn, &DImageButton::clicked, m_edt, static_cast<void (QLineEdit::*)()>(&QLineEdit::setFocus));
    connect(m_clearBtn, &DImageButton::clicked, this, &DSearchEdit::clear);
    connect(m_edt, &QLineEdit::textChanged, [this] {m_clearBtn->setVisible(!m_edt->text().isEmpty());});
    connect(m_edt, &QLineEdit::textChanged, this, &DSearchEdit::textChanged, Qt::DirectConnection);
    connect(m_edt, &QLineEdit::editingFinished, this, &DSearchEdit::editingFinished, Qt::DirectConnection);
    connect(m_edt, &QLineEdit::returnPressed, this, &DSearchEdit::returnPressed, Qt::DirectConnection);
//    connect(m_searchBtn, &DImageButton::clicked, this, &DSearchEdit::toEditMode);
}

DSearchEdit::~DSearchEdit()
{
    m_animation->deleteLater();
}

const QString DSearchEdit::text() const
{
    return m_edt->text();
}

void DSearchEdit::mousePressEvent(QMouseEvent *e)
{
    if (e->button() != Qt::LeftButton)
        return QFrame::mousePressEvent(e);

    toEditMode();

    e->accept();
}

void DSearchEdit::mouseReleaseEvent(QMouseEvent *e)
{
    e->accept();
}

bool DSearchEdit::eventFilter(QObject *o, QEvent *e)
{
    if (o == m_edt && e->type() == QEvent::FocusOut && m_edt->text().isEmpty()) {
        auto fe = dynamic_cast<QFocusEvent *>(e);
        if (fe && fe->reason() != Qt::PopupFocusReason) {
            m_animation->stop();
            m_animation->setStartValue(m_edt->width());
            m_animation->setEndValue(0);
            m_animation->setEasingCurve(m_hideCurve);
            m_animation->start();

            connect(m_animation, &QPropertyAnimation::finished, m_placeHolder, &QLabel::show);
        }
    }

    if (o == m_edt) {
        if (e->type() == QEvent::FocusOut) {
            Q_EMIT focusOut();
        }

        if (e->type() == QEvent::FocusIn) {
            Q_EMIT focusIn();
        }
    }

    return QFrame::eventFilter(o, e);
}

QLineEdit *DSearchEdit::getLineEdit() const
{
    return m_edt;
}

void DSearchEdit::toEditMode()
{
    // already in edit mode
    if (!m_placeHolder->isVisible()) {
        return;
    }

    disconnect(m_animation, &QPropertyAnimation::finished, m_placeHolder, &QLabel::show);

    m_animation->stop();
    m_animation->setStartValue(0);
    m_animation->setEndValue(m_size.width() - m_searchBtn->width() - 6); // left + right margins = 6
    m_animation->setEasingCurve(m_showCurve);
    m_animation->start();

    m_placeHolder->hide();
    m_edt->setFocus();
}

//Bypassing the problem here
//qss can't draw box-shadow
void DSearchEdit::initInsideFrame()
{
    m_insideFrame = new QFrame(this);
    m_insideFrame->raise();
    m_insideFrame->setObjectName("DEditInsideFrame");
    QHBoxLayout *insideLayout = new QHBoxLayout(this);
    insideLayout->setContentsMargins(0, 0, 0, 1);
    insideLayout->setSpacing(0);
    insideLayout->addWidget(m_insideFrame);
}

void DSearchEdit::resizeEvent(QResizeEvent *e)
{
    m_size = e->size();
    m_edt->setFixedHeight(m_size.height());
}

bool DSearchEdit::event(QEvent *e)
{
    if (e->type() == QEvent::FocusIn) {
        const QFocusEvent *event = static_cast<QFocusEvent*>(e);

        if (event->reason() == Qt::TabFocusReason
                || event->reason() == Qt::BacktabFocusReason
                || event->reason() == Qt::OtherFocusReason
                || event->reason() == Qt::ShortcutFocusReason) {
            toEditMode();
        }
    }

    return QFrame::event(e);
}
