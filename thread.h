//
// thread.h
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

#include "event.h"
#include <process.h>

namespace c4u {

class thread
{
public:
	thread () noexcept :
		m_thread (nullptr),
		m_id (0),
		m_suspended (false)
	{
	}
	~thread () noexcept
	{
		close ();
	}
	inline operator HANDLE () const noexcept
	{
		return m_thread;
	}
	inline operator bool () const noexcept
	{
		return (m_thread != nullptr);
	}
	inline bool start (unsigned(__stdcall*fn)(void*),
		LPVOID param = nullptr, bool suspended = false) noexcept
	{
		if (!m_thread)
			m_thread = reinterpret_cast <HANDLE> (_beginthreadex (nullptr, 0, fn, param,
				(suspended ? CREATE_SUSPENDED : 0), &m_id));
		return (m_thread != nullptr);
	}
	inline bool is_suspended () const noexcept
	{
		return m_suspended;
	}
	inline bool wait_for_stop (DWORD timeout = INFINITE) noexcept
	{
		while ( m_thread )
		{
			resume ();
			MSG msg;
			while (PeekMessage (&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage (&msg);
				DispatchMessage (&msg);
			}
			DWORD reason = MsgWaitForMultipleObjects (1, &m_thread, FALSE,
				((timeout == INFINITE) ? 250 : timeout), QS_ALLINPUT);
			switch (reason)
			{
				case WAIT_OBJECT_0 + 1:
					break;

				case WAIT_TIMEOUT:
					if (timeout != INFINITE)
						return false;
					break;

				default: // WAIT_OBJECT_0, WAIT_ABANDONED_0, WAIT_FAILED
					close ();
			}
		}
		return true;
	}
	inline bool is_running () const noexcept
	{
		return m_thread && (CoWaitForSingleObject (m_thread, 0) == WAIT_TIMEOUT);
	}
	inline void suspend () noexcept
	{
		if (m_thread)
			m_suspended = SuspendThread (m_thread) != (DWORD) -1;
	}
	inline void resume () noexcept
	{
		if (m_thread) {
			DWORD ret = (DWORD) -1;
			do {
				ret = ResumeThread (m_thread);
			} while (ret > 0 && ret != (DWORD) -1);
			m_suspended = false;
		}
	}
	inline void close () noexcept
	{
		if (m_thread) {
			locker_holder h (&m_guard);
			if (m_thread) {
				CloseHandle (m_thread);
				m_thread = nullptr;
			}
		}
	}
	inline int priority () const noexcept
	{
		if (m_thread)
			return GetThreadPriority (m_thread);
		else
			return THREAD_PRIORITY_ERROR_RETURN;
	}
	inline void priority (int p) noexcept
	{
		if (m_thread)
			SetThreadPriority (m_thread, p);
	}

protected:
	cs				m_guard;
	HANDLE			m_thread;
	unsigned int	m_id;
	bool			m_suspended;
};

#define THREAD_INLINE(ClassName,Fn) \
	inline bool start##Fn (bool suspended = false) noexcept { \
		m_term##Fn.reset (); \
		return m_thread##Fn.start (stub##Fn, \
			reinterpret_cast<LPVOID> (this), suspended); \
	} \
	inline void stop##Fn () noexcept { \
		m_term##Fn.set (); \
		m_thread##Fn.wait_for_stop (); \
	} \
	inline void suspend##Fn () noexcept { \
		if (!m_term##Fn.check ()) \
			m_thread##Fn.suspend (); \
	} \
	inline void resume##Fn () noexcept { \
		m_thread##Fn.resume (); \
	} \
	c4u::event m_term##Fn; \
	c4u::thread m_thread##Fn; \
	static unsigned __stdcall stub##Fn (void* param) noexcept { \
		ClassName* self = reinterpret_cast <ClassName*> (param); \
		self->##Fn (); \
		return 0; \
	}

#define THREAD(ClassName,Fn) \
	void Fn (); \
	THREAD_INLINE(ClassName,Fn)

}
