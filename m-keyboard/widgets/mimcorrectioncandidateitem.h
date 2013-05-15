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

#ifndef MIMCORRECTIONCANDIDATEITEM_H
#define MIMCORRECTIONCANDIDATEITEM_H

#include <QTimer>
#include <MStylableWidget>
#include "mimcorrectioncandidateitemstyle.h"

class MImCorrectionCandidateItem: public MStylableWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(MImCorrectionCandidateItem)

    Q_PROPERTY(QString title READ title WRITE setTitle)

public:
    explicit MImCorrectionCandidateItem(const QString &title = QString(), QGraphicsItem *parent = 0);

    virtual ~MImCorrectionCandidateItem();

    /*
     * \brief Sets title label.
     */
    void setTitle(const QString &);

    /*
     * \brief Returns current title label.
     */
    QString title() const;

    /*!
     * \brief Returns the ideal width of the container widget.
     *
     * The ideal width is the actually used width of the title together with margins and paddings.
     */
    qreal idealWidth() const;
    
    /*!
     * \brief Select item.
     */
    void setSelected(bool);

    /*
     * \brief Returns selected state.
     */
    bool isSelected() const;

public Q_SLOTS:
    /*!
     \brief Makes the list cell to send clicked() signal.
     */
     void click();

    /*!
     *\brief Makes the list cell to send longTapped signal.
     *\param pos The position of the tap.
     */
    void longTap();

protected:
    /*! \reimp */
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void drawContents(QPainter *painter, const QStyleOptionGraphicsItem *option) const;
    /*! \reimp_end */

Q_SIGNALS:
    /*!
     * \brief The signal is emitted when the item is clicked.
     */
    void clicked();

    /*!
     * \brief The signal is emitted when the item has been tapped and holded.
     */
    void longTapped();

private Q_SLOTS:
    void applyQueuedStyleModeChange();

    void handleVisibilityChanged();

    /*!
     * Update stored style stuffs when the theme changed.
     */
    void onThemeChangeCompleted();

private:
    void updateStyleMode();

    void setupLongTapTimer();

    bool mSelected;
    bool mDown;
    QString mTitle;
    QTimer styleModeChangeTimer;
    QTimer longTapTimer;
    bool queuedStyleModeChange;

    M_STYLABLE_WIDGET(MImCorrectionCandidateItemStyle)
};

#endif
