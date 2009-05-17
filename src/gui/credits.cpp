//  $Id$
//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2006 Joerg Henrichs
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 3
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


#include "user_config.hpp"
#include "gui/credits.hpp"
#include "gui/engine.hpp"
#include "io/file_manager.hpp"
#include <fstream>
#include <irrString.h>

using irr::core::stringw;
using irr::core::stringc;

namespace GUIEngine
{
    const float TIME_SECTION_FADE = 0.4f;

    class CreditsEntry
        {
        public:
            stringw m_name;
            std::vector<stringw> m_subentries;
            
            CreditsEntry(stringw& name)
            {
                m_name = name;
            }
        };
    
    
    class CreditsSection
        {
        public:
            // read-only
            std::vector<CreditsEntry> m_entries;
            stringw m_name;
            
            CreditsSection(stringw name)
            {
                this->m_name = name;
            }
            void addEntry(CreditsEntry& entry)
            {
                m_entries.push_back(entry);
            }
            void addSubEntry(stringw& subEntryString)
            {
                m_entries[m_entries.size()-1].m_subentries.push_back(subEntryString);
            }
        };
    
    CreditsSection* Credits::getCurrentSection()
    {
        return m_sections.get(m_sections.size()-1);
    }
    
    Credits::Credits()
    {
        reset();
        
        std::string creditsfile = file_manager->getDataDir() + "/CREDITS";

        std::ifstream file( creditsfile.c_str() ) ;
        stringw line;
        std::string getline;
        while( std::getline( file, getline ) )
        {
            line = getline.c_str();
            line = line.trim();
            
            if(line.size() < 1) continue; // empty line
            
            if(line[0] == '=' && line[line.size()-1] == '=')
            {
                line = stringw( line.subString(1, line.size()-2).c_str() );
                line = line.trim();
                
                //stringc cversion = line.c_str();
                //std::cout << "Section : " << (char*)(cversion.c_str()) << std::endl;
                m_sections.push_back( new CreditsSection(line)  );
            }
            else if(line[0] == '-')
            {
                line = stringw( line.subString(1, line.size()-1).c_str() );
                line = line.trim();
                
                //stringc cversion = line.c_str();
                //std::cout << "---- Sub-Entry : " << (char*)(cversion.c_str()) << std::endl;
                
                getCurrentSection()->addSubEntry( line );
            }
            else
            {
                //tringc cversion = line.c_str();
                //std::cout << "-- Entry : " << (char*)(cversion.c_str()) << std::endl;
                
                CreditsEntry entry(line);
                getCurrentSection()->addEntry( entry );
            }
        } // end while
        
    }
    
    void Credits::setArea(const int x, const int y, const int w, const int h)
    {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
        
        m_section_rect = core::rect< s32 >( x, y, x+w, y+h/6 );
    }

    void Credits::reset()
    {
        m_curr_section = 0;
        m_curr_element = -1;
        time_before_next_step = TIME_SECTION_FADE;
        m_time_element = 2.5f;
    }
    
    void Credits::render(const float elapsed_time)
    {
        time_before_next_step -= elapsed_time;

        const bool before_first_elem = (m_curr_element == -1);
        const bool after_last_elem = (m_curr_element >= (int)m_sections[m_curr_section].m_entries.size());

        
        // ---- section name
        video::SColor color( 255 /* a */, 0 /* r */, 0 /* g */ , 75 /* b */ );
        
        // manage fade-in
        if(before_first_elem)
        {
            int alpha = 255 - (int)(time_before_next_step/TIME_SECTION_FADE * 255);
            if(alpha < 0) alpha = 0;
            else if(alpha > 255) alpha = 255;
            color.setAlpha( alpha );
        }
        // manage fade-out
        else if(after_last_elem)
        {
            int alpha = (int)(time_before_next_step/TIME_SECTION_FADE * 255);
            if(alpha < 0) alpha = 0;
            else if(alpha > 255) alpha = 255;
            color.setAlpha( alpha );
        }
        
        GUIEngine::getFont()->draw(m_sections[m_curr_section].m_name.c_str(), m_section_rect, color,
                                   true /* center h */, true /* center v */ );
        
        // draw entries
        if(!before_first_elem && !after_last_elem)
        {
            int text_offset  = 0;
            
            // fade in
            if(time_before_next_step < 0.3f)
            {
                const float fade_in = time_before_next_step / 0.3f;
                
                int alpha =  (int)(fade_in * 255);
                if(alpha < 0) alpha = 0;
                else if(alpha > 255) alpha = 255;
                color.setAlpha( alpha );
                
                text_offset = (int)((1.0f - fade_in) * 100);
            }
            // fade out
            else if(time_before_next_step >= m_time_element - 0.3f)
            {
                const float fade_out = (time_before_next_step - (m_time_element - 0.3f)) / 0.3f;
                
                int alpha = 255 - (int)(fade_out * 255);
                if(alpha < 0) alpha = 0;
                else if(alpha > 255) alpha = 255;
                color.setAlpha( alpha );
                
                text_offset = -(int)(fade_out * 100);
            }
            
            
            GUIEngine::getFont()->draw(m_sections[m_curr_section].m_entries[m_curr_element].m_name.c_str(),
                                       core::rect< s32 >( x + text_offset, y+h/6, x+w+text_offset, y+h/3 ), color,
                                       false /* center h */, true /* center v */ );
            
            const int subamount = m_sections[m_curr_section].m_entries[m_curr_element].m_subentries.size();
            int suby = y+h/3;
            const int inc = subamount == 0 ? h/8 : std::min(h/8, (h - h/3)/(subamount+1));
            for(int i=0; i<subamount; i++)
            {
               GUIEngine::getFont()->draw(m_sections[m_curr_section].m_entries[m_curr_element].m_subentries[i].c_str(),
                                          core::rect< s32 >( x + 32, suby + text_offset/(1+1), x+w+32, suby+h/8 + text_offset/(1+1) ),
                                          color,
                                          false/* center h */, true /* center v */ );
                suby += inc;
            }

        }
        
        // is it time to move on?
        if(time_before_next_step < 0)
        {
            if(after_last_elem)
            {
                // switch to next element
                m_curr_section++;
                m_curr_element = -1;
                time_before_next_step = TIME_SECTION_FADE;
                
                if(m_curr_section >= (int)m_sections.size()) reset();
            }
            else
            {
                // move on
                m_curr_element++;
                
                if(m_curr_element >= (int)m_sections[m_curr_section].m_entries.size())
                    time_before_next_step = TIME_SECTION_FADE;
                else
                {
                    m_time_element = 1.6f + (int)m_sections[m_curr_section].m_entries[m_curr_element].m_subentries.size()*0.5f;
                    time_before_next_step = m_time_element;
                }
            }
        }
        
        /*
         draw (const wchar_t *text, const core::rect< s32 > &position, video::SColor color,
         bool hcenter=false,
         bool vcenter=false, const core::rect< s32 > *clip=0)=0
         */
    }
    
    static Credits* singleton = NULL;
    Credits* Credits::getInstance()
    {
        if(singleton == NULL) singleton = new Credits();
        
        return singleton;
    }
} // end namespace