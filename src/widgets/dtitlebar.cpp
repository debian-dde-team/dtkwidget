/*
 * Copyright (C) 2017 ~ 2017 Deepin Technology Co., Ltd.
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

#include "dtitlebar.h"

#include <QDebug>
#include <QMenu>
#include <QHBoxLayout>
#include <QApplication>
#include <QMouseEvent>
#include <QProcess>

#include <DObjectPrivate>

#include "dwindowclosebutton.h"
#include "dwindowmaxbutton.h"
#include "dwindowminbutton.h"
#include "dwindowrestorebutton.h"
#include "dwindowoptionbutton.h"
#include "dlabel.h"
#include "dplatformwindowhandle.h"
#ifdef Q_OS_LINUX
#include "../platforms/x11/xutil.h"
#endif
#include "daboutdialog.h"
#include "dapplication.h"
#include "dthememanager.h"
#include "util/dwindowmanagerhelper.h"

DWIDGET_BEGIN_NAMESPACE

const int DefaultTitlebarHeight = 40;
const int DefaultIconHeight = 24;
const int DefaultIconWidth = 24;

class DTitlebarPrivate : public DTK_CORE_NAMESPACE::DObjectPrivate
{
protected:
    DTitlebarPrivate(DTitlebar *qq);

private:
    void init();
    void _q_toggleWindowState();
    void _q_showMinimized();
    void _q_onTopWindowMotifHintsChanged(quint32 winId);

#ifndef QT_NO_MENU
    void _q_addDefaultMenuItems();
    void _q_helpActionTriggered();
    void _q_aboutActionTriggered();
    void _q_quitActionTriggered();

    bool isUserManualExists() const;
#endif

    QHBoxLayout         *mainLayout;
    DLabel              *iconLabel;
    DLabel              *titleLabel;
    DWindowMinButton    *minButton;
    DWindowMaxButton    *maxButton;
    DWindowCloseButton  *closeButton;
    DWindowOptionButton *optionButton;

    QWidget             *customWidget = Q_NULLPTR;
    QWidget             *coustomAtea;
    QWidget             *buttonArea;
    QWidget             *titleArea;
    QWidget             *titlePadding;
    QLabel              *separator;

#ifndef QT_NO_MENU
    QMenu               *menu = Q_NULLPTR;
    QAction             *helpAction = Q_NULLPTR;
    QAction             *aboutAction = Q_NULLPTR;
    QAction             *quitAction = Q_NULLPTR;
#endif

    QWidget             *parentWindow = Q_NULLPTR;

    bool                mousePressed = false;
    bool                useWindowFlags = false;

    Q_DECLARE_PUBLIC(DTitlebar)
};

DTitlebarPrivate::DTitlebarPrivate(DTitlebar *qq): DObjectPrivate(qq)
{
}

void DTitlebarPrivate::init()
{
    D_Q(DTitlebar);

    mainLayout      = new QHBoxLayout;
    iconLabel       = new DLabel;
    titleLabel      = new DLabel;
    minButton       = new DWindowMinButton;
    maxButton       = new DWindowMaxButton;
    closeButton     = new DWindowCloseButton;
    optionButton    = new DWindowOptionButton;
    coustomAtea     = new QWidget;
    buttonArea      = new QWidget;
    titleArea       = new QWidget;
    titlePadding    = new QWidget;
    separator       = new QLabel(q);

    optionButton->setObjectName("DTitlebarDWindowOptionButton");
    minButton->setObjectName("DTitlebarDWindowMinButton");
    maxButton->setObjectName("DTitlebarDWindowMaxButton");
    closeButton->setObjectName("DTitlebarDWindowCloseButton");

    mainLayout->setContentsMargins(6, 0, 0, 0);
    mainLayout->setSpacing(0);

    iconLabel->setFixedSize(DefaultIconWidth, DefaultIconHeight);
    titleLabel->setText(qApp->applicationName());
    // TODO: use QSS
    titleLabel->setStyleSheet("font-size: 12px;");
    titleLabel->setContentsMargins(0, 0, DefaultIconWidth + 10, 0);
//    q->setStyleSheet("background-color: green;");

    separator->setFixedHeight(1);
    separator->setStyleSheet("background: rgba(0, 0, 0, 20);");
    separator->hide();

    QHBoxLayout *buttonAreaLayout = new QHBoxLayout;
    buttonAreaLayout->setContentsMargins(0, 1, 0, 0);
    buttonAreaLayout->setMargin(0);
    buttonAreaLayout->setSpacing(0);
    buttonAreaLayout->addWidget(optionButton);
    buttonAreaLayout->addWidget(minButton);
    buttonAreaLayout->addWidget(maxButton);
    buttonAreaLayout->addWidget(closeButton);
    buttonArea->setLayout(buttonAreaLayout);

    QHBoxLayout *titleAreaLayout = new QHBoxLayout;
    titleAreaLayout->setContentsMargins(10, 0, 10 + iconLabel->width(), 0);
    titleAreaLayout->setSpacing(0);
    titleAreaLayout->addWidget(iconLabel);
    titleAreaLayout->setAlignment(iconLabel, Qt::AlignLeft);
    titlePadding->setFixedSize(buttonArea->size());
    titleAreaLayout->addWidget(titlePadding);
    titleAreaLayout->addStretch();
    titleAreaLayout->addWidget(titleLabel);
    titleAreaLayout->setAlignment(titleLabel, Qt::AlignCenter);

    titleAreaLayout->addStretch();
    titleArea->setLayout(titleAreaLayout);

    QHBoxLayout *coustomAteaLayout = new QHBoxLayout;
    coustomAteaLayout->setMargin(0);
    coustomAteaLayout->setSpacing(0);
    coustomAteaLayout->addWidget(titleArea);
    coustomAtea->setLayout(coustomAteaLayout);

    mainLayout->addWidget(coustomAtea);
    mainLayout->addWidget(buttonArea);
    mainLayout->setAlignment(buttonArea, Qt::AlignRight |  Qt::AlignVCenter);

    q->setLayout(mainLayout);
    q->setFixedHeight(DefaultTitlebarHeight);
    q->setMinimumHeight(DefaultTitlebarHeight);
    coustomAtea->setFixedHeight(q->height());
    buttonArea->setFixedHeight(q->height());

    q->connect(optionButton, &DWindowOptionButton::clicked, q, &DTitlebar::optionClicked);
    q->connect(DWindowManagerHelper::instance(), SIGNAL(windowMotifWMHintsChanged(quint32)),
               q, SLOT(_q_onTopWindowMotifHintsChanged(quint32)));
}

void DTitlebarPrivate::_q_toggleWindowState()
{
    D_QC(DTitlebar);

    QWidget *parentWindow = q->parentWidget();

    if (!parentWindow) {
        return;
    }

    parentWindow = parentWindow->window();

    if (parentWindow->isMaximized()) {
        parentWindow->showNormal();
    } else if (!parentWindow->isFullScreen()
               && (maxButton->isVisible())) {
        parentWindow->showMaximized();
    }
}

void DTitlebarPrivate::_q_showMinimized()
{
    if (DPlatformWindowHandle::isEnabledDXcb(parentWindow)) {
        parentWindow->showMinimized();
    } else {
#ifdef Q_OS_LINUX
        XUtils::ShowMinimizedWindow(parentWindow, true);
#else
        parentWindow->showMinimized();
#endif
    }
}

void DTitlebarPrivate::_q_onTopWindowMotifHintsChanged(quint32 winId)
{
    D_QC(DTitlebar);

    if (useWindowFlags)
        return;

    if (!DPlatformWindowHandle::isEnabledDXcb(q->window())) {
        q->disconnect(DWindowManagerHelper::instance(), SIGNAL(windowMotifWMHintsChanged(quint32)),
                      q, SLOT(_q_onTopWindowMotifHintsChanged(quint32)));
        return;
    }

    if (winId != q->window()->internalWinId())
        return;

    DWindowManagerHelper::MotifFunctions functions_hints = DWindowManagerHelper::getMotifFunctions(q->window()->windowHandle());
    DWindowManagerHelper::MotifDecorations decorations_hints = DWindowManagerHelper::getMotifDecorations(q->window()->windowHandle());

    if (titleLabel) {
        titleLabel->setVisible(decorations_hints.testFlag(DWindowManagerHelper::DECOR_TITLE));
    }

    if (iconLabel) {
        iconLabel->setVisible(decorations_hints.testFlag(DWindowManagerHelper::DECOR_TITLE));
    }

    minButton->setVisible(decorations_hints.testFlag(DWindowManagerHelper::DECOR_MINIMIZE));
    minButton->setEnabled(functions_hints.testFlag(DWindowManagerHelper::FUNC_MINIMIZE));
    maxButton->setVisible(decorations_hints.testFlag(DWindowManagerHelper::DECOR_MAXIMIZE));
    maxButton->setEnabled(functions_hints.testFlag(DWindowManagerHelper::FUNC_MAXIMIZE));
    closeButton->setEnabled(functions_hints.testFlag(DWindowManagerHelper::FUNC_CLOSE));
    optionButton->setVisible(decorations_hints.testFlag(DWindowManagerHelper::DECOR_MENU));
    buttonArea->adjustSize();
    buttonArea->resize(buttonArea->size());

    if (titlePadding) {
        titlePadding->setFixedSize(buttonArea->size());
    }
}

#ifndef QT_NO_MENU

void DTitlebarPrivate::_q_addDefaultMenuItems()
{
    D_Q(DTitlebar);

    if (!menu) {
        q->setMenu(new QMenu(q));
    }

    // add help menu item.
    if (!helpAction && isUserManualExists()) {
        helpAction = new QAction(qApp->translate("TitleBarMenu", "Help"), menu);
        QObject::connect(helpAction, SIGNAL(triggered(bool)), q, SLOT(_q_helpActionTriggered()));
        menu->addAction(helpAction);
    }

    // add about menu item.
    if (!aboutAction) {
        aboutAction = new QAction(qApp->translate("TitleBarMenu", "About"), menu);
        QObject::connect(aboutAction, SIGNAL(triggered(bool)), q, SLOT(_q_aboutActionTriggered()));
        menu->addAction(aboutAction);
    }

    // add quit menu item.
    if (!quitAction) {
        quitAction = new QAction(qApp->translate("TitleBarMenu", "Exit"), menu);
        QObject::connect(quitAction, SIGNAL(triggered(bool)), q, SLOT(_q_quitActionTriggered()));
        menu->addAction(quitAction);
    }
}

void DTitlebarPrivate::_q_helpActionTriggered()
{
    DApplication *dapp = qobject_cast<DApplication *>(qApp);
    if (dapp) {
        dapp->handleHelpAction();
    }
}

void DTitlebarPrivate::_q_aboutActionTriggered()
{
    DApplication *dapp = qobject_cast<DApplication *>(qApp);
    if (dapp) {
        dapp->handleAboutAction();
    }
}

void DTitlebarPrivate::_q_quitActionTriggered()
{
    DApplication *dapp = qobject_cast<DApplication *>(qApp);
    if (dapp) {
        dapp->handleQuitAction();
    }
}

bool DTitlebarPrivate::isUserManualExists() const
{
    const QString appName = qApp->applicationName();

    return QFile::exists("/usr/bin/dman") && \
           QFile::exists("/usr/share/dman/" + appName);
}

#endif

/**
 * \class DTitlebar
 * \brief The DTitlebar class is an universal title bar on the top of windows.
 * \param parent is the parent widget to be attached on.
 *
 * Usually you don't need to construct a DTitlebar instance by your self, you
 * can get an DTitlebar instance by DMainWindow::titlebar .
 */

/**
 * @brief DTitlebar::DTitlebar create an default widget with icon/title/and bottons
 * @param parent
 */
DTitlebar::DTitlebar(QWidget *parent) :
    QWidget(parent),
    DObject(*new DTitlebarPrivate(this))
{
    D_THEME_INIT_WIDGET(DTitlebar)

    D_D(DTitlebar);
    d->init();
    d->buttonArea->adjustSize();
    d->buttonArea->resize(d->buttonArea->size());
    d->titlePadding->setFixedSize(d->buttonArea->size());
}

#ifndef QT_NO_MENU
/**
 * @brief DTitlebar::menu holds the QMenu object attached to this title bar.
 * @return the QMenu object it holds, returns null if there's no one set.
 */
QMenu *DTitlebar::menu() const
{
    D_DC(DTitlebar);

    return d->menu;
}

/**
 * @brief DTitlebar::setMenu attaches a QMenu object to the title bar.
 * @param menu is the target menu.
 */
void DTitlebar::setMenu(QMenu *menu)
{
    D_D(DTitlebar);

    d->menu = menu;
    if (d->menu) {
        disconnect(this, &DTitlebar::optionClicked, 0, 0);
        connect(this, &DTitlebar::optionClicked, this, &DTitlebar::showMenu);
    }
}

#endif

/**
 * @brief DTitlebar::customWidget
 * @return the customized widget used in this title bar.
 *
 * One can set customized widget to show some extra widgets on the title bar.
 * \see DTitlebar::setCustomWidget()
 */
QWidget *DTitlebar::customWidget() const
{
    D_DC(DTitlebar);

    return d->customWidget;
}

///
/// \brief setWindowFlags
/// \param type
/// accpet  WindowTitleHint, WindowSystemMenuHint, WindowMinimizeButtonHint, WindowMaximizeButtonHint
/// and WindowMinMaxButtonsHint.
void DTitlebar::setWindowFlags(Qt::WindowFlags type)
{
    D_D(DTitlebar);

    d->useWindowFlags = true;

    if (d->titleLabel) {
        d->titleLabel->setVisible(type & Qt::WindowTitleHint);
    }
    if (d->iconLabel) {
        d->iconLabel->setVisible(type & Qt::WindowTitleHint);
    }

    d->minButton->setVisible(type & Qt::WindowMinimizeButtonHint);
    d->maxButton->setVisible(type & Qt::WindowMaximizeButtonHint);
    d->closeButton->setVisible(type & Qt::WindowCloseButtonHint);
    d->optionButton->setVisible(type & Qt::WindowSystemMenuHint);
    d->buttonArea->adjustSize();
    d->buttonArea->resize(d->buttonArea->size());

    if (d->titlePadding) {
        d->titlePadding->setFixedSize(d->buttonArea->size());
    }
}

#ifndef QT_NO_MENU
void DTitlebar::showMenu()
{
    D_D(DTitlebar);

    if (d->menu) {
        d->menu->exec(d->optionButton->mapToGlobal(d->optionButton->rect().bottomLeft()));
    } else {
        d->menu->exec(d->optionButton->mapToGlobal(d->optionButton->rect().bottomLeft()));
    }
}
#endif

void DTitlebar::showEvent(QShowEvent *event)
{
    D_D(DTitlebar);
    d->separator->setFixedWidth(width());
    d->separator->move(0, height() - d->separator->height());

#ifndef QT_NO_MENU
    d->_q_addDefaultMenuItems();
#endif

    QWidget::showEvent(event);

    if (DPlatformWindowHandle::isEnabledDXcb(window()))
        d->_q_onTopWindowMotifHintsChanged(window()->internalWinId());
}

void DTitlebar::mousePressEvent(QMouseEvent *event)
{
    D_D(DTitlebar);
    d->mousePressed = (event->buttons() == Qt::LeftButton);

#ifdef Q_OS_WIN
    Q_EMIT mousePosPressed(event->buttons(), event->globalPos());
#endif
    Q_EMIT mousePressed(event->buttons());
}

void DTitlebar::mouseReleaseEvent(QMouseEvent *event)
{
    D_D(DTitlebar);
    if (event->buttons() == Qt::LeftButton) {
        d->mousePressed = false;
    }
}

bool DTitlebar::eventFilter(QObject *obj, QEvent *event)
{
    D_D(DTitlebar);

    if (obj == d->parentWindow) {
        if (event->type() == QEvent::WindowStateChange) {
            d->maxButton->setMaximized(d->parentWindow->windowState() == Qt::WindowMaximized);
        }
    }

    return QWidget::eventFilter(obj, event);
}

void DTitlebar::resizeEvent(QResizeEvent *event)
{
    D_D(DTitlebar);

    d->optionButton->setFixedHeight(event->size().height());
    d->minButton->setFixedHeight(event->size().height());
    d->maxButton->setFixedHeight(event->size().height());
    d->closeButton->setFixedHeight(event->size().height());
    d->separator->setFixedWidth(event->size().width());
    return QWidget::resizeEvent(event);
}

/**
 * @brief DTitlebar::setCustomWidget is an overloaded function.
 * @param w is the widget to be used as the customize widget shown in the title
 * bar.
 * @param fixCenterPos indicates whether it should automatically move the
 * customize widget to the horizontal center of the title bar or not.
 */
void DTitlebar::setCustomWidget(QWidget *w, bool fixCenterPos)
{
    setCustomWidget(w, Qt::AlignCenter, fixCenterPos);
}

/**
 * @brief DTitlebar::setCustomWidget sets a customized widget to be used as the
 * central content of the title bar.
 * @param w is the widget to be used as the customize widget shown in the title
 * bar.
 * @param wflag is used to vertically align the widget.
 * @param fixCenterPos indicates whether it should automatically move the
 * customize widget to the horizontal center of the title bar or not.
 */
void DTitlebar::setCustomWidget(QWidget *w, Qt::AlignmentFlag wflag, bool fixCenterPos)
{
    D_D(DTitlebar);
    if (!w || w == d->titleArea) {
        return;
    }

    QSize old = d->buttonArea->size();

    QHBoxLayout *l = new QHBoxLayout;
    l->setSpacing(0);
    l->setMargin(0);

    if (fixCenterPos) {
        d->titlePadding = new QWidget;
        d->titlePadding->setFixedSize(old);
        l->addWidget(d->titlePadding);
    }

    l->addWidget(w, 0, wflag);
    qDeleteAll(d->coustomAtea->children());
    d->titleLabel = Q_NULLPTR;
    d->titleArea = Q_NULLPTR;
    d->iconLabel = Q_NULLPTR;
    d->titlePadding = Q_NULLPTR;
    d->coustomAtea->setLayout(l);
    d->buttonArea->resize(old);
    d->customWidget = w;

    w->resize(d->coustomAtea->size());
}

/**
 * @brief DTitlebar::setFixedHeight change the height of the title bar to
 * another value.
 * @param h is the target height.
 */
void DTitlebar::setFixedHeight(int h)
{
    D_D(DTitlebar);
    QWidget::setFixedHeight(h);
    d->coustomAtea->setFixedHeight(h);
    d->buttonArea->setFixedHeight(h);
}

/**
 * @brief DTitlebar::setSeparatorVisible sets the bottom separator of the title
 * bar and the window contents to be visible or not.
 * @param visible is the targeting value.
 */
void DTitlebar::setSeparatorVisible(bool visible)
{
    D_D(DTitlebar);
    if (visible) {
        d->separator->show();
        d->separator->raise();
    } else {
        d->separator->hide();
    }
}

/**
 * @brief DTitlebar::setTitle sets the title to be shown on the title bar.
 * @param title is the text to be used as the window title.
 */
void DTitlebar::setTitle(const QString &title)
{
    D_D(DTitlebar);
    if (d->titleLabel) {
        d->titleLabel->setText(title);
    }
}

/*!
 * \brief Use setIcon(const QIcon &icon) instead
 */
void DTitlebar::setIcon(const QPixmap &icon)
{
    D_D(DTitlebar);
    if (d->titleLabel) {
        d->titleLabel->setContentsMargins(0, 0, 0, 0);
        d->iconLabel->setPixmap(icon.scaled(DefaultIconWidth * icon.devicePixelRatio(),
                                            DefaultIconHeight * icon.devicePixelRatio(), Qt::KeepAspectRatio));
    }
}

/*!
 * @brief DTitlebar::setIcon sets the icon to be shown on the title bar.
 * @param icon is to be used as the window icon.
 */
void DTitlebar::setIcon(const QIcon &icon)
{
    D_D(DTitlebar);
    if (d->titleLabel) {
        d->titleLabel->setContentsMargins(0, 0, 0, 0);
        d->iconLabel->setPixmap(icon.pixmap(QSize(DefaultIconWidth, DefaultIconHeight)));
    }
}

void DTitlebar::setWindowState(Qt::WindowState windowState)
{
    D_D(DTitlebar);
    d->maxButton->setWindowState(windowState);
}

void DTitlebar::toggleWindowState()
{
    D_D(DTitlebar);

    d->_q_toggleWindowState();
}

/**
 * @brief DTitlebar::buttonAreaWidth returns the width of the area that all the
 * window buttons occupies.
 */
int DTitlebar::buttonAreaWidth() const
{
    D_DC(DTitlebar);
    return d->buttonArea->width();
}

/**
 * @brief DTitlebar::separatorVisible returns the visibility of the bottom
 * separator of the titlebar.
 */
bool DTitlebar::separatorVisible() const
{
    D_DC(DTitlebar);
    return d->separator->isVisible();
}

/**
 * @brief DTitlebar::setVisible overrides QWidget::setVisible(bool visible)
 */
void DTitlebar::setVisible(bool visible)
{
    D_D(DTitlebar);

    if (visible == isVisible()) {
        return;
    }

    QWidget::setVisible(visible);

    if (visible) {
        d->parentWindow = parentWidget();

        if (!d->parentWindow) {
            return;
        }

        d->parentWindow = d->parentWindow->window();
        d->parentWindow->installEventFilter(this);

        connect(d->maxButton, SIGNAL(clicked()), this, SLOT(_q_toggleWindowState()));
        connect(this, SIGNAL(doubleClicked()), this, SLOT(_q_toggleWindowState()));
        connect(d->minButton, SIGNAL(clicked()), this, SLOT(_q_showMinimized()));
        connect(d->closeButton, &DWindowCloseButton::clicked, d->parentWindow, &QWidget::close);
    } else {
        if (!d->parentWindow) {
            return;
        }

        d->parentWindow->removeEventFilter(this);

        disconnect(d->maxButton, SIGNAL(clicked()), this, SLOT(_q_toggleWindowState()));
        disconnect(this, SIGNAL(doubleClicked()), this, SLOT(_q_toggleWindowState()));
        disconnect(d->minButton, SIGNAL(clicked()), this, SLOT(_q_showMinimized()));
        disconnect(d->closeButton, &DWindowCloseButton::clicked, d->parentWindow, &QWidget::close);
    }
}

/**
 * @brief DTitlebar::resize resizes the title bar.
 * @param w is the target width.
 * @param h is the target height.
 */
void DTitlebar::resize(int w, int h)
{
    D_DC(DTitlebar);
    if (d->customWidget) {
        d->customWidget->resize(w - d->buttonArea->width(), h);
    }
}

/**
 * @brief DTitlebar::resize is an overloaded function.
 * @param sz the target size.
 *
 * @see DTitlebar::resize(int w, int h)
 */
void DTitlebar::resize(const QSize &sz)
{
    DTitlebar::resize(sz.width(), sz.height());
}

void DTitlebar::mouseMoveEvent(QMouseEvent *event)
{
    D_DC(DTitlebar);

    Qt::MouseButton button = event->buttons() & Qt::LeftButton ? Qt::LeftButton : Qt::NoButton;
    if (event->buttons() == Qt::LeftButton /*&& d->mousePressed*/) {
        Q_EMIT mouseMoving(button);
    }

#ifdef Q_OS_WIN
    if (d->mousePressed) {
        Q_EMIT mousePosMoving(button, event->globalPos());
    }
#endif
    QWidget::mouseMoveEvent(event);
}

void DTitlebar::mouseDoubleClickEvent(QMouseEvent *event)
{
    D_D(DTitlebar);

    if (event->buttons() == Qt::LeftButton) {
        d->mousePressed = false;
        Q_EMIT doubleClicked();
    }
}

DWIDGET_END_NAMESPACE

#include "moc_dtitlebar.cpp"
