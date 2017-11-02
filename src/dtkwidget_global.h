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

#pragma once

#include <dtkcore_global.h>

#include <QtCore/QMetaMethod>

#define DWIDGET_NAMESPACE Widget
#define DTK_WIDGET_NAMESPACE DTK_NAMESPACE::Widget

#define DWIDGET_BEGIN_NAMESPACE namespace DTK_NAMESPACE { namespace DWIDGET_NAMESPACE {
#define DWIDGET_END_NAMESPACE }}
#define DWIDGET_USE_NAMESPACE using namespace DTK_WIDGET_NAMESPACE;

namespace Dtk
{
namespace Widget
{

#if defined(STATIC_LIB)
#define DWIDGET_INIT_RESOURCE() \
    do { \
        Q_INIT_RESOURCE(icons); \
        Q_INIT_RESOURCE(dui_theme_dark); \
        Q_INIT_RESOURCE(dui_theme_light); } while (0)
#endif

}
}

#if defined(STATIC_LIB)
#  define LIBDTKWIDGETSHARED_EXPORT
#else
#if defined(LIBDTKWIDGET_LIBRARY)
#  define LIBDTKWIDGETSHARED_EXPORT Q_DECL_EXPORT
#else
#  define LIBDTKWIDGETSHARED_EXPORT Q_DECL_IMPORT
#endif
#endif

#define DTKWIDGET_DECL_DEPRECATED Q_DECL_DEPRECATED

#define D_THEME_INIT_WIDGET(className, ...) do{\
        DThemeManager * manager = DThemeManager::instance(); \
        this->setStyleSheet(this->styleSheet() + manager->getQssForWidget(#className, this)); \
        connect(manager, &DThemeManager::themeChanged, this, [this, manager] (QString) { \
            this->setStyleSheet(manager->getQssForWidget(#className, this)); \
        });\
        QStringList list = QString(#__VA_ARGS__).replace(" ", "").split(",");\
        const QMetaObject *self = metaObject();\
        Q_FOREACH (const QString &str, list) {\
            if(str.isEmpty())\
                continue;\
            connect(this, self->property(self->indexOfProperty(str.toLatin1().data())).notifySignal(),\
                    manager, manager->metaObject()->method(manager->metaObject()->indexOfMethod("updateQss()")));\
        } \
    } while (0);
