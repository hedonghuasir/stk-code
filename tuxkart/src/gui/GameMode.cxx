//  $Id: GameMode.cxx,v 1.5 2004/08/17 21:51:35 grumbel Exp $
//
//  TuxKart - a fun racing game with go-kart
//  Copyright (C) 2004 Steve Baker <sjbaker1@airmail.net>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include "GameMode.h"
#include "tuxkart.h"
#include "WidgetSet.h"

GameMode::GameMode()
{
        menu_id = widgetSet -> vstack(0);
        
        widgetSet -> label(menu_id, "Chose a Race Mode", GUI_LRG, GUI_ALL, 0, 0);

        int va = widgetSet -> varray(menu_id);
	widgetSet -> start(va, "Grand Prix",  GUI_MED, MENU_GP, 0);
	widgetSet -> state(va, "Quick Race",  GUI_MED, MENU_QUICKRACE, 0);
	widgetSet -> state(va, "Time Trial",  GUI_MED, MENU_TIMETRIAL, 0);
	widgetSet -> space(va);
	widgetSet -> space(menu_id);
	
	widgetSet -> layout(menu_id, 0, -1);
}

GameMode::~GameMode()
{
	widgetSet -> delete_widget(menu_id) ;
}
	
void GameMode::update(float dt)
{
	
	widgetSet -> timer(menu_id, dt) ;
	widgetSet -> paint(menu_id) ;
}

void GameMode::select()
{
	switch ( widgetSet -> token (widgetSet -> click()) )
	{
	case MENU_GP:	guiStack.push_back(GUIS_DIFFICULTYGP); break;
	case MENU_QUICKRACE:	guiStack.push_back(GUIS_DIFFICULTYQR); break;
        case MENU_TIMETRIAL:    guiStack.push_back(GUIS_TRACKSEL); break;
	default: break;
	}
}

void GameMode::keybd(const SDL_keysym& key)
{
	switch ( key.sym )
	{
	case SDLK_LEFT:    
	case SDLK_RIGHT:    
	case SDLK_UP:    
	case SDLK_DOWN:
		widgetSet -> pulse(widgetSet -> cursor(menu_id, key.sym), 1.2f);
		break;
		
	case SDLK_RETURN: select(); break;
	
	case SDLK_ESCAPE:
		guiStack.pop_back();
	
	default: break;
	}
}

void GameMode::point(int x, int y)
{
	widgetSet -> pulse(widgetSet -> point(menu_id, x, y), 1.2f);
}

void GameMode::stick(int x, int y)
{
	widgetSet -> pulse(widgetSet -> stick(menu_id, x, y), 1.2f);
}

