/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2015 - ROLI Ltd.

   Permission is granted to use this software under the terms of either:
   a) the GPL v2 (or any later version)
   b) the Affero GPL v3

   Details of these licenses can be found at: www.gnu.org/licenses

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.juce.com for more information.

  ==============================================================================
*/

class JuceMainMenuHandler   : private MenuBarModel::Listener,
                              private DeletedAtShutdown
{
public:
    JuceMainMenuHandler()
    {
        static JuceMenuCallbackClass cls;
        callback = [cls.createInstance() init];
        JuceMenuCallbackClass::setOwner (callback, this);
    }

    ~JuceMainMenuHandler()
    {
        setMenu (nullptr, nullptr, String());

        jassert (instance == this);
        instance = nullptr;

        [callback release];
    }

    void setMenu (MenuBarModel* const newMenuBarModel,
                  const PopupMenu* newExtraAppleMenuItems,
                  const String& recentItemsName)
    {
        recentItemsMenuName = recentItemsName;

        if (currentModel != newMenuBarModel)
        {
            if (currentModel != nullptr)
                currentModel->removeListener (this);

            currentModel = newMenuBarModel;

            if (currentModel != nullptr)
                currentModel->addListener (this);

            menuBarItemsChanged (nullptr);
        }

        extraAppleMenuItems = createCopyIfNotNull (newExtraAppleMenuItems);
    }

    void addTopLevelMenu (NSMenu* parent, const PopupMenu& child, const String& name, int menuId, int topLevelIndex)
    {
        NSMenuItem* item = [parent addItemWithTitle: juceStringToNS (name)
                                             action: nil
                                      keyEquivalent: nsEmptyString()];

        NSMenu* sub = createMenu (child, name, menuId, topLevelIndex, true);

        [parent setSubmenu: sub forItem: item];
        [sub setAutoenablesItems: false];
        [sub release];
    }

    void updateTopLevelMenu (NSMenuItem* parentItem, const PopupMenu& menuToCopy, const String& name, int menuId, int topLevelIndex)
    {
        // Note: This method used to update the contents of the existing menu in-place, but that caused
        // weird side-effects which messed-up keyboard focus when switching between windows. By creating
        // a new menu and replacing the old one with it, that problem seems to be avoided..
        NSMenu* menu = [[NSMenu alloc] initWithTitle: juceStringToNS (name)];

        for (PopupMenu::MenuItemIterator iter (menuToCopy); iter.next();)
            addMenuItem (iter, menu, menuId, topLevelIndex);

        [menu setAutoenablesItems: false];
        [menu update];
        [parentItem setSubmenu: menu];
        [menu release];
    }

    void updateTopLevelMenu (NSMenu* menu)
    {
        NSMenu* superMenu = [menu supermenu];
        auto menuNames = currentModel->getMenuBarNames();
        auto indexOfMenu = (int) [superMenu indexOfItemWithSubmenu: menu];
        [menu removeAllItems];
        auto updatedPopup = currentModel->getMenuForIndex (indexOfMenu, menuNames[indexOfMenu - 1]);

        for (PopupMenu::MenuItemIterator iter (updatedPopup); iter.next();)
            addMenuItem (iter, menu, 1, indexOfMenu);

        [menu update];
    }

    void menuBarItemsChanged (MenuBarModel*)
    {
        if (isOpen)
        {
            defferedUpdateRequested = true;
            return;
        }

        lastUpdateTime = Time::getMillisecondCounter();

        StringArray menuNames;
        if (currentModel != nullptr)
            menuNames = currentModel->getMenuBarNames();

        NSMenu* menuBar = [[NSApp mainMenu] retain];

        while ([menuBar numberOfItems] > 1 + menuNames.size())
            [menuBar removeItemAtIndex: [menuBar numberOfItems] - 1];

        int menuId = 1;

        for (int i = 0; i < menuNames.size(); ++i)
        {
            const PopupMenu menu (currentModel->getMenuForIndex (i, menuNames [i]));

            if (i >= [menuBar numberOfItems] - 1)
                addTopLevelMenu (menuBar, menu, menuNames[i], menuId, i);
            else
                updateTopLevelMenu ([menuBar itemAtIndex: 1 + i], menu, menuNames[i], menuId, i);
        }

        [menuBar release];
    }

    void menuCommandInvoked (MenuBarModel*, const ApplicationCommandTarget::InvocationInfo& info)
    {
        if ((info.commandFlags & ApplicationCommandInfo::dontTriggerVisualFeedback) == 0
              && info.invocationMethod != ApplicationCommandTarget::InvocationInfo::fromKeyPress)
            if (auto* item = findMenuItemWithTag ([NSApp mainMenu], info.commandID))
                flashMenuBar ([item menu]);
    }

    void invoke (int commandId, ApplicationCommandManager* commandManager, int topLevelIndex) const
    {
        if (currentModel != nullptr)
        {
            if (commandManager != nullptr)
            {
                ApplicationCommandTarget::InvocationInfo info (commandId);
                info.invocationMethod = ApplicationCommandTarget::InvocationInfo::fromMenu;

                commandManager->invoke (info, true);
            }

            MessageManager::callAsync ([=]()
            {
                if (instance != nullptr)
                    instance->invokeDirectly (commandId, topLevelIndex);
            });
        }
    }

    void invokeDirectly (int commandId, int topLevelIndex)
    {
        if (currentModel != nullptr)
            currentModel->menuItemSelected (commandId, topLevelIndex);
    }

    void addMenuItem (PopupMenu::MenuItemIterator& iter, NSMenu* menuToAddTo,
                      const int topLevelMenuId, const int topLevelIndex)
    {
        const PopupMenu::Item& i = iter.getItem();
        NSString* text = juceStringToNS (i.text);

        if (text == nil)
            text = nsEmptyString();

        if (i.isSeparator)
        {
            [menuToAddTo addItem: [NSMenuItem separatorItem]];
        }
        else if (i.isSectionHeader)
        {
            NSMenuItem* item = [menuToAddTo addItemWithTitle: text
                                                      action: nil
                                               keyEquivalent: nsEmptyString()];

            [item setEnabled: false];
        }
        else if (i.subMenu != nullptr)
        {
            if (i.text == recentItemsMenuName)
            {
                if (recent == nullptr)
                    recent = new RecentFilesMenuItem();

                if (recent->recentItem != nil)
                {
                    if (NSMenu* parent = [recent->recentItem menu])
                        [parent removeItem: recent->recentItem];

                    [menuToAddTo addItem: recent->recentItem];
                    return;
                }
            }

            NSMenuItem* item = [menuToAddTo addItemWithTitle: text
                                                      action: nil
                                               keyEquivalent: nsEmptyString()];

            [item setTag: i.itemID];
            [item setEnabled: i.isEnabled];

            NSMenu* sub = createMenu (*i.subMenu, i.text, topLevelMenuId, topLevelIndex, false);
            [menuToAddTo setSubmenu: sub forItem: item];
            [sub release];
        }
        else
        {
            NSMenuItem* item = [menuToAddTo addItemWithTitle: text
                                                      action: @selector (menuItemInvoked:)
                                               keyEquivalent: nsEmptyString()];

            [item setTag: i.itemID];
            [item setEnabled: i.isEnabled];
            [item setState: i.isTicked ? NSOnState : NSOffState];
            [item setTarget: (id) callback];

            NSMutableArray* info = [NSMutableArray arrayWithObject: [NSNumber numberWithUnsignedLongLong: (pointer_sized_uint) (void*) i.commandManager]];
            [info addObject: [NSNumber numberWithInt: topLevelIndex]];
            [item setRepresentedObject: info];

            if (i.commandManager != nullptr)
            {
                for (auto& kp : i.commandManager->getKeyMappings()->getKeyPressesAssignedToCommand (i.itemID))
                {
                    if (kp != KeyPress::backspaceKey   // (adding these is annoying because it flashes the menu bar
                         && kp != KeyPress::deleteKey) // every time you press the key while editing text)
                    {
                        juce_wchar key = kp.getTextCharacter();

                        if (key == 0)
                            key = (juce_wchar) kp.getKeyCode();

                        [item setKeyEquivalent: juceStringToNS (String::charToString (key).toLowerCase())];
                        [item setKeyEquivalentModifierMask: juceModsToNSMods (kp.getModifiers())];
                    }

                    break;
                }
            }
        }
    }

    NSMenu* createMenu (const PopupMenu menu,
                        const String& menuName,
                        const int topLevelMenuId,
                        const int topLevelIndex,
                        const bool addDelegate)
    {
        NSMenu* m = [[NSMenu alloc] initWithTitle: juceStringToNS (menuName)];

        [m setAutoenablesItems: false];

        if (addDelegate)
        {
           #if defined (MAC_OS_X_VERSION_10_6) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_6
            [m setDelegate: (id<NSMenuDelegate>) callback];
           #else
            [m setDelegate: callback];
           #endif
        }

        for (PopupMenu::MenuItemIterator iter (menu); iter.next();)
            addMenuItem (iter, m, topLevelMenuId, topLevelIndex);

        [m update];
        return m;
    }

    static JuceMainMenuHandler* instance;

    MenuBarModel* currentModel = nullptr;
    ScopedPointer<PopupMenu> extraAppleMenuItems;
    uint32 lastUpdateTime = 0;
    NSObject* callback = nil;
    String recentItemsMenuName;
    bool isOpen = false, defferedUpdateRequested = false;

private:
    struct RecentFilesMenuItem
    {
        RecentFilesMenuItem() : recentItem (nil)
        {
            if (NSNib* menuNib = [[[NSNib alloc] initWithNibNamed: @"RecentFilesMenuTemplate" bundle: nil] autorelease])
            {
                NSArray* array = nil;

               #if (! defined (MAC_OS_X_VERSION_10_8)) || MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_8
                [menuNib instantiateNibWithOwner: NSApp  topLevelObjects: &array];
               #else
                [menuNib instantiateWithOwner: NSApp  topLevelObjects: &array];
               #endif

                for (id object in array)
                {
                    if ([object isKindOfClass: [NSMenu class]])
                    {
                        if (NSArray* items = [object itemArray])
                        {
                            if (NSMenuItem* item = findRecentFilesItem (items))
                            {
                                recentItem = [item retain];
                                break;
                            }
                        }
                    }
                }
            }
        }

        ~RecentFilesMenuItem()
        {
            [recentItem release];
        }

        static NSMenuItem* findRecentFilesItem (NSArray* const items)
        {
            for (id object in items)
                if (NSArray* subMenuItems = [[object submenu] itemArray])
                    for (id subObject in subMenuItems)
                        if ([subObject isKindOfClass: [NSMenuItem class]])
                            return subObject;
            return nil;
        }

        NSMenuItem* recentItem;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RecentFilesMenuItem)
    };

    ScopedPointer<RecentFilesMenuItem> recent;

    //==============================================================================
    static NSMenuItem* findMenuItemWithTag (NSMenu* const menu, int tag)
    {
        for (NSInteger i = [menu numberOfItems]; --i >= 0;)
        {
            NSMenuItem* m = [menu itemAtIndex: i];
            if ([m tag] == tag)
                return m;

            if (NSMenu* sub = [m submenu])
                if (NSMenuItem* found = findMenuItemWithTag (sub, tag))
                    return found;
        }

        return nil;
    }

    static void flashMenuBar (NSMenu* menu)
    {
        if ([[menu title] isEqualToString: nsStringLiteral ("Apple")])
            return;

        [menu retain];

        const unichar f35Key = NSF35FunctionKey;
        NSString* f35String = [NSString stringWithCharacters: &f35Key length: 1];

        NSMenuItem* item = [[NSMenuItem alloc] initWithTitle: nsStringLiteral ("x")
                                                      action: nil
                                               keyEquivalent: f35String];
        [item setTarget: nil];
        [menu insertItem: item atIndex: [menu numberOfItems]];
        [item release];

        if ([menu indexOfItem: item] >= 0)
        {
            NSEvent* f35Event = [NSEvent keyEventWithType: NSEventTypeKeyDown
                                                 location: NSZeroPoint
                                            modifierFlags: NSEventModifierFlagCommand
                                                timestamp: 0
                                             windowNumber: 0
                                                  context: [NSGraphicsContext currentContext]
                                               characters: f35String
                              charactersIgnoringModifiers: f35String
                                                isARepeat: NO
                                                  keyCode: 0];

            [menu performKeyEquivalent: f35Event];

            if ([menu indexOfItem: item] >= 0)
                [menu removeItem: item]; // (this throws if the item isn't actually in the menu)
        }

        [menu release];
    }

    static unsigned int juceModsToNSMods (const ModifierKeys mods)
    {
        unsigned int m = 0;
        if (mods.isShiftDown())    m |= NSEventModifierFlagShift;
        if (mods.isCtrlDown())     m |= NSEventModifierFlagControl;
        if (mods.isAltDown())      m |= NSEventModifierFlagOption;
        if (mods.isCommandDown())  m |= NSEventModifierFlagCommand;
        return m;
    }

    //==============================================================================
    struct JuceMenuCallbackClass   : public ObjCClass<NSObject>
    {
        JuceMenuCallbackClass()  : ObjCClass<NSObject> ("JUCEMainMenu_")
        {
            addIvar<JuceMainMenuHandler*> ("owner");

            addMethod (@selector (menuItemInvoked:),  menuItemInvoked, "v@:@");
            addMethod (@selector (menuNeedsUpdate:),  menuNeedsUpdate, "v@:@");

           #if defined (MAC_OS_X_VERSION_10_6) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_6
            addProtocol (@protocol (NSMenuDelegate));
           #endif

            registerClass();
        }

        static void setOwner (id self, JuceMainMenuHandler* owner)
        {
            object_setInstanceVariable (self, "owner", owner);
        }

    private:
        static void menuItemInvoked (id self, SEL, NSMenuItem* item)
        {
            auto owner = getIvar<JuceMainMenuHandler*> (self, "owner");

            if ([[item representedObject] isKindOfClass: [NSArray class]])
            {
                // If the menu is being triggered by a keypress, the OS will have picked it up before we had a chance to offer it to
                // our own components, which may have wanted to intercept it. So, rather than dispatching directly, we'll feed it back
                // into the focused component and let it trigger the menu item indirectly.
                NSEvent* e = [NSApp currentEvent];

                if ([e type] == NSEventTypeKeyDown || [e type] == NSEventTypeKeyUp)
                {
                    if (auto* focused = juce::Component::getCurrentlyFocusedComponent())
                    {
                        if (auto peer = dynamic_cast<juce::NSViewComponentPeer*> (focused->getPeer()))
                        {
                            if ([e type] == NSEventTypeKeyDown)
                                peer->redirectKeyDown (e);
                            else
                                peer->redirectKeyUp (e);

                            return;
                        }
                    }
                }

                NSArray* info = (NSArray*) [item representedObject];

                owner->invoke ((int) [item tag],
                               (ApplicationCommandManager*) (pointer_sized_int)
                                    [((NSNumber*) [info objectAtIndex: 0]) unsignedLongLongValue],
                               (int) [((NSNumber*) [info objectAtIndex: 1]) intValue]);
            }
        }

        static void menuNeedsUpdate (id self, SEL, NSMenu* menu)
        {
            getIvar<JuceMainMenuHandler*> (self, "owner")->updateTopLevelMenu (menu);
        }
    };
};

JuceMainMenuHandler* JuceMainMenuHandler::instance = nullptr;

//==============================================================================
class TemporaryMainMenuWithStandardCommands
{
public:
    TemporaryMainMenuWithStandardCommands()
        : oldMenu (MenuBarModel::getMacMainMenu())
    {
        if (auto* appleMenu = MenuBarModel::getMacExtraAppleItemsMenu())
            oldAppleMenu = new PopupMenu (*appleMenu);

        if (auto* handler = JuceMainMenuHandler::instance)
            oldRecentItems = handler->recentItemsMenuName;

        MenuBarModel::setMacMainMenu (nullptr);

        NSMenu* menu = [[NSMenu alloc] initWithTitle: nsStringLiteral ("Edit")];
        NSMenuItem* item;

        item = [[NSMenuItem alloc] initWithTitle: NSLocalizedString (nsStringLiteral ("Cut"), nil)
                                          action: @selector (cut:)  keyEquivalent: nsStringLiteral ("x")];
        [menu addItem: item];
        [item release];

        item = [[NSMenuItem alloc] initWithTitle: NSLocalizedString (nsStringLiteral ("Copy"), nil)
                                          action: @selector (copy:)  keyEquivalent: nsStringLiteral ("c")];
        [menu addItem: item];
        [item release];

        item = [[NSMenuItem alloc] initWithTitle: NSLocalizedString (nsStringLiteral ("Paste"), nil)
                                          action: @selector (paste:)  keyEquivalent: nsStringLiteral ("v")];
        [menu addItem: item];
        [item release];

        item = [[NSApp mainMenu] addItemWithTitle: NSLocalizedString (nsStringLiteral ("Edit"), nil)
                                          action: nil  keyEquivalent: nsEmptyString()];
        [[NSApp mainMenu] setSubmenu: menu forItem: item];
        [menu release];

        // use a dummy modal component so that apps can tell that something is currently modal.
        dummyModalComponent.enterModalState (false);
    }

    ~TemporaryMainMenuWithStandardCommands()
    {
        MenuBarModel::setMacMainMenu (oldMenu, oldAppleMenu, oldRecentItems);
    }

private:
    MenuBarModel* const oldMenu;
    ScopedPointer<PopupMenu> oldAppleMenu;
    String oldRecentItems;

    // The OS view already plays an alert when clicking outside
    // the modal comp, so this override avoids adding extra
    // inappropriate noises when the cancel button is pressed.
    // This override is also important because it stops the base class
    // calling ModalComponentManager::bringToFront, which can get
    // recursive when file dialogs are involved
    struct SilentDummyModalComp  : public Component
    {
        SilentDummyModalComp() {}
        void inputAttemptWhenModal() override {}
    };

    SilentDummyModalComp dummyModalComponent;
};

//==============================================================================
namespace MainMenuHelpers
{
    static NSString* translateMenuName (const String& name)
    {
        return NSLocalizedString (juceStringToNS (TRANS (name)), nil);
    }

    static NSMenuItem* createMenuItem (NSMenu* menu, const String& name, SEL sel, NSString* key)
    {
        NSMenuItem* item = [[[NSMenuItem alloc] initWithTitle: translateMenuName (name)
                                                       action: sel
                                                keyEquivalent: key] autorelease];
        [item setTarget: NSApp];
        [menu addItem: item];
        return item;
    }

    static void createStandardAppMenu (NSMenu* menu, const String& appName, const PopupMenu* extraItems)
    {
        if (extraItems != nullptr && JuceMainMenuHandler::instance != nullptr && extraItems->getNumItems() > 0)
        {
            for (PopupMenu::MenuItemIterator iter (*extraItems); iter.next();)
                JuceMainMenuHandler::instance->addMenuItem (iter, menu, 0, -1);

            [menu addItem: [NSMenuItem separatorItem]];
        }

        // Services...
        NSMenuItem* services = [[[NSMenuItem alloc] initWithTitle: translateMenuName ("Services")
                                                           action: nil  keyEquivalent: nsEmptyString()] autorelease];
        [menu addItem: services];

        NSMenu* servicesMenu = [[[NSMenu alloc] initWithTitle: translateMenuName ("Services")] autorelease];
        [menu setSubmenu: servicesMenu forItem: services];
        [NSApp setServicesMenu: servicesMenu];
        [menu addItem: [NSMenuItem separatorItem]];

        createMenuItem (menu, TRANS("Hide") + String (" ") + appName, @selector (hide:), nsStringLiteral ("h"));

        [createMenuItem (menu, TRANS("Hide Others"), @selector (hideOtherApplications:), nsStringLiteral ("h"))
            setKeyEquivalentModifierMask: NSEventModifierFlagCommand | NSEventModifierFlagOption];

        createMenuItem (menu, TRANS("Show All"), @selector (unhideAllApplications:), nsEmptyString());

        [menu addItem: [NSMenuItem separatorItem]];

        createMenuItem (menu, TRANS("Quit") + String (" ") + appName, @selector (terminate:), nsStringLiteral ("q"));
    }

    // Since our app has no NIB, this initialises a standard app menu...
    static void rebuildMainMenu (const PopupMenu* extraItems)
    {
        // this can't be used in a plugin!
        jassert (JUCEApplicationBase::isStandaloneApp());

        if (auto* app = JUCEApplicationBase::getInstance())
        {
            JUCE_AUTORELEASEPOOL
            {
                NSMenu* mainMenu = [[NSMenu alloc] initWithTitle: nsStringLiteral ("MainMenu")];
                NSMenuItem* item = [mainMenu addItemWithTitle: nsStringLiteral ("Apple")
                                                       action: nil
                                                keyEquivalent: nsEmptyString()];

                NSMenu* appMenu = [[NSMenu alloc] initWithTitle: nsStringLiteral ("Apple")];

                [NSApp performSelector: @selector (setAppleMenu:) withObject: appMenu];
                [mainMenu setSubmenu: appMenu forItem: item];

                [NSApp setMainMenu: mainMenu];
                MainMenuHelpers::createStandardAppMenu (appMenu, app->getApplicationName(), extraItems);

                [appMenu release];
                [mainMenu release];
            }
        }
    }
}

void MenuBarModel::setMacMainMenu (MenuBarModel* newMenuBarModel,
                                   const PopupMenu* extraAppleMenuItems,
                                   const String& recentItemsMenuName)
{
    if (getMacMainMenu() != newMenuBarModel)
    {
        JUCE_AUTORELEASEPOOL
        {
            if (newMenuBarModel == nullptr)
            {
                delete JuceMainMenuHandler::instance;
                jassert (JuceMainMenuHandler::instance == nullptr); // should be zeroed in the destructor
                jassert (extraAppleMenuItems == nullptr); // you can't specify some extra items without also supplying a model

                extraAppleMenuItems = nullptr;
            }
            else
            {
                if (JuceMainMenuHandler::instance == nullptr)
                    JuceMainMenuHandler::instance = new JuceMainMenuHandler();

                JuceMainMenuHandler::instance->setMenu (newMenuBarModel, extraAppleMenuItems, recentItemsMenuName);
            }
        }
    }

    MainMenuHelpers::rebuildMainMenu (extraAppleMenuItems);

    if (newMenuBarModel != nullptr)
        newMenuBarModel->menuItemsChanged();
}

MenuBarModel* MenuBarModel::getMacMainMenu()
{
    if (auto* mm = JuceMainMenuHandler::instance)
        return mm->currentModel;

    return nullptr;
}

const PopupMenu* MenuBarModel::getMacExtraAppleItemsMenu()
{
    if (auto* mm = JuceMainMenuHandler::instance)
        return mm->extraAppleMenuItems.get();

    return nullptr;
}

typedef void (*MenuTrackingChangedCallback) (bool);
extern MenuTrackingChangedCallback menuTrackingChangedCallback;

static void mainMenuTrackingChanged (bool isTracking)
{
    PopupMenu::dismissAllActiveMenus();

    if (auto* menuHandler = JuceMainMenuHandler::instance)
    {
        menuHandler->isOpen = isTracking;

        if (auto* model = menuHandler->currentModel)
            model->handleMenuBarActivate (isTracking);

        if (menuHandler->defferedUpdateRequested && ! isTracking)
        {
            menuHandler->defferedUpdateRequested = false;
            menuHandler->menuBarItemsChanged (menuHandler->currentModel);
        }
    }
}

void juce_initialiseMacMainMenu()
{
    menuTrackingChangedCallback = mainMenuTrackingChanged;

    if (JuceMainMenuHandler::instance == nullptr)
        MainMenuHelpers::rebuildMainMenu (nullptr);
}

// (used from other modules that need to create an NSMenu)
NSMenu* createNSMenu (const PopupMenu& menu, const String& name,
                      int topLevelMenuId, int topLevelIndex, bool addDelegate)
{
    juce_initialiseMacMainMenu();

    if (auto* mm = JuceMainMenuHandler::instance)
        return mm->createMenu (menu, name, topLevelMenuId, topLevelIndex, addDelegate);

    jassertfalse; // calling this before making sure the OSX main menu stuff was initialised?
    return nil;
}