//
// event.h
//
// Copyright (c) Nikolay Raspopov, 2009-2015.
// This file is part of USB Oblivion (http://www.cherubicsoft.com/en/projects/usboblivion)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//

#pragma once

#include "cs.h"

namespace c4u {

class event
{
public:
	event (BOOL manual = TRUE, BOOL initial = FALSE, LPCTSTR name = nullptr) noexcept
	{
		m_event = CreateEvent (nullptr, manual, initial, name);
	}
	~event () noexcept
	{
		if (m_event)
			CloseHandle (m_event);
	}
	bool create (BOOL manual = TRUE, BOOL initial = FALSE, LPCTSTR name = nullptr) noexcept
	{
		if (m_event)
			CloseHandle (m_event);
		m_event = CreateEvent (nullptr, manual, initial, name);
		return (m_event != nullptr);
	}
	inline operator HANDLE () const noexcept
	{
		return m_event;
	}
	inline bool check () const noexcept
	{
		return (CoWaitForSingleObject (m_event, 0) == WAIT_OBJECT_0);
	}
	inline DWORD wait (const DWORD timeout) const noexcept
	{
		return CoWaitForSingleObject (m_event, timeout);
	}
	inline bool set () noexcept
	{
		return (SetEvent (m_event) != FALSE);
	}
	inline bool reset () noexcept
	{
		return (ResetEvent (m_event) != FALSE);
	}
	inline bool pulse () noexcept
	{
		return (PulseEvent (m_event) != FALSE);
	}
	struct set_event {
		inline void operator () (const HANDLE h) noexcept
		{
			SetEvent (h);
		}
	};
	struct reset_event {
		inline void operator () (const HANDLE h) noexcept
		{
			ResetEvent (h);
		}
	};
	struct pulse_event {
		inline void operator () (const HANDLE h) noexcept
		{
			PulseEvent (h);
		}
	};
protected:
	event (const event&);
	event& operator = (const event&);

	HANDLE m_event;
};

}
