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

#ifndef SYMBOLVIEW_H
#define SYMBOLVIEW_H

#include "keyeventhandler.h"
#include "mkeyboardcommon.h"
#include "layoutdata.h"
#include "reactionmappaintable.h"

#include <minputmethodnamespace.h>
#include <MWidget>

#include <QPointer>

class MReactionMap;
class MSceneManager;
class MVirtualKeyboardStyleContainer;
class HorizontalSwitcher;
class LayoutsManager;
class QGraphicsLinearLayout;
class QGraphicsSceneMouseEvent;
class KeyEvent;
class Handle;
class MKeyOverride;

/*!
 * \brief SymbolView is used to show different layouts symbols/upper case/lower case
 */
class SymbolView : public MWidget, public ReactionMapPaintable
{
    Q_OBJECT

public:
    enum ShowMode {
        NormalShowMode,         // Shows page normally
        FollowMouseShowMode     // Shows page temporarily and grabs the mouse
    };

    enum HideMode {
        NormalHideMode,         // Hides page normally
        TemporaryHideMode       // Hides page temporarily, page is hidden temporarily during screen rotation.
    };

    /*!
     * \brief Constructor for creating an SymbolView object.
     * \param baseSize QSize
     * \param parent Parent object.
     */
    SymbolView(const LayoutsManager &layoutsManager, const MVirtualKeyboardStyleContainer *,
               const QString &layout, QGraphicsWidget *parent = 0);

    /*!
     * Destructor
     */
    virtual ~SymbolView();

    void prepareToOrientationChange();

    void finalizeOrientationChange();

    void setShiftState(ModifierState newShiftState);

    //! Returns current level.
    int currentLevel() const;

    //! Return TRUE if widget is activated
    bool isActive() const;

    /*! Returns interactive region currently occupied by SymbolView,
     *  in scene coordinates.
     */
    QRegion interactiveRegion() const;

    //! Returns the page count.
    int pageCount() const;

    //! Returns the index of current page.
    int currentPage() const;

    /*!
     * \brief If \a hidden is true, hides active symbol view. Otherwise, shows temporarily inactive symbol view.
     */
    void setTemporarilyHidden(bool hidden);

    /*! \reimp */
    bool isPaintable() const;
    /*! \reimp_end */

public slots:
    /*!
     * Handler to show the page.
     * If the page is already visible, then just return.
     * \param mode Value is NormalShowMode or FollowMouseShowMode. Default is NormalShowMode. \sa ShowMode.
     */
    void showSymbolView(ShowMode mode = NormalShowMode);

    /*!
     * Handler to hide the page
     * \param mode Value is NormalHideMode or TemporaryHideMode. Default is NormalHideMode. \sa HideMode.
     */
    void hideSymbolView(HideMode mode = NormalHideMode);

    /*!
     * Changes between onscreen and hardware keyboard modes. Symbol view's contents depend on which one is used.
     * When hardware keyboard is used, symbol view has hw layout specific layout variant, and function row is hidden.
     * \param state New state to be applied.
     */
    void setKeyboardState(MInputMethod::HandlerState state);

    /*!
     * Method to load svc corresponding to given layout file name
     * \param layoutFile const QString&, new layout file name
     */
    void setLayout(const QString &layoutFile);

    /*!
     * \brief Selects the next page if current is not the rightmost.
     */
    void switchToNextPage();

    /*!
     * \brief Selects the previous page if current is not the leftmost.
     */
    void switchToPrevPage();

    //! \brief Paint symview's area to global reaction map.
    void paintReactionMap(MReactionMap *reactionMap, QGraphicsView *view);

    /*!
     * \brief Resets current active key area (release active keys in the main keyboard).
     * \param resetCapsLock whether to reset shift key when in caps-lock mode.
     *        By default, always resets shift key.
     */
    void resetCurrentKeyArea(bool resetCapsLock = false);

    /*!
     * \brief Uses custom key overrides which is defined by \a overrides.
     */
    void setKeyOverrides(const QMap<QString, QSharedPointer<MKeyOverride> > &overrides);

signals:
    //! Used to broadcast shift state to all pages/MImAbstractKeyAreas.
    void levelSwitched(int);

    /*!
     * Emitted on clicked on layout
     */
    void keyClicked(const KeyEvent &event);

    /*!
     * Emitted when key is pressed
     */
    void keyPressed(const KeyEvent &event);

    /*!
     * Emitted when key is released
     */
    void keyReleased(const KeyEvent &event);

    /*!
     * \brief Emitted when key is long pressed
     */
    void longKeyPressed(const KeyEvent &event);

protected:
    /*! \reimp */
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual bool sceneEventFilter(QGraphicsItem *watched, QEvent *event);
    /*! \reimp_end */

private slots:
    void onSwitchStarting(QGraphicsWidget *current, QGraphicsWidget *next);
    void onSwitchDone();

    //! Handler for shift pressed state change (separate from shift state).
    void handleShiftPressed(bool shiftPressed);

    //! Special handling for clicked keys, not handled through KeyEventHandler.
    void handleKeyClicked(const MImAbstractKey *);

    //! When hardware keyboard layout has changed, reload contents if currently in Hardware state.
    void handleHwLayoutChange();

private:
    //! Main layout indices
    enum LayoutIndex {
        GripIndex,
        KeyboardIndex
    };

    //! symbol view state wrt. \a showSymbolView / \a hideSymbolView calls.
    enum Activity {
        Active,                 // After showSymbolView(NormalShowMode) call
        TemporarilyActive,      // After showSymbolView(FollowMouseShowMode) call
        Inactive,               // After hideSymbolView(NormalHideMode) calls
        TemporarilyInactive     // After hideSymbolView(TemporaryHideMode)
    };

    //! Ensure that content, position and size are correct for the current orientation
    void organizeContent();

    //! \brief Creates necessary widget layouts where keys will be added.
    void setupLayout();

    //! \brief Reloads keys/buttons based on current language and orientation.
    void reloadContent();

    //! \brief Reloads switcher with pages from given \a layout, selecting the page \a selectPage.
    void loadSwitcherPages(const LayoutData *layout, unsigned int selectPage = 0);

    //! \brief Helper method to create and add a new page.
    void addPage(const LayoutData::SharedLayoutSection &symbolSection);

    //! \brief Creates MImAbstractKeyArea from given section model and connects appropriate signals.
    MImAbstractKeyArea *createMImAbstractKeyArea(const LayoutData::SharedLayoutSection &section,
                                       bool enablePopup = true);

    //! Getter for style container
    const MVirtualKeyboardStyleContainer &style() const;

    //! Retrieves title of a symbol section from given page.
    QString pageTitle(int pageIndex) const;

    //! Connect signals from a \a handle widget
    void connectHandle(Handle *handle);

    //! Current style being used.
    const MVirtualKeyboardStyleContainer *styleContainer;

    //! scene manager
    const MSceneManager &sceneManager;

    //! To check if symbol view is opened
    Activity activity;

    //! Zero-based index to currently active page
    int activePage;

    //! Case selector: 0 for lower case, 1 for upper case
    ModifierState shiftState;

    const LayoutsManager &layoutsMgr;

    QPointer<HorizontalSwitcher> pageSwitcher;

    M::Orientation currentOrientation;

    QString currentLayout;

    /*! This is true when one of the pages has received mouse
     *  down event. It is used to check whether on page switch
     *  we need to reassign mouse grab.
     */
    bool mouseDownKeyArea;

    //! The main layout holds toolbar, handles and key button areas.
    QGraphicsLinearLayout *mainLayout;

    //! Handler for button events
    KeyEventHandler eventHandler;

    //! Contains true if multi-touch is enabled
    bool enableMultiTouch;

    MInputMethod::HandlerState activeState;

    bool hideOnQuickPick;

#ifdef UNIT_TEST
    friend class Ut_SymbolView;
#endif
};

#endif

