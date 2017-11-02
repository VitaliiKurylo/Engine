#pragma once

#include <string>
#include <sstream>

#include <Windows.h>

template<class TComObject>
class com_ptr
{
protected:
	TComObject *pointer;

public:
	com_ptr() : pointer(nullptr)
	{
	}

	com_ptr(decltype(__nullptr)) : pointer(nullptr)
	{
	}

	com_ptr(const com_ptr& other) : pointer(other.pointer)
	{
		if (pointer)
		{
			pointer->AddRef();
		}
	}

	template<class TOtherObject>
	com_ptr(__in_opt TOtherObject* other) : pointer(other)
	{
		if (pointer)
		{
			pointer->AddRef();
		}
	}

	~com_ptr()
	{
		using namespace std;

		reset();

		wostringstream debugStream;
		debugStream << "~com_ptr called" << endl;
		OutputDebugString(debugStream.str().c_str());
	}

	com_ptr& operator=(decltype(__nullptr))
	{
		reset();
		return *this;
	}

	com_ptr& operator=(__in_opt TComObject* other)
	{
		if (pointer != other)
		{
			com_ptr(other).swap(*this);
		}

		return *this;
	}

	com_ptr& operator=(const com_ptr &other)
	{
		if (pointer != other.pointer)
		{
			com_ptr(other).swap(*this);
		}

		return *this;
	}

	com_ptr& operator=(__inout com_ptr &&other)
	{
		com_ptr(static_cast<com_ptr&&>(other)).swap(*this);
		return *this;
	}

	void swap(__inout com_ptr&& other)
	{
		auto temporaryPointer = pointer;
		pointer = other.pointer;
		other.pointer = temporaryPointer;
	}

	void swap(__inout com_ptr& other)
	{
		auto temporaryPointer = pointer;
		pointer = other.pointer;
		other.pointer = temporaryPointer;
	}

	explicit operator bool() const noexcept
	{
		return (get() != nullptr);
	}

	TComObject* get() const
	{
		return pointer;
	}

	typename TComObject* operator->() const
	{
		return pointer;
	}

	TComObject* const* getAddress() const
	{
		return &pointer;
	}

	TComObject** getAddress()
	{
		return &pointer;
	}

	TComObject** resetAndGetAddress()
	{
		reset();
		return &pointer;
	}

	unsigned long reset()
	{
		using namespace std;

		auto reference = 0;
		auto temporaryPointer = pointer;

		if (temporaryPointer)
		{
			pointer = nullptr;
			reference = temporaryPointer->Release();
		}

		wostringstream debugStream;
		debugStream << "com_ptr reseted, reference count = " << to_wstring(reference) << endl;
		OutputDebugString(debugStream.str().c_str());

		return reference;
	}
};
