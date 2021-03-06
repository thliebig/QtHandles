/*

Copyright (C) 2011 Michael Goffioul.

This file is part of QtHandles.

Foobar is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

QtHandles is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef __QtHandles_Menu__
#define __QtHandles_Menu__ 1

#include "MenuContainer.h"
#include "Object.h"

class QAction;
class QMenu;
class QWidget;

//////////////////////////////////////////////////////////////////////////////

namespace QtHandles
{

//////////////////////////////////////////////////////////////////////////////

class Menu : public Object, public MenuContainer
{
  Q_OBJECT

public:
  Menu (const graphics_object& go, QAction* action, Object* parent);
  ~Menu (void);

  static Menu* create (const graphics_object& go);

  Container* innerContainer (void) { return 0; }

  QWidget* menu (void);

protected:
  void update (int pId);

private slots:
  void actionTriggered (void);
  void actionHovered (void);

private:
  void updateSiblingPositions (void);

private:
  QWidget* m_parent;
  QAction* m_separator;
};

//////////////////////////////////////////////////////////////////////////////

}; // namespace QtHandles

//////////////////////////////////////////////////////////////////////////////

#endif
