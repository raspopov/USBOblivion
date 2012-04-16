#pragma once

#include "cs.h"

namespace c4u {

class event
{
public:
	event (BOOL manual = TRUE, BOOL initial = FALSE, LPCTSTR name = NULL) throw()
	{
		m_event = CreateEvent (NULL, manual, initial, name);
	}
	~event () throw()
	{
		if (m_event)
			CloseHandle (m_event);
	}
	bool create (BOOL manual = TRUE, BOOL initial = FALSE, LPCTSTR name = NULL) throw()
	{
		if (m_event)
			CloseHandle (m_event);
		m_event = CreateEvent (NULL, manual, initial, name);
		return (m_event != NULL);
	}
	inline operator HANDLE () const throw()
	{
		return m_event;
	}
	inline bool check () const throw()
	{
		return (CoWaitForSingleObject (m_event, 0) == WAIT_OBJECT_0);
	}
	inline DWORD wait (const DWORD timeout) const throw()
	{
		return CoWaitForSingleObject (m_event, timeout);
	}
	inline bool set () throw()
	{
		return (SetEvent (m_event) == TRUE);
	}
	inline bool reset () throw()
	{
		return (ResetEvent (m_event) == TRUE);
	}
	inline bool pulse () throw()
	{
		return (PulseEvent (m_event) == TRUE);
	}
	struct set_event {
		inline void operator () (const HANDLE h) throw ()
		{
			SetEvent (h);
		}
	};
	struct reset_event {
		inline void operator () (const HANDLE h) throw ()
		{
			ResetEvent (h);
		}
	};
	struct pulse_event {
		inline void operator () (const HANDLE h) throw ()
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
