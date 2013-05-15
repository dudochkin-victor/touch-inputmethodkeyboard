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



#include "layoutdata.h"
#include "mimkeymodel.h"

#include <QtAlgorithms>
#include <QDebug>

const QString LayoutData::mainSection("main");
const QString LayoutData::functionkeySection("functionkey");
const QString LayoutData::symbolsSymSection("symbols Sym");

LayoutSection::Row::~Row()
{
    qDeleteAll(keys);
}

LayoutData::LayoutData()
    : layoutOrientation(M::Landscape),
      layoutType(General)
{
}

LayoutData::~LayoutData()
{
}

LayoutData::LayoutType LayoutData::type() const
{
    return layoutType;
}

M::Orientation LayoutData::orientation() const
{
    return layoutOrientation;
}

int LayoutData::numSections() const
{
    return sections.size();
}

LayoutData::SharedLayoutSection LayoutData::section(int index) const
{
    if (index < 0 || index >= sections.size()) {
        return SharedLayoutSection(0);
    } else {
        return sections[index];
    }
}

LayoutData::SharedLayoutSection LayoutData::section(const QString &name) const
{
    SharedLayoutSection layoutSection = sectionMap.value(name);

    if (layoutSection.isNull()) {
        qWarning() << __PRETTY_FUNCTION__
                   << "Could not find requested section in current layout:"
                   << "'" << name << "'\n"
                   << " * Available sections:"
                   << sectionMap;

        layoutSection = SharedLayoutSection(new LayoutSection);
    }

    return layoutSection;
}

LayoutSection::LayoutSection()
    : mMaxColumns(0),
      movable(false),
      sectionType(Sloppy)
{
}

LayoutSection::LayoutSection(const QString &characters, bool rtl)
    : mMaxColumns(characters.length()),
      movable(false),
      sectionName("<dynamic section>"),
      sectionType(Sloppy)
{
    Row *row(new Row);
    row->heightType = Medium;
    rows.append(row);

    for (int i = 0; i < characters.length(); ++i) {
        MImKeyModel *key(new MImKeyModel(MImKeyModel::NormalStyle, MImKeyModel::Medium, true, rtl));

        row->keys.append(key);

        MImKeyBinding *binding(new MImKeyBinding(characters[i]));
        key->setBinding(*binding, false);
        key->setBinding(*binding, true);
    }
}

LayoutSection::~LayoutSection()
{
    qDeleteAll(rows);
}

const QString &LayoutSection::name() const
{
    return sectionName;
}

LayoutSection::Type LayoutSection::type() const
{
    return sectionType;
}

int LayoutSection::maxColumns() const
{
    return mMaxColumns;
}

int LayoutSection::rowCount() const
{
    return rows.count();
}

int LayoutSection::columnsAt(int row) const
{
    return (isInvalidRow(row) ? 0
                              : rows[row]->keys.count());
}

int LayoutSection::keyCount() const
{
    int numKeys = 0;

    for (QList<Row *>::const_iterator rowIter(rows.begin());
         rowIter != rows.end(); ++rowIter) {
         numKeys += (*rowIter)->keys.count();
    }

    return numKeys;
}

QList<int> LayoutSection::spacerIndices(int row) const
{
    return (isInvalidRow(row) ? QList<int>()
                              : rows[row]->spacerIndices);
}

MImKeyModel *LayoutSection::keyModel(int row, int column) const
{
    return (isInvalidCell(row, column) ? 0
                                       : rows[row]->keys[column]);
}

LayoutSection::RowHeightType LayoutSection::rowHeightType(int row) const
{
    return (isInvalidRow(row) ? LayoutSection::Medium
                              : rows[row]->heightType);
}

bool LayoutSection::isInvalidRow(int row) const
{
    return ((row < 0) || (row >= rows.count()));
}

bool LayoutSection::isInvalidCell(int row, int column) const
{
    return ((column < 0) || isInvalidRow(row) || (column >= rows[row]->keys.count()));
}
