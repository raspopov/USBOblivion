//
// targetver.h 
//
// Copyright (c) Nikolay Raspopov, 2009-2010.
// This file is part of USB Oblivion (www.cherubicsoft.com)
//
// Shareaza is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// Shareaza is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Shareaza; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#pragma once

#ifndef WINVER					// ”казывает, что минимальной требуемой платформой €вл€етс€ Windows Vista.
#define WINVER 0x0600			// ¬ыберите значение, указывающее на другие версии Windows.
#endif

#ifndef _WIN32_WINNT			// ”казывает, что минимальной требуемой платформой €вл€етс€ Windows Vista.
#define _WIN32_WINNT 0x0600		// ¬ыберите значение, указывающее на другие версии Windows.
#endif

#ifndef _WIN32_WINDOWS			// ”казывает, что минимальной требуемой платформой €вл€етс€ Windows 98.
#define _WIN32_WINDOWS 0x0410	// ¬ыберите значение, указывающее на Windows Me или более поздние версии Windows.
#endif

#ifndef _WIN32_IE				// ”казывает, что минимальной требуемой платформой €вл€етс€ Internet Explorer 7.0.
#define _WIN32_IE 0x0700		// ¬ыберите значение, указывающее на другие версии Internet Explorer.
#endif
