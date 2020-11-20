#ifndef IDELEGATE_H__
#define IDELEGATE_H__

#include "operatingsystem.h"

template <class TArgs>
class Lib_API IDelegate
{
public:
	IDelegate()
	{
	}

	IDelegate(const IDelegate& /*rhs*/)
	{
	}

	virtual ~IDelegate()
	{
	}

	virtual bool notify(const void* sender, TArgs& arguments) = 0;

	virtual bool equals(const IDelegate& other) const = 0;

	virtual IDelegate* clone() const = 0;

	virtual void disable() = 0;

	virtual const IDelegate* unwrap() const
	{
		return this;
	}
};

template <>
class IDelegate<void>
{
public:
	IDelegate()
	{
	}

	IDelegate(const IDelegate& /*rhs*/)
	{
	}

	virtual ~IDelegate()
	{
	}

	virtual bool notify(const void* sender) = 0;

  virtual bool equals(const IDelegate& other) const = 0;

	virtual IDelegate* clone() const = 0;

	virtual void disable() = 0;

	virtual const IDelegate* unwrap() const
	{
		return this;
	}
};


#endif // IDELEGATE_H__
