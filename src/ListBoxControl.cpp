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

#include <QListWidget>

#include "Container.h"
#include "ListBoxControl.h"
#include "Utils.h"

//////////////////////////////////////////////////////////////////////////////

namespace QtHandles
{

//////////////////////////////////////////////////////////////////////////////

ListBoxControl* ListBoxControl::create (const graphics_object& go)
{
  Object* parent = Object::parentObject (go);

  if (parent)
    {
      Container* container = parent->innerContainer ();

      if (container)
	return new ListBoxControl (go, new QListWidget (container));
    }

  return 0;
}

//////////////////////////////////////////////////////////////////////////////

ListBoxControl::ListBoxControl (const graphics_object& go, QListWidget* list)
     : BaseControl (go, list)
{
  uicontrol::properties& up = properties<uicontrol> ();

  list->addItems (Utils::fromStringVector (up.get_string_vector ()));
  if ((up.get_max () - up.get_min ()) > 1)
    list->setSelectionMode (QAbstractItemView::ExtendedSelection);
  else
    list->setSelectionMode (QAbstractItemView::SingleSelection);
  Matrix value = up.get_value ().matrix_value ();
  if (value.numel () > 0)
    {
      octave_idx_type n = value.numel ();
      int lc = list->count ();

      for (octave_idx_type i = 0; i < n; i++)
	{
	  int idx = xround (value(i));

	  if (1 <= idx && idx <= lc)
	    {
	      list->item (idx-1)->setSelected (true);
	      if (i == 0
		  && list->selectionMode () ==
		  	QAbstractItemView::SingleSelection)
		break;
	    }
	}
    }

  list->removeEventFilter (this);
  list->viewport ()->installEventFilter (this);

  connect (list, SIGNAL (itemSelectionChanged (void)),
	   SLOT (itemSelectionChanged (void)));
}

//////////////////////////////////////////////////////////////////////////////

ListBoxControl::~ListBoxControl (void)
{
}

//////////////////////////////////////////////////////////////////////////////

void ListBoxControl::update (int pId)
{
  uicontrol::properties& up = properties<uicontrol> ();
  QListWidget* list = qWidget<QListWidget> ();

  switch (pId)
    {
    case uicontrol::properties::ID_STRING:
      list->clear ();
      list->addItems (Utils::fromStringVector (up.get_string_vector ()));
      break;
    case uicontrol::properties::ID_MIN:
    case uicontrol::properties::ID_MAX:
      if ((up.get_max () - up.get_min ()) > 1)
	list->setSelectionMode (QAbstractItemView::ExtendedSelection);
      else
	list->setSelectionMode (QAbstractItemView::SingleSelection);
      break;
    case uicontrol::properties::ID_VALUE:
      list->blockSignals (true);
      list->clearSelection ();
	{
	  Matrix value = up.get_value ().matrix_value ();

	  octave_idx_type n = value.numel ();
	  int lc = list->count ();

	  for (octave_idx_type i = 0; i < n; i++)
	    {
	      int idx = xround (value(i));

	      if (1 <= idx && idx <= lc)
		{
		  list->item (idx-1)->setSelected (true);
		  if (i == 0
		      && list->selectionMode () == 
		      		QAbstractItemView::SingleSelection)
		    break;
		}
	    }
	}
      list->blockSignals (false);
      break;
    default:
      BaseControl::update (pId);
      break;
    }
}

//////////////////////////////////////////////////////////////////////////////

void ListBoxControl::itemSelectionChanged (void)
{
  QListWidget* list = qWidget<QListWidget> ();

  QModelIndexList l = list->selectionModel ()->selectedIndexes ();
  Matrix value (dim_vector (1, l.size ()));
  int i = 0;

  foreach (const QModelIndex& idx, l)
    value(i++) = (idx.row () + 1);

  gh_manager::post_set (m_handle, "value", octave_value (value), false);
  gh_manager::post_callback (m_handle, "callback");
}

//////////////////////////////////////////////////////////////////////////////

}; // namespace QtHandles