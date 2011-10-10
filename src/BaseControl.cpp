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

#include <QEvent>
#include <QMouseEvent>
#include <QWidget>

#include "BaseControl.h"
#include "Utils.h"

//////////////////////////////////////////////////////////////////////////////

namespace QtHandles
{

//////////////////////////////////////////////////////////////////////////////

static void updatePalette (const uicontrol::properties& props, QWidget* w)
{
  QPalette p = w->palette ();

  if (props.style_is ("edit")
      || props.style_is ("listbox")
      || props.style_is ("popupmenu"))
    {
      p.setColor (QPalette::Base,
		  Utils::fromRgb (props.get_backgroundcolor_rgb ()));
      p.setColor (QPalette::Text,
		  Utils::fromRgb (props.get_foregroundcolor_rgb ()));
    }
  else if (props.style_is ("pushbutton")
	   || props.style_is ("togglebutton"))
    {
      p.setColor (QPalette::Button,
		  Utils::fromRgb (props.get_backgroundcolor_rgb ()));
      p.setColor (QPalette::ButtonText,
		  Utils::fromRgb (props.get_foregroundcolor_rgb ()));
    }
  else
    {
      p.setColor (QPalette::Window,
		  Utils::fromRgb (props.get_backgroundcolor_rgb ()));
      p.setColor (QPalette::WindowText,
		  Utils::fromRgb (props.get_foregroundcolor_rgb ()));
    }

  w->setPalette (p);
}

//////////////////////////////////////////////////////////////////////////////

BaseControl::BaseControl (const graphics_object& go, QWidget* w)
  : Object (go, w), m_normalizedFont (false)
{
  init (w);
}

//////////////////////////////////////////////////////////////////////////////

void BaseControl::init (QWidget* w, bool callBase)
{
  if (callBase)
    Object::init (w, callBase);

  uicontrol::properties& up = properties<uicontrol> ();

  Matrix bb = up.get_boundingbox (false);
  w->setGeometry (xround (bb(0)), xround (bb(1)),
		  xround (bb(2)), xround (bb(3)));
  w->setFont (Utils::computeFont<uicontrol> (up, bb(3)));
  updatePalette (up, w);
  w->setEnabled (up.enable_is ("on"));
  w->setToolTip (Utils::fromStdString (up.get_tooltipstring ()));
  w->setVisible (up.is_visible ());

  w->installEventFilter (this);

  m_normalizedFont = up.fontunits_is ("normalized");
}

//////////////////////////////////////////////////////////////////////////////

BaseControl::~BaseControl (void)
{
}

//////////////////////////////////////////////////////////////////////////////

void BaseControl::update (int pId)
{
  uicontrol::properties& up = properties<uicontrol> ();
  QWidget* w = qWidget<QWidget> ();

   switch (pId)
    {
    case uicontrol::properties::ID_POSITION:
	{
	  Matrix bb = up.get_boundingbox (false);
	  w->setGeometry (xround (bb(0)), xround (bb(1)),
			  xround (bb(2)), xround (bb(3)));
	}
      break;
    case uicontrol::properties::ID_FONTNAME:
    case uicontrol::properties::ID_FONTSIZE:
    case uicontrol::properties::ID_FONTWEIGHT:
    case uicontrol::properties::ID_FONTANGLE:
      w->setFont (Utils::computeFont<uicontrol> (up));
      break;
    case uicontrol::properties::ID_FONTUNITS:
      // FIXME: We shouldn't have to do anything, octave should update
      //        the "fontsize" property automatically to the new units.
      //        Hence the actual font used shouldn't change.
      m_normalizedFont = up.fontunits_is ("normalized");
      break;
    case uicontrol::properties::ID_BACKGROUNDCOLOR:
    case uicontrol::properties::ID_FOREGROUNDCOLOR:
      updatePalette (up, w);
      break;
    case uicontrol::properties::ID_ENABLE:
      w->setEnabled (up.enable_is ("on"));
      break;
    case uicontrol::properties::ID_TOOLTIPSTRING:
      w->setToolTip (Utils::fromStdString (up.get_tooltipstring ()));
      break;
    case base_properties::ID_VISIBLE:
      w->setVisible (up.is_visible ());
      break;
    default:
      break;
    }
}

//////////////////////////////////////////////////////////////////////////////

bool BaseControl::eventFilter (QObject* watched, QEvent* event)
{
  switch (event->type ())
    {
    case QEvent::Resize:
      if (m_normalizedFont)
	{
	  gh_manager::auto_lock lock;

	  qWidget<QWidget> ()->setFont (Utils::computeFont<uicontrol>
					(properties<uicontrol> ()));
	}
      break;
    case QEvent::MouseButtonPress:
	{
	  gh_manager::auto_lock lock;

	  QMouseEvent* m = dynamic_cast<QMouseEvent*> (event);
	  graphics_object go = object ();
	  uicontrol::properties& up = Utils::properties<uicontrol> (go);
	  graphics_object fig = go.get_ancestor ("figure");

	  if (m->button () != Qt::LeftButton
	      || ! up.enable_is ("on"))
	    {
	      gh_manager::post_set (fig.get_handle (), "selectiontype",
				    Utils::figureSelectionType (m), false);
	      gh_manager::post_set (fig.get_handle (), "currentpoint",
				    Utils::figureCurrentPoint (fig, m),
				    false);
	      gh_manager::post_callback (fig.get_handle (),
					 "windowbuttondownfcn");
	      gh_manager::post_callback (m_handle, "buttondownfcn");
	    }
	  else
	    {
	      if (up.style_is ("listbox"))
		gh_manager::post_set (fig.get_handle (), "selectiontype",
				      Utils::figureSelectionType (m), false);
	      else
		gh_manager::post_set (fig.get_handle (), "selectiontype",
				      octave_value ("normal"), false);
	    }
	}
      break;
    default: break;
    }

  return Object::eventFilter (watched, event);
}

//////////////////////////////////////////////////////////////////////////////

}; // namespace QtHandles