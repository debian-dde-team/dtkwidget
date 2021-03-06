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

#ifndef DCOMBOBOX_P_H
#define DCOMBOBOX_P_H

#include "dcombobox.h"

#include <DObjectPrivate>

DWIDGET_BEGIN_NAMESPACE

class DComboBoxItem;

class DComboBoxPrivate : public DTK_CORE_NAMESPACE::DObjectPrivate
{
protected:
    DComboBoxPrivate(DComboBox *parent);

    void setMaskLabel(DComboBoxItem *label);

private:
    void init();
    void initInsideFrame();
    void restylePopupEnds();

    void _q_slotCurrentIndexChange(int index);

    DComboBoxItem *maskLabel = NULL;    //mask label

    QModelIndex hoverIndex;
    QString normalTickImg = "";
    QString hoverTickImg = "";
    QString insensitiveTickImg = "";
    bool alert = false;

    QPointer<DComboBoxItem> checkedItem;
    QPointer<DComboBoxItem> hoveredItem;

    const int MAX_VISIBLE_ITEMS = 16;
    const int OUTSIDE_BORDER_WIDTH = 1;

    D_DECLARE_PUBLIC(DComboBox)
};

class DComboBoxItem : public QLabel
{
    Q_OBJECT

    Q_PROPERTY(bool checked READ checked WRITE setChecked NOTIFY checkedChanged)
    Q_PROPERTY(bool hovered READ hovered WRITE setHovered NOTIFY hoveredChanged)

public:
    explicit DComboBoxItem(QWidget *parent = 0);

    bool checked() const;
    Q_SLOT void setChecked(bool value);

    bool hovered() const;
    Q_SLOT void setHovered(bool value);

    virtual QVariantMap data() const;
    virtual void setData(const QVariantMap &map);

Q_SIGNALS:
    void checkedChanged(bool checked);
    void hoveredChanged(bool hovered);

private:
    bool m_checked = false;
    bool m_hovered = false;
};

DWIDGET_END_NAMESPACE

#endif // DCOMBOBOX_P_H

