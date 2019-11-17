#ifndef __AxPluginMaiBox_H__
#define __AxPluginMaiBox_H__
//=============================================================================
/*
 * See the file README in the main directory for a description of
 * this software, copyright information, and how to reach the author.
 *
 * Author: alex
 * Date:   15.11.2003
 *
 * Last modfied:
 *   $Author: alex $
 *   $Date: 2013-03-20 20:33:18 +0100 (Mi, 20 Mär 2013) $
 */
//=============================================================================

//=============================================================================
//     includes
//=============================================================================

//----- qt --------------------------------------------------------------------
//----- CORBA -----------------------------------------------------------------
//----- C++ -------------------------------------------------------------------
//----- C ---------------------------------------------------------------------
//----- AxLib -----------------------------------------------------------------
//----- vdr -------------------------------------------------------------------
#include <vdr/plugin.h>

//----- local -----------------------------------------------------------------
#include "AxPluginSetup.h"
#include "AxMailChecker.h"

//-------------------------------------------------------------------------
//     typedefs
//-------------------------------------------------------------------------

//=============================================================================
//     forward declarations
//=============================================================================
class AxPluginMailBox;
namespace Ax   {
namespace Mail {
  class MailBox;
}
}

//=============================================================================
//     defines
//=============================================================================
#define MAIN_MENU_ENTRY_LEN 80

//=============================================================================
//     class AxPluginMailBox
//=============================================================================
/** Main class of the mail plugin, description copied from $VDRDIR/PLUGINS.html
 */
class AxPluginMailBox : public cPlugin
{
  public:
    //-------------------------------------------------------------------------
    //     AxPluginMailBox()
    //-------------------------------------------------------------------------
    /** Default constructor which is called by the factory VDRPLUGINCREATOR
     *
     *  Don't do any "heavyweight" things here
     *  - don't access vdr-structurs
     *  - don't create threads
     *  - don't do long running tasks
     *  instead the method start() should be used.
     */
    AxPluginMailBox(void);

    //-------------------------------------------------------------------------
    //     ~AxPluginMailBox()
    //-------------------------------------------------------------------------
    /** Destructor
     *
     *  Stop threads and free memory
     */
    virtual ~AxPluginMailBox();

    //-------------------------------------------------------------------------
    //     Version()
    //-------------------------------------------------------------------------
    /** Returns the version string for this release.
     *
     *  @returns a version string
     */
    virtual const char *Version(void);

    //-------------------------------------------------------------------------
    //     Description()
    //-------------------------------------------------------------------------
    /** Returns a short, one line description of the plugin
     *
     *  @returns a short description of the plugin
     */
    virtual const char *Description(void);

    //-------------------------------------------------------------------------
    //     CommandLineHelp()
    //-------------------------------------------------------------------------
    /** Returns a string containig help on command line args for this plugin
     *
     *  If a plugin accepts command line options, it should implement the
     *  function CommandLineHelp() which will be called if the user enters
     *  the -h option when starting VDR. The returned string should contain
     *  the command line help for this plugin, formatted in the same way as
     *  done by VDR itself:
     *
     *  \code
          const char *cPluginHello::CommandLineHelp(void)
          {
            // Return a string that describes all known command line options.
            return "  -a ABC,   --aaa=ABC      do something nice with ABC\n"
                   "  -b,       --bbb          activate 'plan B'\n";
          }
        \endcode
     *
     *   This command line help will be printed directly below VDR's help
     *   texts (separated by a line indicating the plugin's name, version
     *   and description), so if you use the same formatting as shown here
     *   it will line up nicely. Note that all lines should be terminated
     *   with a newline character, and should be shorter than 80 characters.
     *
     *  @returns a string containing help for plugin-arguments
     */
    virtual const char *CommandLineHelp(void);

    //-------------------------------------------------------------------------
    //     ProcessArgs()
    //-------------------------------------------------------------------------
    /** Parse and store command line args
     *
     *  A VDR plugin can have command line arguments just like any normal program.
     *  If a plugin wants to react on command line arguments, it needs to
     *  implement the function ProcessArgs().
     *
     *  The parameters argc and argv have exactly the same meaning as in a
     *  normal C program's main() function. argv[0] contains the name of the
     *  plugin (as given in the -P option of the vdr call).
     *
     *  Each plugin has its own set of command line options, which are totally
     *  independent from those of any other plugin or VDR itself.
     *
     *  You can use the getopt() or getopt_long() function to process these
     *  arguments. As with any normal C program, the strings pointed to by argv
     *  will survive the entire lifetime of the plugin, so it is safe to store
     *  pointers to these values inside the plugin. Here's an example:
     *
     *  \code
          bool cPluginHello::ProcessArgs(int argc, char *argv[])
          {
            // Implement command line argument processing here if applicable.
            static struct option long_options[] = {
                 { "aaa",      required_argument, NULL, 'a' },
                 { "bbb",      no_argument,       NULL, 'b' },
                 { NULL }
               };

            int c;
            while ((c = getopt_long(argc, argv, "a:b", long_options, NULL)) != -1) {
                  switch (c) {
                    case 'a': option_a = optarg;
                              break;
                    case 'b': option_b = true;
                              break;
                    default:  return false;
                    }
                  }
            return true;
          }
        \endcode
     *
     *  The return value must be true if all options have been processed
     *  correctly, or false in case of an error. The first plugin that returns
     *  false from a call to its ProcessArgs() function will cause VDR to exit.
     *
     *  @param argc number of elements in argv
     *  @param argv the command line parameters, argv[0] is the name of the plugin
     *
     *  @returns true, if all options have been processed correctly, or false
     *           in case of an error
     */
    virtual bool ProcessArgs(int argc, char *argv[]);

    //-------------------------------------------------------------------------
    //     Start()
    //-------------------------------------------------------------------------
    /** Prepare the plugin to do the real work.
     *
     *  If a plugin implements a function that runs in the background
     *  (presumably in a thread of its own), or wants to make use of
     *  {internationalization}, it needs to implement the function Start().
     *  which is called once for each plugin at program startup. Inside this
     *  function the plugin must set up everything necessary to perform its task.
     *  This may, for instance, be a thread that collects data from the DVB
     *  stream, which is later presented to the user via a function that is
     *  available from the main menu.
     *
     *  A return value of false indicates that something has gone wrong and the
     *  plugin will not be able to perform its task. In that case, the plugin
     *  should write a proper error message to the log file. The first plugin
     *  that returns false from its Start() function will cause VDR to exit.
     *
     *  @returns true if started correctly, otherwise false (which causes vdr to exit)
     */
    virtual bool Start(void);

    //-------------------------------------------------------------------------
    //     Stop()
    //-------------------------------------------------------------------------
    /** Stop any background operations
     *
     * If a plugin performs any background tasks, it shall implement the function
     *
     *   virtual void Stop(void);
     *
     * in which it shall stop them.
     *
     * The Stop() function will only be called if a previous call to the Start()
     * function of that plugin has returned true. The Stop() functions are called
     * in the reverse order as the Start() functions were called.
     */
    virtual void Stop(void);

    //-------------------------------------------------------------------------
    //     MainMenuEntry()
    //-------------------------------------------------------------------------
    /** Returns the text for the main menu entry.
     *
     *  If the plugin implements a feature that the user shall be able to
     *  access from VDR's main menu, it needs to implement the function
     *  MainMenuEntry().
     *
     *  The default implementation returns a NULL pointer, which means that
     *  this plugin will not have an item in the main menu. Here's an example
     *  of a plugin that will have a main menu item:
     *  \code
          static const char *MAINMENUENTRY = "Hello";

          const char *cPluginHello::MainMenuEntry(void)
          {
            return tr(MAINMENUENTRY);
          }
        \endcode
     *
     *  The menu entries of all plugins will be inserted into VDR's main
     *  menu right after the Recordings item, in the same sequence as
     *  they were given in the call to VDR.
     *
     *  @returns the text for the entry in the main menu or NULL if
     *           no entry desired.
     */
    virtual const char *MainMenuEntry(void);

    //-------------------------------------------------------------------------
    //     MainMenuAction()
    //-------------------------------------------------------------------------
    /** This method gets called whenever the user selects the entry
     *  of this plugin from the main menu.
     *
     *  If the user selects the main menu entry of a plugin, VDR calls the
     *  function MainMenuAction() which can do one of two things:
     *  - Return a pointer to a cOsdMenu object which will be displayed as a
     *    submenu of the main menu (just like the Recordings menu, for instance).
     *    That menu can then implement further functionality and, for instance,
     *    could eventually start a custom player to replay a file other than a
     *    VDR recording.
     *  - Perform a specific action and return NULL. In that case the main menu
     *    will be closed after calling MainMenuAction().
     *
     *  It is very important that a call to MainMenuAction() returns as soon
     *  as possible! As long as the program stays inside this function, no other
     *  user interaction is possible. If a specific action takes longer than a
     *  few seconds, the plugin should launch a separate thread to do this.
     *
     *  @returns a SubMenu or NULL
     */
    virtual cOsdMenu *MainMenuAction(void);

    //-------------------------------------------------------------------------
    //     MainMenuActionFinished()
    //-------------------------------------------------------------------------
    /** Informs the plugin that the action in the main-menu is finished
     *  either by vdr/kMenu or by the user by pressing kOK.
     *
     *  This method is called in the destructor of the main OsdMenu.
     */
    void MainMenuActionFinished();

    //-------------------------------------------------------------------------
    //     SetupMenuActionFinished()
    //-------------------------------------------------------------------------
    /** Informs the plugin that the action in the setup-menu is finished
     *  either by vdr/kMenu or by the user by pressing kOK.
     *
     *  This method is called in the destructor of the setup OsdMenu.
     */
    void SetupMenuActionFinished();

    //-------------------------------------------------------------------------
    //     Housekeeping()
    //-------------------------------------------------------------------------
    /** Allows the plugin to do some (short running) regular tasks.
     *
     *  From time to time a plugin may want to do some regular tasks, like
     *  cleaning up some files or other things. In order to do this it can
     *  implement the function Housekeeping() which gets called when VDR is
     *  otherwise idle. The intervals between subsequent calls to this
     *  function are not defined. There may be several hours between two
     *  calls (if, for instance, there are recordings or replays going on) or
     *  they may be as close as ten seconds. The only thing that is guaranteed
     *  is that there are at least ten seconds between two subsequent calls to
     *  the Housekeeping() function of the same plugin.
     *
     *  It is very important that a call to Housekeeping() returns as soon as
     *  possible! As long as the program stays inside this function, no other
     *  user interaction is possible. If a specific action takes longer than a
     *  few seconds, the plugin should launch a separate thread to do this.
     */
    virtual void Housekeeping(void);

    //-------------------------------------------------------------------------
    //     SetupMenu()
    //-------------------------------------------------------------------------
    /** Returns the plugin's setup menu page which is derived from cMenuSetupPage.
     *
     *  The SetupMenu() function shall return the plugin's setup menu page,
     *  where the user can adjust all the parameters known to this plugin.
     *
     *  For further explanation \see SetupParse().
     *
     *  @returns the plugin's setup menu page
     */
    virtual cMenuSetupPage *SetupMenu(void);

    //-------------------------------------------------------------------------
    //     SetupParse()
    //-------------------------------------------------------------------------
    /** SetupParse() will be called for each parameter the plugin has
     *  previously stored in the global setup data (see below).
     *
     *  It shall return true if the parameter was parsed correctly, false in
     *  case of an error. If false is returned, an error message will be
     *  written to the log file (and program execution will continue).
     *  A possible implementation of SetupParse() could look like this:
     *  \code
          bool cPluginHello::SetupParse(const char *Name, const char *Value)
          {
            // Parse your own setup parameters and store their values.
            if      (!strcasecmp(Name, "GreetingTime"))         GreetingTime = atoi(Value);
            else if (!strcasecmp(Name, "UseAlternateGreeting")) UseAlternateGreeting = atoi(Value);
            else
               return false;
            return true;
          }
        \endcode
     *
     *  It is important to make sure that the parameter names are exactly
     *  the same as used in the {Setup menu}'s Store() function.
     *
     *  The plugin's setup parameters are stored in the same file as VDR's
     *  parameters. In order to allow each plugin (and VDR itself) to have
     *  its own set of parameters, the Name of each parameter will be
     *  preceeded with the plugin's name, as in hello.GreetingTime = 3
     *
     *  The prefix will be handled by the core VDR setup code, so the
     *  individual plugins need not worry about this.
     *
     *  To store its values in the global setup, a plugin has to call the
     *  function SetupStore() where Name is the name of the
     *  parameter ("GreetingTime" in the above example, without the
     *  prefix "hello.") and Value is a simple data type (like char *,
     *  int etc). Note that this is not a function that the individual
     *  plugin class needs to implement! SetupStore() is a non-virtual
     *  member function of the cPlugin class.
     *
     *  To remove a parameter from the setup data, call SetupStore()
     *  with the appropriate name and without any value, as in
     *  SetupStore("GreetingTime");
     *
     *  The VDR menu "Setup/Plugins" will list all loaded plugins with
     *  their name, version number and description. Selecting an item
     *  in this list will bring up the plugin's "Setup" menu if that
     *  plugin has implemented the SetupMenu() function.
     *
     *  Finally, a plugin doesn't have to implement the SetupMenu()
     *  if it only needs setup parameters that are not directly user
     *  adjustable. It can use SetupStore() and SetupParse()
     *  without presenting these parameters to the user.
     *
     *  @param Name  is the name of the configuration entry
     *  @param Value is the value of the configuration entry or NULL if
     *               the entry should get deleted.
     */
    virtual bool SetupParse(const char *Name, const char *Value);

    //-------------------------------------------------------------------------
    //     Service()
    //-------------------------------------------------------------------------
    /** In some situations, two plugins may want to communicate directly,
     *  talking about things that VDR doesn't handle itself. For example, a
     *  plugin may want to use features that some other plugin offers, or it
     *  may want to inform other plugins about important things it does.
     *  To receive requests or messages, a plugin can implement the following
     *  function:
     \code
       virtual bool Service(const char *Id, void *Data = NULL);
     \endcode
     *  Id is a unique identification string that identifies the service protocol.
     *  To avoid collisions, the string should contain a service name, the plugin
     *  name (unless the service is not related to a single plugin) and a protocol
     *  version number. Data points to a custom data structure. For each id string
     *  there should be a specification that describes the format of the data
     *  structure, and any change to the format should be reflected by a change of
     *  the id string.
     *
     *  The function shall return true for any service id string it handles, and
     *  false otherwise. The plugins have to agreee in which situations the service
     *  may be called, for example whether the service may be called from every
     *  thread, or just from the main thread. A possible implementation could look
     *  like this:
     \code
       struct Hello_SetGreetingTime_v1_0 {
         int NewGreetingTime;
       };

       bool cPluginHello::Service(const char *Id, void *Data)
       {
         if (strcmp(Id, "Hello-SetGreetingTime-v1.0") == 0) {
            if (Data == NULL)
               return true;
            GreetingTime = ((Hello_SetGreetingTime_v1_0*)Data)->NewGreetingTime;
            return true;
            }
         return false;
       }
     \endcode
     *
     *  Plugins should expect to be called with Data set to NULL and may use this
     *  as a 'service supported' check without performing any actions.
     *
     *  To send messages to, or request services from a specific plugin, one
     *  plugin can directly call another plugin's service function:
     \code
       Hello_SetGreetingTime_v1_0 hellodata;
       hellodata.NewGreetingTime = 3;
       cPlugin *Plugin = cPluginManager::GetPlugin("hello");
       if (Plugin)
          Plugin->Service("Hello-SetGreetingTime-v1.0", >hellodata);
     \endcode
     *
     *  To send messages to, or request services from some plugin that offers
     *  the protocol, a plugin can call the function \c cPluginManager::CallFirstService().
     *  This function will send the request to all plugins until one plugin
     *  handles it. The function returns a pointer to the plugin that handled
     *  the request, or \c NULL if no plugin handled it.
     *
     *  To send a message to all plugins, a plugin can call the function
     *  \c cPluginManager::CallAllServices(). This function returns true if any
     *  plugin handled the request, or false if no plugin handled the request.
     */
    virtual bool Service(const char *Id, void *Data = NULL);

#if 0
    //-------------------------------------------------------------------------
    //     MainThreadHook()
    //-------------------------------------------------------------------------
    virtual void MainThreadHook(void);

    //-------------------------------------------------------------------------
    //     Active()
    //-------------------------------------------------------------------------
    virtual cString Active(void);
#endif

    //-------------------------------------------------------------------------
    //     getSetup()
    //-------------------------------------------------------------------------
    AxPluginSetup &getSetup() { return mySetup; }

    //-------------------------------------------------------------------------
    //     getMailBoxCltn()
    //-------------------------------------------------------------------------
    Ax::Mail::MailBoxList &getMailBoxCltn() { return myMailBoxCltn; }

    //-------------------------------------------------------------------------
    //     setMainMenuAppendix()
    //-------------------------------------------------------------------------
    /** copies (protected my mutex) the given string to myMainMenuEntryBuffer
     */
    void setMainMenuAppendix(unsigned long theTotalNewMails = 0, const std::string &theMenuText = std::string(""));

    //-------------------------------------------------------------------------
    //     copyMainMenuEntry()
    //-------------------------------------------------------------------------
    /** copies (protected my mutex) the contents of myMainMenuEntryBuffer
     *  to myMainMenuEntry
     */
    void copyMainMenuEntry();

    //-------------------------------------------------------------------------
    //     setInitialMailBox()
    //-------------------------------------------------------------------------
    void setInitialMailBox(Ax::Mail::MailBox *theMailBox);

    //-------------------------------------------------------------------------
    //     getInitialMailBox()
    //-------------------------------------------------------------------------
    Ax::Mail::MailBox *getInitialMailBox();

  private:
    //-------------------------------------------------------------------------
    //     createMailBoxes()
    //-------------------------------------------------------------------------
    void createMailBoxes();

    //-------------------------------------------------------------------------
    //     destroyMailBoxes()
    //-------------------------------------------------------------------------
    void destroyMailBoxes();

    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    /** Settings of the plugin
     */
    AxPluginSetup mySetup;

    /** Collection of MailBox-instances to work with
     */
    Ax::Mail::MailBoxList myMailBoxCltn;

    /** mail-checker-thread
     */
    AxMailChecker myMailChecker;

    /** MailBox to display directly when calling MainMenuAction
     */
    Ax::Mail::MailBox *myInitialMailBox;
    cMutex             myInitialMailBoxMutex;

    /** main-menu-entry, buffer and mutex
     */
    char          myMainMenuEntry[MAIN_MENU_ENTRY_LEN];
    unsigned long myTotalNewMails;
    std::string   myMainMenuEntryAppendix;
    cMutex        myMainMenuEntryAppendixMutex;

}; // class AxPluginMailBox

#endif // __AxPluginMaiBox_H__

