/****************************************************************************
 * DESCRIPTION:
 *             Submenu
 *
 * $Id: vdr-1.3.44-Setup-0.3.0.diff,v 1.1 2006/03/04 09:58:47 ralf Exp $
 *
 * Contact:    ranga@teddycats.de
 *
 * Copyright (C) 2004, 2005 by Ralf Dotzert
 *
 * modified for the VDR Extensions Patch by zulu @vdr-portal
 ****************************************************************************/

#ifndef SUBMENU_H
#include <tinyxml.h>
#include "submenu.h"
#include "plugin.h"
#include "iconpatch.h"
static const char* TAG_SYSTEM      = "system";
static const char* TAG_PLUGIN      = "plugin";
static const char* TAG_COMMAND     = "command";
static const char* TAG_THREAD      = "thread";
static const char* TAG_MENU        = "menu";
static const char* TAG_UNDEFINED   = "undefined";
static const char* TRUE_STR        = "yes";


//################################################################################
//# SubMenuNode
//################################################################################

cSubMenuNode::cSubMenuNode(TiXmlElement *xml, int level,  cSubMenuNodes *currentMenu, cSubMenuNodes *parentMenu)
{
  init();
  _parentMenu  = parentMenu;
  _currentMenu = currentMenu;
  _level       = level;

  if (xml != NULL && xml->Type() == TiXmlNode::TINYXML_ELEMENT) {
     const char *tag = xml->Value();

     if (cSubMenuNode::IsType(tag) != cSubMenuNode::UNDEFINED) {
        SetType(tag);
        SetName(xml->Attribute("name"));
        if ((_type == COMMAND) || (_type == THREAD)) {
           SetCommand(xml->Attribute("execute"));
           const char *confirmStr = xml->Attribute("confirm");
           if (confirmStr != NULL && strcmp(confirmStr, TRUE_STR) == 0)
              _commandConfirm = true;
           }
        else if (_type == CALL_PLUGIN) { // Add Plugin Index
           SetCustomTitle(xml->Attribute("title"));
           SetPlugin();
           }
        else if (_type == MENU && xml->NoChildren() == false) {
           xml = xml->FirstChildElement();
           do {
              cSubMenuNode *node = new cSubMenuNode(xml, level+1, &_subMenus, currentMenu);
              _subMenus.Add(node);
              } while ((xml=xml->NextSiblingElement()) != NULL);
           }
        }
     }
  else
     throw "Invalid XML Node";
}

/**
 * Construct new Node empty Node
 *
 *
 */
cSubMenuNode::cSubMenuNode(cSubMenuNodes *currentMenu, cSubMenuNodes *parentMenu)
{
  init();
  _parentMenu  = parentMenu;
  _currentMenu = currentMenu;

}


/**
 *
 */
void cSubMenuNode::init()
{
  _name                = NULL;
  _command             = NULL;
  _title               = NULL;
  _pluginMainMenuEntry = NULL;
  _type                = UNDEFINED;
  _level               = 0;
  _parentMenu          = NULL;
  _currentMenu         = NULL;
  _pluginIndex         = 0;
  _commandConfirm      = false;
}


cSubMenuNode::~ cSubMenuNode()
{
  if (_name != NULL)
     free((void*)_name);
  if (_command != NULL)
     free((void*)_command);
  if (_title != NULL)
     free((void*)_title);
  if (_pluginMainMenuEntry != NULL)
     free((void*)_pluginMainMenuEntry);
}

/**
 *
 */
void cSubMenuNode::SetPlugin()
{
  bool found = false;
  for (int i = 0; ; i++) {
      cPlugin *p = cPluginManager::GetPlugin(i);
      if (p) {
         if (strcmp(_name, p->Name()) == 0 && p->MainMenuEntry() != NULL) {
            SetPluginMainMenuEntry(p->MainMenuEntry());
            _pluginIndex = i;
            found = true;
            break;
            }
         }
      else
         break;
      }

      if (!found)
         _type = UNDEFINED;
}


bool cSubMenuNode::SaveXml(TiXmlElement *root)
{
  bool ok = true;

  if (root!=NULL) {
     TiXmlElement *e = NULL;
     switch(_type) {
        case SYSTEM:
           e = new TiXmlElement(TAG_SYSTEM);
           e->SetAttribute("name", GetName());
           break;
        case COMMAND:
           e = new TiXmlElement(TAG_COMMAND);
           e->SetAttribute("name", GetName());
           e->SetAttribute("execute", GetCommand());
           if (_commandConfirm)
              e->SetAttribute("confirm", TRUE_STR);
           break;
        case THREAD:
           e = new TiXmlElement(TAG_THREAD);
           e->SetAttribute("name", GetName());
           e->SetAttribute("execute", GetCommand());
           if (_commandConfirm)
              e->SetAttribute("confirm", TRUE_STR);
           break;
        case CALL_PLUGIN:
           e = new TiXmlElement(TAG_PLUGIN);
           e->SetAttribute("name", GetName());
           if (GetCustomTitle() != NULL && strcmp(GetCustomTitle(), "") != 0)
              e->SetAttribute("title", GetCustomTitle());
           break;
        case MENU:
           e = new TiXmlElement(TAG_MENU);
           e->SetAttribute("name", GetName());
           break;
        case UNDEFINED:
        default:
           ok = false;
           break;
        }
        if (ok) {
           root->LinkEndChild(e);
           if (HasSubMenus())
              for (cSubMenuNode *node = _subMenus.First(); node; node = _subMenus.Next(node))
                  node->SaveXml(e);
           }
     }

  return(ok);
}


cSubMenuNode::Type cSubMenuNode::IsType(const char *name)
{
  Type type = UNDEFINED;

  if (strcmp(name ,TAG_SYSTEM) == 0)
     type = cSubMenuNode::SYSTEM;
  else if (strcmp(name ,TAG_PLUGIN) == 0)
     type = cSubMenuNode::CALL_PLUGIN;
  else if (strcmp(name ,TAG_COMMAND) == 0)
     type = cSubMenuNode::COMMAND;
  else if (strcmp(name ,TAG_THREAD) == 0)
     type = cSubMenuNode::THREAD;
  else if (strcmp(name ,TAG_MENU) == 0)
     type = cSubMenuNode::MENU;

  return(type);
}

void cSubMenuNode::SetType(const char *name)
{
   _type = IsType(name);
}

void cSubMenuNode::SetType(enum Type type)
{
  _type = type;
}


cSubMenuNode::Type cSubMenuNode::GetType()
{
  return(_type);
}

const char *cSubMenuNode::GetTypeAsString()
{
  const char *str=NULL;
  switch(_type) {
     case SYSTEM:
        str = TAG_SYSTEM;
        break;
     case COMMAND:
        str = TAG_COMMAND;
        break;
     case THREAD:
        str = TAG_THREAD;
        break;
     case CALL_PLUGIN:
        str = TAG_PLUGIN;
        break;
     case MENU:
        str = TAG_MENU;
        break;
     case UNDEFINED:
        str = TAG_UNDEFINED;
     default:
        break;
    }

  return(str);
}

void cSubMenuNode::SetCommand(const char *command)
{
  if (_command != NULL)
     free((void*)_command);

  if (command != NULL)
     _command = strdup(command);
  else
     _command = NULL;
}

const char *cSubMenuNode::GetCommand()
{
  return(_command);
}

bool cSubMenuNode::CommandConfirm()
{
  return(_commandConfirm);
}

void cSubMenuNode::SetCommandConfirm(int val)
{
  if (val == 1)
     _commandConfirm = true;
  else
     _commandConfirm = false;
}

void cSubMenuNode::SetCustomTitle(const char *title)
{
  if (_title != NULL)
     free((void*)_title);

  if (title != NULL)
     _title = strdup(title);
  else
     _title = NULL;
}

const char *cSubMenuNode::GetCustomTitle()
{
  return(_title);
}

void cSubMenuNode::SetName(const char *name)
{
  if (_name)
     free ((void*)_name);

  if (name != NULL)
     _name = strdup(name);
  else
     _name = NULL;
}

const char *cSubMenuNode::GetName()
{
  return(_name);
}

int cSubMenuNode::GetLevel()
{
  return(_level);
}

void cSubMenuNode::SetLevel(int level)
{
  _level = level;
  if (HasSubMenus()) { //Adjust Levels of Subnodes
     for (cSubMenuNode *node = _subMenus.First(); node; node = _subMenus.Next(node))
         node->SetLevel(level+1);
     }
}

int cSubMenuNode::GetPluginIndex()
{
  return(_pluginIndex);
}

void cSubMenuNode::SetPluginIndex(int index)
{
  _pluginIndex = index;
}

void cSubMenuNode::SetPluginMainMenuEntry(const char *mainMenuEntry)
{
  if (_pluginMainMenuEntry != NULL)
     free((void*)_pluginMainMenuEntry);

  if (_title != NULL && strcmp(_title, "") != 0)
     _pluginMainMenuEntry = strdup(_title);
  else if (mainMenuEntry != NULL)
     _pluginMainMenuEntry = strdup(mainMenuEntry);
  else
     _pluginMainMenuEntry = NULL;
}

const char *cSubMenuNode::GetPluginMainMenuEntry()
{
  return(_pluginMainMenuEntry);
}


cSubMenuNodes *cSubMenuNode::GetParentMenu()
{
  return(_parentMenu);
}

void cSubMenuNode::SetParentMenu(cSubMenuNodes *parent)
{
  _parentMenu = parent;
}

cSubMenuNodes *cSubMenuNode::GetCurrentMenu()
{
  return(_currentMenu);
}

void cSubMenuNode::SetCurrentMenu(cSubMenuNodes *current)
{
  _currentMenu = current;
}


cSubMenuNodes *cSubMenuNode::GetSubMenus()
{
  return(&_subMenus);
}

bool cSubMenuNode::HasSubMenus()
{
  if (_subMenus.Count() > 0)
     return(true);
  else
     return(false);
}


void cSubMenuNode::Print(int index)
{
  for (int i = 0; i < index; i++)
      printf(" ");

  printf("Name=%s Type=%s Level=%d", _name, GetTypeAsString(), _level);
  if (_type == COMMAND || _type == THREAD)
     printf(" Command=%s", _command);
  else if (_type == CALL_PLUGIN && _title != NULL)
     printf(" Title=%s", _title);
  printf("\n");

  for (cSubMenuNode *node = _subMenus.First(); node; node = _subMenus.Next(node))
      node->Print(index+4);
}


//################################################################################
//#
//################################################################################
cSubMenu::cSubMenu()
{
  _commandResult         = NULL;
  _currentMenuTree       = &_menuTree;
  _currentParentMenuTree = NULL;
  _currentParentIndex    = -1;
  _nodeArray             = NULL;
  _nrNodes               = 0;
}


cSubMenu::~cSubMenu()
{
  if (_commandResult)
     free(_commandResult);
  if (_nodeArray)
     free(_nodeArray);
  _nrNodes = 0;
}


bool cSubMenu::LoadXml(cString fname)
{
  TiXmlDocument  xmlDoc = TiXmlDocument(fname);
  TiXmlElement  *root   = NULL;
  cSubMenuNode  *node   = NULL;

  bool  ok = true;
  // Clear previously loaded Menu
  _menuTree.Clear();
  _fname = fname;

  if ((ok = xmlDoc.LoadFile())) {
     if ((root = xmlDoc.FirstChildElement("menus")) != NULL) {
        cString tmp = root->Attribute("suffix");
        if (*tmp)
           _menuSuffix = tmp;
        else
           _menuSuffix = cString::sprintf(" ");

        if ((root = root->FirstChildElement()) != NULL) {
           do {
              try {
                 node = new cSubMenuNode(root, 0,  &_menuTree, NULL);
                 _menuTree.Add(node);
                 }
              catch (char *message) {
                 esyslog("ERROR: while decoding XML Node");
                 ok = false;
                 }
              } while (ok == true && (root = root->NextSiblingElement()) != NULL);
           addMissingPlugins();
           removeUndefinedNodes();
           }
        }
     else {
        esyslog("ERROR: in %s, missing Tag <menus>\n", *fname);
        ok = false;
        }
     }
  else {
     esyslog("ERROR: in %s : %s  Col=%d Row=%d\n",
            *fname,
            xmlDoc.ErrorDesc(),
            xmlDoc.ErrorCol(),
            xmlDoc.ErrorRow());
     ok = false;
     }

  return(ok);
}


bool cSubMenu::SaveXml()
{
  return(SaveXml(_fname));
}


bool cSubMenu::SaveXml(cString fname)
{
  bool ok = true;

  if (*_fname) {
     TiXmlDocument xml = TiXmlDocument(fname);
     TiXmlComment  comment;
     comment.SetValue("\n\
-    VDR Menu-Configuration File\n\
-\n\
-\n\
-   Example:\n\
-\n\
 <menus>\n\
    <system name=\"Schedule\" />\n\
    <system name=\"Channels\" />\n\
    <system name=\"Timers\" />\n\
    <system name=\"Recordings\" />\n\
    <menu name=\"System\">\n\
        <system name=\"Setup\" />\n\
        <system name=\"Commands\" />\n\
        <plugin name=\"setup\" title=\"My Setup\" />\n\
        <command name=\"myCommand1\" execute=\"/usr/bin/mycommand1\" />\n\
        <command name=\"myCommand2\" execute=\"/usr/bin/mycommand2\" confirm=\"yes\" />\n\
        <thread name=\"myCommand3\" execute=\"/usr/bin/mycommand3\" confirm=\"yes\" />\n\
        <plugin name=\"epgsearch\" title=\"myProgram\" />\n\
        <menu name=\"mySubSubMenu\">\n\
            ...\n\
        </menu>\n\
    </menu>\n\
     <menu name=\"Suche\">\n\
        <plugin name=\"epgsearch\" />\n\
        ...\n\
    </menu>\n\
 </menus>\n\
");

     TiXmlElement root("menus");
     root.SetAttribute("suffix", _menuSuffix);
     for (cSubMenuNode *node = _menuTree.First(); node; node = _menuTree.Next(node))
         node->SaveXml(&root);

         if (xml.InsertEndChild(comment) != NULL && xml.InsertEndChild(root) != NULL)
            ok = xml.SaveFile(fname);
     }
  else
     ok = false;

  return(ok);
}


cSubMenuNodes *cSubMenu::GetMenuTree()
{
  return(_currentMenuTree);
}


void cSubMenu::PrintMenuTree()
{
  for (cSubMenuNode *node = _menuTree.First(); node; node = _menuTree.Next(node))
      node->Print();
}


int cSubMenu::GetNrOfNodes()
{
  if (_nrNodes == 0) {
     if ((_nrNodes = countNodes(&_menuTree)) > 0) {
        _nodeArray = (cSubMenuNode**) malloc(sizeof(cSubMenuNode*)*_nrNodes);
        int index = 0;
        tree2Array(&_menuTree, index);
        }
     }

  return(_nrNodes);
}


/**
 * returns the specified node within the current menu
 * @param index position in the current menu
 * @return node or null if not found
 */
cSubMenuNode *cSubMenu::GetNode(int index)
{
  cSubMenuNode *node = NULL;
  if (_currentMenuTree == NULL || (node=_currentMenuTree->Get(index)) == NULL)
     esyslog("ERROR: illegal call of cSubMenu::GetNode(%d)", index);

  return(node);
}


/**
 * Get the specified Node
 * @param index specfies the absolut indes in the list of all nodes
 * @return node or NULL if not found
 */
cSubMenuNode *cSubMenu::GetAbsNode(int index)
{
  cSubMenuNode *node = NULL;
  GetNrOfNodes();
  if (_nrNodes > 0 && index >= 0 && index < _nrNodes)
     node = _nodeArray[index];

  return(node);
}


bool cSubMenu::Down(cSubMenuNode *node, int currentIndex)
{
  bool ok = true;
  if (_currentMenuTree != NULL && node && node->GetType() == cSubMenuNode::MENU) {
     _currentParentMenuTree = _currentMenuTree;
     _currentParentIndex = currentIndex;
     _currentMenuTree = node->GetSubMenus();
     }
  else {
     ok = false;
     esyslog("ERROR: illegal call of cSubMenu::Down");
     }

  return(ok);
}

bool cSubMenu::Up(int *parentIndex)
{
  bool ok = true;
  if (_currentMenuTree != NULL && parentIndex != NULL) {
     cSubMenuNode *node = NULL;
     *parentIndex = 0;
     if (_currentParentIndex >= 0)
        *parentIndex = _currentParentIndex;
	  if ( node ) {
	  }
     _currentMenuTree = _currentParentMenuTree;
     if (_currentMenuTree != NULL)
        _currentParentMenuTree = _currentMenuTree->Get(0)->GetParentMenu();
     else
        ok = false;
     }
  else {
     ok = false;
     esyslog("ERROR: illegal call of cSubMenu::Up()");
     }

  return(ok);
}

const char *cSubMenu::ExecuteCommand(const char *cmd)
{
  free(_commandResult);
  _commandResult = NULL;

  dsyslog("executing command '%s'", cmd);
  FILE *p = popen(cmd, "r");
  if (p) {
     int l = 0;
     int c;
     while ((c = fgetc(p)) != EOF) {
           if (l % 20 == 0)
              _commandResult = (char *)realloc(_commandResult, l + 21);
           _commandResult[l++] = c;
           }
     if (_commandResult)
        _commandResult[l] = 0;
     pclose(p);
     }
  else
     esyslog("ERROR: can't open pipe for command '%s'", cmd);

  return _commandResult;
}

/**
 * Move Menu Entry to new Position
 * @param index  index of menu entry to move
 * @param toIndex index of destination
 * @param where After ore before the destination index
 */
void cSubMenu::MoveMenu(int index, int toIndex, enum Where where)
{
  if (index < 0 || index > _nrNodes || // invalid index is ignored
     toIndex < 0 || toIndex > _nrNodes || index == toIndex)
     return;

  cSubMenuNode *srcNode  = GetAbsNode(index);
  cSubMenuNode *destNode = GetAbsNode(toIndex);

  if (where == cSubMenu::INTO && destNode->GetType() != cSubMenuNode::MENU)
     return;

  if (where == cSubMenu::INTO) {
     if (destNode->GetType() == cSubMenuNode::MENU) {
        srcNode->GetCurrentMenu()->Del(srcNode, false);
        srcNode->SetLevel(destNode->GetLevel()+1);
        srcNode->SetParentMenu(destNode->GetCurrentMenu());
        srcNode->SetCurrentMenu(destNode->GetSubMenus());

        destNode->GetSubMenus()->Add(srcNode);
        reloadNodeArray();
        }
     }
  else {
     srcNode->GetCurrentMenu()->Del(srcNode, false);
     srcNode->SetLevel(destNode->GetLevel());
     srcNode->SetParentMenu(destNode->GetParentMenu());
     srcNode->SetCurrentMenu(destNode->GetCurrentMenu());

     if (where == cSubMenu::BEHIND) {
        destNode->GetCurrentMenu()->Add(srcNode, GetAbsNode(toIndex));
        reloadNodeArray();
        }
     else {
        destNode->GetCurrentMenu()->Ins(srcNode, GetAbsNode(toIndex));
        reloadNodeArray();
        }
     }
}

/**
 * Create a new Menu Entry
 * @param index index of destination
 * @param menuTitle  Titel of new Menu entry
 */
void cSubMenu::CreateMenu(int index, const char *menuTitle)
{
  if (index >= 0 && index < _nrNodes) {
     cSubMenuNode *srcNode  = GetAbsNode(index);
     if (srcNode != NULL) {
        cSubMenuNode *newNode = new cSubMenuNode(srcNode->GetParentMenu(), srcNode->GetCurrentMenu());
        newNode->SetLevel(srcNode->GetLevel());
        newNode->SetName(menuTitle);
        newNode->SetType(cSubMenuNode::MENU);
        newNode->SetParentMenu(srcNode->GetParentMenu());
        newNode->SetCurrentMenu(srcNode->GetCurrentMenu());

        srcNode->GetCurrentMenu()->Add(newNode, GetAbsNode(index));
        reloadNodeArray();
        }
     }
}

/**
 * delete the specified entry, or subtree if the specified entry is a menu
 * @param index destion index
 */
void cSubMenu::DeleteMenu(int index)
{
  if (index >= 0 && index < _nrNodes) {
     cSubMenuNode *srcNode = GetAbsNode(index);
     srcNode->GetCurrentMenu()->Del(srcNode, true);
     reloadNodeArray();
     }
}


// Private Methods

int cSubMenu::countNodes(cSubMenuNodes *tree)
{
  int count = 0;
  if (tree != NULL) {
     for (cSubMenuNode *node = tree->First(); node; node = tree->Next(node)) {
         count++;
         if (node->HasSubMenus())
            count += countNodes(node->GetSubMenus());
         }
     }
  return(count);
}


void cSubMenu::tree2Array(cSubMenuNodes *tree, int &index)
{
  if (tree != NULL) {
     for (cSubMenuNode *node = tree->First(); node; node = tree->Next(node)) {
         _nodeArray[index++]=node;
         if (node->HasSubMenus())
            tree2Array(node->GetSubMenus(), index);
         }
     }

}

bool cSubMenu::IsPluginInMenu(const char *name)
{
  bool found = false;
  for (int i = 0; i < _nrNodes && found == false; i++) {
      cSubMenuNode *node = GetAbsNode(i);
      if (node != NULL && node->GetType() == cSubMenuNode::CALL_PLUGIN && strcmp(name, node->GetName()) == 0)
         found = true;
      }
  return(found);
}

/**
 * Adds the given plugin to the Menu-Tree if not allready in List
 * @param name specifies the name of the plugin
 */
void cSubMenu::AddPlugin(const char *name)
{
  if (! IsPluginInMenu(name)) {
     cSubMenuNode *node = new  cSubMenuNode(&_menuTree, NULL);
     node->SetName(name);
     node->SetType("plugin");
     node->SetPlugin();
     _menuTree.Add(node);
     }
}

void cSubMenu::addMissingPlugins()
{
  _nrNodes = GetNrOfNodes();
  for (int i = 0; ; i++) {
      cPlugin *p = cPluginManager::GetPlugin(i);
      if (p)
         AddPlugin(p->Name());
      else
         break;
      }
  reloadNodeArray();
}

/**
 * Adds the given command to the Menu-Tree
 * @param name specifies the name of the command
 */
void cSubMenu::CreateCommand(int index, const char *name, const char *execute, int confirm)
{
  if (index >= 0 && index < _nrNodes) {
     cSubMenuNode *srcNode  = GetAbsNode(index);
     if (srcNode != NULL) {
        cSubMenuNode *newNode = new cSubMenuNode(srcNode->GetParentMenu(), srcNode->GetCurrentMenu());
        newNode->SetLevel(srcNode->GetLevel());
        newNode->SetName(name);
        newNode->SetType("command");
        newNode->SetCommand(execute);
        newNode->SetCommandConfirm(confirm);
        newNode->SetParentMenu(srcNode->GetParentMenu());
        newNode->SetCurrentMenu(srcNode->GetCurrentMenu());

        srcNode->GetCurrentMenu()->Add(newNode, GetAbsNode(index));
        reloadNodeArray();
        }
     }
}

void cSubMenu::CreateThread(int index, const char *name, const char *execute, int confirm)
{
  if (index >= 0 && index < _nrNodes) {
     cSubMenuNode *srcNode  = GetAbsNode(index);
     if (srcNode != NULL) {
        cSubMenuNode *newNode = new cSubMenuNode(srcNode->GetParentMenu(), srcNode->GetCurrentMenu());
        newNode->SetLevel(srcNode->GetLevel());
        newNode->SetName(name);
        newNode->SetType("thread");
        newNode->SetCommand(execute);
        newNode->SetCommandConfirm(confirm);
        newNode->SetParentMenu(srcNode->GetParentMenu());
        newNode->SetCurrentMenu(srcNode->GetCurrentMenu());

        srcNode->GetCurrentMenu()->Add(newNode, GetAbsNode(index));
        reloadNodeArray();
        }
     }
}

/**
 * reloads the internal Array of Nodes
 */
void cSubMenu::reloadNodeArray()
{
  if (_nrNodes > 0)
     free(_nodeArray);
  _nodeArray = NULL;
  _nrNodes = 0;
  _nrNodes = GetNrOfNodes();
}

/**
 * remove Undefined Nodes
 */
void cSubMenu::removeUndefinedNodes()
{
  bool remove = false;

  reloadNodeArray();
  for (int i = 0; i < _nrNodes; i++) {
      cSubMenuNode *node = GetAbsNode(i);
      if (node != NULL && node->GetType() == cSubMenuNode::UNDEFINED) {
         cSubMenuNodes *pMenu = node->GetCurrentMenu();
         pMenu->Del(node, true);
         remove = true;
         }
      }
  if (remove)
     reloadNodeArray();
}


/**
* Retrieves the Menutitel of the parent Menu
*/
const char *cSubMenu::GetParentMenuTitel()
{
  const char *result = "";

  if (_currentMenuTree != NULL && _currentParentMenuTree != NULL) {
     cSubMenuNode *node = NULL;
     for (int i = 0; (node = _currentParentMenuTree->Get(i)) != NULL; i++) {
         if (_currentMenuTree == node->GetSubMenus()) {
            result = node->GetName();
            break;
            }
         }
     }

  return(result);
}

#endif
