/* * This file is part of m-keyboard *
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



#ifndef MKEYBOARDHOST_H
#define MKEYBOARDHOST_H

#include "mkeyboardcommon.h"
#include "mabstractinputmethod.h"
#include "keyevent.h"
#include "mplainwindow.h"

#include <QStringList>
#include <QTimer>
#include <QPointer>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <QTextStream>
#include <QPixmap>

class MGConfItem;
class MImCorrectionHost;
class MSceneWindow;
class MVirtualKeyboard;
class MVirtualKeyboardStyleContainer;
class MHardwareKeyboard;
class SymbolView;
class MImEngineWordsInterface;
class QWidget;
class MBanner;
class SharedHandleArea;
class MImToolbar;
class MAbstractInputMethodHost;
class SimpleFileLog;

//! Logic class for virtual keyboard
class MKeyboardHost: public MAbstractInputMethod
{
    Q_OBJECT

public:
    MKeyboardHost(MAbstractInputMethodHost *host,
                  QWidget *mainWindow);
    virtual ~MKeyboardHost();

    //! \brief Return the current instance, or 0 if none.
    static MKeyboardHost* instance();
    //! \brief Return the logger for touchpoints. Created if neccesary.
    QTextStream &touchPointLog();

    //! reimp
    virtual void handleFocusChange(bool focusIn);
    virtual void show();
    virtual void hide();
    // Note: application call this setPreedit() to tell keyboard about the preedit information.
    // If keyboard wants to update preedit, it should not call the method but call the lower
    // localSetPreedit().
    virtual void setPreedit(const QString &preeditString, int cursorPosition);
    virtual void update();
    virtual void reset();
    virtual void handleMouseClickOnPreedit(const QPoint &mousePos, const QRect &preeditRect);
    virtual void handleVisualizationPriorityChange(bool priority);
    virtual void handleAppOrientationAboutToChange(int angle);
    virtual void handleAppOrientationChanged(int angle);
    virtual void setToolbar(QSharedPointer<const MToolbarData> toolbar);
    virtual void setState(const QSet<MInputMethod::HandlerState> &state);
    virtual void processKeyEvent(QEvent::Type keyType, Qt::Key keyCode,
                                 Qt::KeyboardModifiers modifiers,
                                 const QString &text, bool autoRepeat, int count,
                                 quint32 nativeScanCode, quint32 nativeModifiers, unsigned long time);
    virtual void handleClientChange();
    virtual void switchContext(MInputMethod::SwitchDirection direction, bool enableAnimation);
    virtual QList<MAbstractInputMethod::MInputMethodSubView> subViews(MInputMethod::HandlerState state
                                                                  = MInputMethod::OnScreen) const;
    virtual void setActiveSubView(const QString &,
                                  MInputMethod::HandlerState state = MInputMethod::OnScreen);
    virtual QString activeSubView(MInputMethod::HandlerState state = MInputMethod::OnScreen) const;
    virtual void showLanguageNotification();
    virtual void setKeyOverrides(const QMap<QString, QSharedPointer<MKeyOverride> > &overrides);
    //! reimp_end

private slots:
    //! \brief Hide and show vkb based on symbol view visibility changes
    void handleSymbolViewVisibleChanged();

    //! \brief Adjust symbol view position when its size changed
    void handleSymbolViewGeometryChange();

    //! \brief Adjust vkb position when its size changed.
    void handleVirtualKeyboardGeometryChange();

    /*!
     * Handle key clicks from widgets
     * \param event internal key event
     */
    void handleKeyClick(const KeyEvent &event);

    /*!
     * Send QKeyEvent when key is pressed in direct input mode
     * \param event internal key event
     */
    void handleKeyPress(const KeyEvent &event);

    /*!
     * Send QKeyEvent when key is released in direct input mode
     * \param event internal key event
     */
    void handleKeyRelease(const KeyEvent &event);

    /*!
     * Handles user long press a key.
     * \param event internal key event
     */
    void handleLongKeyPress(const KeyEvent &event);

    /*!
     * \brief Commits \a string.
     *
     * If there exists preedit, the preedit will be replaced.
     */
    void commitString(const QString &string);

    /*! \brief Prepares vkb for orientation change when application is about to rotate.
     *
     * This should hide vkb.
     */
    void prepareOrientationChange();

    /*! \brief Finalizes orientation change after application has rotated.
     *
     * This should popup vkb if it was visible before orientation change.
     */
    void finalizeOrientationChange();

    //! Synchronize correction setting
    void synchronizeCorrectionSetting();

    //! Synchronize the correction setting whether the correction suggestion should be accepted by space.
    void synchronizeCorrectionSettingSpace();

    //! handles user initiated hiding of the keyboard
    void userHide();

    //! does one backspace and schedules the next if it is holding backspace.
    void autoBackspace();

    /*! \brief Sends request to copy or paste text
     *  \param action ImCopyPasteState Required action (copy or paste)
     */
    virtual void sendCopyPaste(CopyPasteState action);

    //! Changes plugin into given direction
    void switchPlugin(MInputMethod::SwitchDirection direction);

    //! Sends key event
    void sendKeyEvent(const QKeyEvent &);

    //! Sends string
    void sendString(const QString &);

    //! Sends string from toolbar
    void sendStringFromToolbar(const QString &);

    //! Handle symbol key click.
    void handleSymbolKeyClick();

    //! Updates the shift level for Symbol view.
    void updateSymbolViewLevel();

    //! Shows symbol view
    void showSymbolView();

    /*!
     * Receives modifier state changed signal or script changed signal  from hardware
     * keyboard, sends input mode indicator state notification to Application Framework
     * (Home screen status bar).
     */
    void handleHwKeyboardStateChanged();

    //!Receives modifier state changed and shows Caps Lock infobanner
    void handleVirtualKeyboardCapsLock();

    //! show FN/Caps Lock infobanner
    void showLockOnInfoBanner(const QString &notification);

    //! hide FN/Caps Lock infobanner
    void hideLockOnInfoBanner();

    //! Handle active layout is changed to \a layout for virtual keyboard.
    void handleVirtualKeyboardLayoutChanged(const QString &layout);

    void turnOffFastTyping();

    void handleAnimationFinished();

private:
    //! Configures the parts that may change dynamically.
    void prepareHideShowAnimation();

    void sendRegionEstimate();

    //! \brief Reset internal state, used by reset() and others
    void resetInternalState();

    void createCorrectionCandidateWidget();

    //! Update error correction flag
    void updateCorrectionState();

    //! update autocapitalization state
    void updateAutoCapitalization();

    //! update context
    void updateContext();

    /*!
     * Reset temporary shift state (shift on state set by user or auto capitalization,
     * besides capslocked) for virtual keyboard.
     */
    void resetVirtualKeyboardShiftState();

    //! Actual backspace operation
    void doBackspace();

    /*! \brief Handle key click event that changes the state of the keyboard.
     *
     *  This method should contain functionality that is common to
     *  both directmode and non-directmode.
     *  \sa handleTextInputKeyClick
     */
    void handleGeneralKeyClick(const KeyEvent &event);

    /*! \brief Handle key click event that relates to sending text.
     *
     *  This key click handling is not called in direct mode since
     *  it causes extra text to be sent to input context.
     *  \sa handleGeneralKeyClick
     */
    void handleTextInputKeyClick(const KeyEvent &event);

    //! initialize input engine
    void initializeInputEngine();

    //! \return input mode indicator corresponding to a dead \a key character or
    //! MInputMethod::NoIndicator if not a (supported) dead key character
    static MInputMethod::InputModeIndicator deadKeyToIndicator(const QChar &key);

    //! update input engine keyboard layout according keyboard layout.
    void updateEngineKeyboardLayout();

    //! set preedit string and cursor position.
    void localSetPreedit(const QString &preeditString, int replaceStart, int replaceLength,
                         int cursorPosition);

    /*!
     * \brief send preedit string and cursor to application and its style depends on \a candidateCount.
     *
     * \param string The preedit string
     * \param candidateCount The count of candidates suggested by input engine.
     * \param cursorPosition The cursor position inside preedit. Default value -1 indicates to hide the cursor.
     */
    void updatePreedit(const QString &string, int candidateCount, int replaceStart = 0,
                       int replaceLength = 0, int cursorPosition = -1);

    /*!
     * \brief This enum defines different mode for backspace clicking.
     */
    enum BackspaceMode {
        NormalBackspaceMode,      //!< backspace for normal state.
        AutoBackspaceMode,        //!< auto backspace state.
        WordTrackerBackspaceMode  //!< backspace when word tracker is visible.
    };

    //! start backspaceTimer according \a mode
    void startBackspace(BackspaceMode mode);

    //! Send return as press&release event pair, send as input method event with
    //! commit string otherwise.
    void sendCommitStringOrReturnEvent(const KeyEvent &event) const;

    //! update correction widget position.
    void updateCorrectionWidgetPosition();

    //! Sends backspace key event to application.
    void sendBackSpaceKeyEvent() const;

    void turnOnFastTyping();

    //! Changes +/- sign in front of text in number mode.
    void togglePlusMinus();

private:
    //! \brief Slides full-width QGraphicsWidgets up from the bottom of the display,
    //! aligning their bottom with the display bottom
    //!
    //! You only need to set the start value explicitly, end value is set automatically.
    class SlideUpAnimation : public QPropertyAnimation
    {
    public:
        explicit SlideUpAnimation(QObject *parent = 0);

        //! reimp
        void updateCurrentTime(int currentTime);
        //! reimp_end

    private:
        Q_DISABLE_COPY(SlideUpAnimation)
    };

    enum {
        OnScreenAnimationTime = 400, // in ms
        HardwareAnimationTime = 100  // in ms
    };

    class CycleKeyHandler; //! Reacts to cycle key press events.
    friend class CycleKeyHandler;
    QString preedit;

    MVirtualKeyboardStyleContainer *vkbStyleContainer;

    MImCorrectionHost *correctionHost;
    MVirtualKeyboard *vkbWidget;
    MHardwareKeyboard *hardwareKeyboard;
    SymbolView *symbolView;

    MImEngineWordsInterface *imCorrectionEngine;
    //! default input method error correction setting
    MGConfItem *inputMethodCorrectionSettings;
    MGConfItem *inputMethodCorrectionSettingsSpace;
    MGConfItem *inputMethodCorrectionEngine;

    QStringList candidates;

    int displayWidth;
    int displayHeight;

    bool rotationInProgress;

    //! error correction flag
    bool correctionEnabled;
    bool correctionAcceptedWithSpaceEnabled;

    //! FIXME: should we provide such a flag to on/off auto caps
    bool autoCapsEnabled;
    //! Contains true if autocapitalization decides to switch keyboard to upper case
    bool autoCapsTriggered;
    QString surroundingText;
    int cursorPos;
    int preeditCursorPos;
    bool hasSelection;

    int inputMethodMode;

    QTimer backspaceTimer;

    KeyEvent lastClickEvent;

    //! Keeps track of shift up/down status.
    bool shiftHeldDown;

    MSceneWindow *sceneWindow;

    //! current active state
    MInputMethod::HandlerState activeState;

    QPointer<MBanner> modifierLockOnBanner; //! widget to notify modifier is in locked state

    //! Indicates whether focus is in a widget according to focusChanged calls
    //! \sa focusChanged
    bool haveFocus;

    //! In practice show() and hide() correspond to application SIP (close)
    //! requests.  We track the current shown/SIP requested state using this variable.
    bool sipRequested;

    bool visualizationPriority;

    //! Contains true if multi-touch is enabled
    bool enableMultiTouch;

    //! Handle area containing toolbar widget.
    QPointer<SharedHandleArea> sharedHandleArea;

    //! Toolbar widget containing copy/paste and custom buttons.
    QPointer<MImToolbar> imToolbar;

    CycleKeyHandler *cycleKeyHandler;

    bool currentIndicatorDeadKey;

    //! Indicates whether engine layout need to be updated
    bool engineLayoutDirty;

    BackspaceMode backspaceMode;

    bool wordTrackerSuggestionAcceptedWithSpace;

    QTimer fastTypingTimeout;
    int fastTypingKeyCount;
    bool fastTypingEnabled;

    SlideUpAnimation slideUpAnimation;
    QPropertyAnimation& vkbFadeInAnimation;
    QPropertyAnimation& toolbarFadeInAnimation;
    QParallelAnimationGroup toolbarAndVkbFadeInAnimation;

    SimpleFileLog *touchPointLogHandle;
    MPlainWindow *view;

#ifdef UNIT_TEST
    friend class Ut_MKeyboardHost;
#endif
};

#endif
