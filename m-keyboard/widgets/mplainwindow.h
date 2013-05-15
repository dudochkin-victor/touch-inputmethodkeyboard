/* * This file is part of meego-im-framework *
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
#ifndef MPLAINWINDOW_H
#define MPLAINWINDOW_H

#include <MWindow>
#include <QPixmap>

class QRegion;
class MAbstractInputMethodHost;

class MPlainWindow
    : public MWindow
{
    Q_OBJECT

public:
    explicit MPlainWindow(const MAbstractInputMethodHost *host = 0,
                          QWidget *parent = 0);
    virtual ~MPlainWindow();
    static MPlainWindow *instance();

protected:
    //! \reimp
    virtual bool viewportEvent(QEvent *event);
    virtual void drawBackground(QPainter *painter,
                                const QRectF &rect);
    //! \reimp_end

private:
    Q_DISABLE_COPY(MPlainWindow);

    static MPlainWindow *m_instance;
    const MAbstractInputMethodHost *const host;
};

#endif // MPLAINWINDOW_H
