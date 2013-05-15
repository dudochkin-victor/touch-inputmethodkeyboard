/* * This file is part of meego-keyboard *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Contact: Nokia Corporation (directui@nokia.com)
 *
 * If you have questions regarding the use of this file, please contact
 * Nokia at directui@nokia.com.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */



#include "mimtoolbar.h"
#include "mtoolbarbutton.h"
#include "mtoolbarlabel.h"
#include "reactionmapwrapper.h"
#include "mplainwindow.h"

#include <mtoolbardata.h>
#include <mtoolbaritem.h>
#include <mtoolbarlayout.h>
#include <minputmethodnamespace.h>

#include <MNamespace>
#include <QKeySequence>
#include <QGraphicsLinearLayout>
#include <QDebug>
#include <MSceneManager>

namespace
{
    const QString ObjectNameToolbar("MImToolbar");
    const QString ObjectNameToolbarLeft("VirtualKeyboardToolbarLeft");
    const QString ObjectNameToolbarRight("VirtualKeyboardToolbarRight");
    const QString ObjectNameToolbarCenter("VirtualKeyboardToolbarCenter");
};

MImToolbar::MImToolbar(QGraphicsWidget *parent)
    : MStylableWidget(parent),
      ReactionMapPaintable(),
      textSelected(false),
      leftBar(this),
      rightBar(this),
      centerBar(this),
      arrangeWidgetsCalled(false),
      arrangeWidgetsDisabledCount(0)
{
    leftBar.setObjectName(ObjectNameToolbarLeft);
    rightBar.setObjectName(ObjectNameToolbarRight);
    centerBar.setObjectName(ObjectNameToolbarCenter);
    setObjectName(ObjectNameToolbar);

    setupLayout();

    connect(this, SIGNAL(visibleChanged()), this, SLOT(arrangeWidgets()));
    connect(MTheme::instance(), SIGNAL(themeChangeCompleted()),
            this, SLOT(updateFromStyle()));

    // Request a reaction map painting if it appears
    connect(this, SIGNAL(displayEntered()), &signalForwarder, SIGNAL(requestRepaint()));
}

MImToolbar::~MImToolbar()
{
}

void MImToolbar::setupLayout()
{
    QGraphicsLinearLayout *mainLayout = new QGraphicsLinearLayout(Qt::Horizontal, this);

    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Empty button bars are hidden.
    leftBar.hide();
    rightBar.hide();
    centerBar.hide();
    // Add the left and right side WidgetBar widgets with a stretch item in between.
    mainLayout->addItem(&leftBar);
    mainLayout->addStretch();
    mainLayout->addItem(&centerBar);
    mainLayout->addStretch();
    mainLayout->addItem(&rightBar);

    mainLayout->setAlignment(&leftBar, Qt::AlignBottom | Qt::AlignLeft);
    mainLayout->setAlignment(&centerBar, Qt::AlignBottom | Qt::AlignHCenter);
    mainLayout->setAlignment(&rightBar, Qt::AlignBottom | Qt::AlignRight);

    connect(&leftBar, SIGNAL(regionUpdated()), this, SLOT(arrangeWidgets()));
    connect(&rightBar, SIGNAL(regionUpdated()), this, SLOT(arrangeWidgets()));
    connect(&centerBar, SIGNAL(regionUpdated()), this, SLOT(arrangeWidgets()));

    leftBar.setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    centerBar.setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed));
    rightBar.setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed));

    resize(geometry().width(), mainLayout->preferredHeight());
}

QRegion MImToolbar::region() const
{
    QRegion region;

    if (isVisible()) {
        region = QRegion(mapRectToScene(rect()).toRect());
    }
    return region;
}

void MImToolbar::handleButtonClick(MToolbarItem *item)
{
    if (!item || item->type() != MInputMethod::ItemButton)
        return;

    foreach(QSharedPointer<const MToolbarItemAction> action, item->actions()) {
        switch (action->type()) {
        case MInputMethod::ActionSendKeySequence:
            sendKeySequence(action->keys());
            break;
        case MInputMethod::ActionSendString:
            sendStringRequest(action->text());
            break;
        case MInputMethod::ActionSendCommand:
            //TODO:not support yet
            break;
        case MInputMethod::ActionCopy:
            emit copyPasteRequest(InputMethodCopy);
            break;
        case MInputMethod::ActionPaste:
            emit copyPasteRequest(InputMethodPaste);
            break;
        case MInputMethod::ActionShowGroup:
            showGroup(action->group());
            break;
        case MInputMethod::ActionHideGroup:
            hideGroup(action->group());
            break;
        case MInputMethod::ActionClose:
            emit closeKeyboardRequest();
            break;
        case MInputMethod::ActionUndefined:
        case MInputMethod::ActionCopyPaste:
            break;
        }
    }
}

void MImToolbar::setSelectionStatus(bool selection)
{
    if (textSelected != selection) {
        textSelected = selection;
        if (isVisible())
            updateVisibility();
    }
}

void MImToolbar::updateItemVisibility(const QSharedPointer<MToolbarItem> &item) const
{
    if ((item->showOn() == MInputMethod::VisibleAlways)
        || (textSelected && item->showOn() == MInputMethod::VisibleWhenSelectingText)
        || (!textSelected && item->hideOn() == MInputMethod::VisibleWhenSelectingText)) {
        item->setVisible(true, false);
    }

    if ((!textSelected && item->showOn() == MInputMethod::VisibleWhenSelectingText)
        || (textSelected && item->hideOn() == MInputMethod::VisibleWhenSelectingText)) {
        item->setVisible(false, false);
    }
}

void MImToolbar::updateVisibility()
{
    suppressArrangeWidgets(true);
    if (currentToolbar) {
        foreach (const QSharedPointer<MToolbarItem> item, currentToolbar->items()) {
            updateItemVisibility(item);
        }
    }
    suppressArrangeWidgets(false);
}

void MImToolbar::updateFromStyle()
{
    prepareGeometryChange(); // notify scene about changes in bounding rect
}

void MImToolbar::loadCustomWidgets()
{
    if (!currentToolbar) {
        return;
    }

    const M::Orientation orientation = MPlainWindow::instance()->sceneManager()->orientation();
    QSharedPointer<const MToolbarLayout> layout = currentToolbar->layout(static_cast<MInputMethod::Orientation>(orientation));

    if (layout.isNull()) {
        qWarning() << __PRETTY_FUNCTION__
                   << "Could not find layout in current toolbar. Orientation was:"
                   << orientation;
        return;
    }

    foreach (QSharedPointer<MToolbarItem> item, layout->items()) {
        createAndAppendWidget(item);
    }
}

void MImToolbar::createAndAppendWidget(const QSharedPointer<MToolbarItem> &item)
{
    MWidget *widget = 0;
    WidgetBar *sidebar = 0;

    if (item->alignment() == Qt::AlignLeft) {
        sidebar = &leftBar;
    } else if (item->alignment() == Qt::AlignRight) {
        sidebar = &rightBar;
    } else {
        sidebar = &centerBar;
    }

    updateItemVisibility(item);

    if (item->type() == MInputMethod::ItemButton) {
        widget = new MToolbarButton(item, sidebar);

        connect(widget, SIGNAL(clicked(MToolbarItem*)),
                this, SLOT(handleButtonClick(MToolbarItem*)));
    } else {
        widget = new MToolbarLabel(item, sidebar);
    }
    customWidgets.append(widget);
    // We should update the reaction map if the custom toolbar elements are changing.
    connect(widget, SIGNAL(geometryChanged()),
            &signalForwarder, SIGNAL(requestRepaint()), Qt::UniqueConnection);
    connect(widget, SIGNAL(displayEntered()),
            &signalForwarder, SIGNAL(requestRepaint()), Qt::UniqueConnection);
    connect(widget, SIGNAL(displayExited()),
            &signalForwarder, SIGNAL(requestRepaint()), Qt::UniqueConnection);
    if (sidebar->count() == 0) {
        // must be done before appending so that isVisible() tells the truth
        sidebar->show();
    }
    sidebar->append(widget, item->isVisible());
}

void MImToolbar::unloadCustomWidgets()
{
    qDeleteAll(customWidgets);
    customWidgets.clear();
    leftBar.cleanup();
    rightBar.cleanup();
    centerBar.cleanup();
}

void MImToolbar::suppressArrangeWidgets(bool suppress)
{
    arrangeWidgetsDisabledCount += suppress ? 1 : -1;
    Q_ASSERT(arrangeWidgetsDisabledCount >= 0);

    if (!suppress && (arrangeWidgetsDisabledCount == 0) && arrangeWidgetsCalled) {
        arrangeWidgets();
    } else if (suppress && (arrangeWidgetsDisabledCount == 1)) {
        arrangeWidgetsCalled = false;
    }
}

void MImToolbar::arrangeWidgets()
{
    if (arrangeWidgetsDisabledCount > 0) {
        arrangeWidgetsCalled = true;
        return;
    }

    if (!layout()) {
        qCritical() << __PRETTY_FUNCTION__ << "Layout does not exist";
    }

    if (isVisible()) {
        layout()->invalidate();
        layout()->activate();
        resize(geometry().width(), layout()->preferredHeight());
    }

    emit regionUpdated();
}

void MImToolbar::showGroup(const QString &group)
{
    if (!currentToolbar) {
        return;
    }

    suppressArrangeWidgets(true);

    foreach (const QSharedPointer<MToolbarItem> item, currentToolbar->items()) {
        if (item->group() == group && !(item->isVisible())) {
            item->setVisible(true, false);
        }
    }

    suppressArrangeWidgets(false);
}

void MImToolbar::hideGroup(const QString &group)
{
    if (!currentToolbar) {
        return;
    }

    suppressArrangeWidgets(true);

    foreach (const QSharedPointer<MToolbarItem> item, currentToolbar->items()) {
        if (item->group() == group && item->isVisible()) {
            item->setVisible(false, false);
        }
    }

    suppressArrangeWidgets(false);
}

void MImToolbar::sendKeySequence(const QString &keys)
{
    const QKeySequence keysequence(keys);
    //translate the keys string to QKeyEvent by QKeySequence
    if (!keysequence.isEmpty()) {
        for (uint i = 0; i < keysequence.count(); i++) {
            int key = keysequence[i];
            Qt::KeyboardModifiers modify = keyModifiers(key);
            key -= modify;
            QString text;
            if (modify == Qt::NoModifier || modify == Qt::ShiftModifier)
                text = QString(key);
            //send both KeyPress and KeyRelease
            QKeyEvent press(QEvent::KeyPress, key, modify, text);
            emit sendKeyEventRequest(press);
            QKeyEvent release(QEvent::KeyRelease, key, modify, text);
            emit sendKeyEventRequest(release);
        }
    }
}

Qt::KeyboardModifiers MImToolbar::keyModifiers(int key) const
{
    Qt::KeyboardModifiers modify = Qt::NoModifier;
    if (key & Qt::CTRL)
        modify |= Qt::ControlModifier;
    if (key & Qt::ALT)
        modify |= Qt::AltModifier;
    if (key & Qt::SHIFT)
        modify |= Qt::ShiftModifier;
    if (key & Qt::META)
        modify |= Qt::MetaModifier;
    return modify;
}

void MImToolbar::showToolbarWidget(QSharedPointer<const MToolbarData> toolbar)
{
    if (toolbar == currentToolbar) {
        return;
    }

    unloadCustomWidgets();

    currentToolbar = toolbar;
    loadCustomWidgets();

    if (isVisible()) {
        updateVisibility();
        // The content has been changed -> Repaint the reaction maps
        signalForwarder.emitRequestRepaint();
    }
    arrangeWidgets();
}

void MImToolbar::hideToolbarWidget()
{
    currentToolbar.clear();
    unloadCustomWidgets();
    arrangeWidgets();
}

void MImToolbar::paintReactionMap(MReactionMap *reactionMap, QGraphicsView *view)
{
#ifndef HAVE_REACTIONMAP
    Q_UNUSED(reactionMap);
    Q_UNUSED(view);
    return;
#else
    if (!layout()) {
        qCritical() << __PRETTY_FUNCTION__ << "Layout does not exist";
    }

    layout()->activate();

    reactionMap->setTransform(this, view);
    reactionMap->setInactiveDrawingValue();
    reactionMap->fillRectangle(boundingRect());

    // Draw all widgets geometries.
    reactionMap->setDrawingValue(MImReactionMap::Press, MImReactionMap::Release);

    QGraphicsLinearLayout *mainLayout = static_cast<QGraphicsLinearLayout*>(layout());

    if (!mainLayout) {
        return;
    }

    mainLayout->activate();

    for (int j = 0; j < mainLayout->count(); ++j) {
        WidgetBar *sidebar = dynamic_cast<WidgetBar *>(mainLayout->itemAt(j));
        if (!sidebar || !sidebar->isVisible()) {
            continue;
        }

        // Buttons sometimes require this.
        sidebar->layout()->activate();

        reactionMap->setDrawingValue(MImReactionMap::Press, MImReactionMap::Release);

        for (int i = 0; i < sidebar->count(); ++i) {
            QGraphicsWidget *widget = sidebar->widgetAt(i);

            if (widget && widget->isVisible()
                    && !qobject_cast<MToolbarLabel*>(widget)) {

                reactionMap->setTransform(widget, view);
                reactionMap->fillRectangle(widget->boundingRect());
            }
            // Otherwise leave as inactive.
        }
    }
#endif // HAVE_REACTIONMAP
}

void MImToolbar::finalizeOrientationChange()
{
    //use brute force: destroy everything and construct it again
    unloadCustomWidgets();
    loadCustomWidgets();

    if (isVisible()) {
        blockSignals(true);
        arrangeWidgets();
        blockSignals(false);
    }
}

QRectF MImToolbar::boundingRect() const
{
    return QRectF(-style()->marginLeft(), -style()->marginTop(),
                  size().width() + style()->marginLeft() + style()->marginRight(),
                  size().height() + style()->marginTop() + style()->marginBottom());
}

bool MImToolbar::isPaintable() const
{
    return isVisible();
}

const MToolbarData *MImToolbar::currentToolbarData() const
{
    return currentToolbar.data();
}
