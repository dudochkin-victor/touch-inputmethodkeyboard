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

#ifndef MIMCORRECTIONCANDIDATEITEMSTYLE_H
#define MIMCORRECTIONCANDIDATEITEMSTYLE_H

#include <MWidgetStyle>

class MImCorrectionCandidateItemStyle : public MWidgetStyle
{
    Q_OBJECT
    M_STYLE(MImCorrectionCandidateItemStyle)

    M_STYLE_ATTRIBUTE(QFont, font, Font)
    M_STYLE_ATTRIBUTE(QColor, fontColor, FontColor)

    M_STYLE_ATTRIBUTE(int, pressTimeout, PressTimeout)
    M_STYLE_ATTRIBUTE(int, releaseMissDelta, ReleaseMissDelta)
    M_STYLE_ATTRIBUTE(int, longTapTimeout, LongTapTimeout)
};

class M_EXPORT MImCorrectionCandidateItemStyleContainer : public MWidgetStyleContainer
{
    M_STYLE_CONTAINER(MImCorrectionCandidateItemStyle)

    M_STYLE_MODE(Landscape)
    M_STYLE_MODE(Portrait)
};

#endif

