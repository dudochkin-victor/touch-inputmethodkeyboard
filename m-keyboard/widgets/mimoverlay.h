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

#ifndef MIMOVERLAY_H
#define MIMOVERLAY_H

#include <MSceneWindow>

/*!
  * \brief The MImOverLay class could be used to show an plain translucent overlay widget.
  *
  * MImOverlay is a translucent overlay widget, fully covering the visible screen area. While visible,
  * it prevents mouse and touch events from reaching the virtual keyboard or the application.
*/
class MImOverlay : public MSceneWindow
{
    Q_OBJECT

    friend class Ut_MImOverlay;

public:
    //! Constructor
    explicit MImOverlay();

    //! Destructor
    virtual ~MImOverlay();

protected slots:
    virtual void handleOrientationChanged();

protected:
    /*! \reimp */
    virtual bool sceneEvent(QEvent *event);
    /*! \reimp_end */

private:
    Q_DISABLE_COPY(MImOverlay)
};

#endif
