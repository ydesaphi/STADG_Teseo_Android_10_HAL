#ifndef TESEO_HAL_SIGNAL_H
#define TESEO_HAL_SIGNAL_H

#include <memory>
#include <list>
#include <deque>
#include <functional>
#include <string>
#include <sstream>

#include "constraints.h"

#ifdef SIGNAL_DEBUGGING
#ifndef LOG_TAG
#define LOG_TAG "teseo_hal_Signal"
#define UNDEF_LOG_TAG
#endif

#include <cutils/log.h>
#define SIG_LOGI(...) ALOGI(__VA_ARGS__)
#else
#define SIG_LOGI(...)
#endif

class Trackable
{
private:
	std::shared_ptr<Trackable> shptr;

public:
	Trackable() :
		shptr(this)
	{ }

	~Trackable()
	{
		shptr.reset();
	}

	std::weak_ptr<Trackable> getWeakPtr() const { return shptr; }
};

template <typename Treturn, typename... Targs>
class GenericSlot
{
public:
	typedef std::shared_ptr<GenericSlot<Treturn, Targs...>> ptr;

	virtual Treturn call(Targs... args) = 0;

	virtual Treturn operator()(Targs... args) { return call(args...); }

	virtual bool isValid() { return true; }

	virtual ~GenericSlot() { }
};

template <typename Treturn, typename ...Targs>
class FunctionSlot : public GenericSlot<Treturn, Targs...>
{
public:
	typedef Treturn (*SlotType)(Targs...);

	FunctionSlot(SlotType s) :
		slot(s)
	{ }

	Treturn call(Targs... args)
	{
		return slot(args...);
	}

	virtual bool isValid() { return slot != nullptr; }

private:
	SlotType slot;
};

template <class Tinstance, typename Treturn, typename ...Targs>
class MemberFunctionSlot :
	public GenericSlot<Treturn, Targs...>,
	DerivedFrom<Tinstance, Trackable>
{
public:
	typedef Treturn (Tinstance::*SlotType)(Targs...);

	MemberFunctionSlot(const Tinstance & i, SlotType s) :
		instance(i.getWeakPtr()),
		slot(s)
	{ }

	Treturn call(Targs... args)
	{
		// 1. Get trackable ptr
		Trackable * ti = instance.lock().get();

		// 2. Promote trackable to child class
		// Static cast performs no type checking (ie it is unsage)
		// but we are sure that our pointer to Trackable can be
		// downcasted to Tinstance because of the DerivedFrom
		// constraint.
		Tinstance * i = static_cast<Tinstance*>(ti);

		return (i->*slot)(args...);
	}

	virtual bool isValid()
	{
		return !instance.expired();
	}

private:
	std::weak_ptr<Trackable> instance;
	SlotType slot;
};

namespace SlotFactory
{
	/**
	 * @brief      Create slot from function pointer
	 *
	 * @param[in]  slot     The slot callback
	 *
	 * @tparam     Treturn  The slot return type
	 * @tparam     Targs    The slot arguments types
	 *
	 * @return     The created slot object
	 */
	template<typename Treturn, typename ...Targs>
	auto create(Treturn (*slot)(Targs...))
	{
		return typename GenericSlot<Treturn, Targs...>::ptr(
			new FunctionSlot<Treturn, Targs...>(slot));
	}

	/**
	 * @brief      Create slot from lambda function
	 *
	 * @param[in]  slot       The lambda function callback
	 *
	 * @tparam     Tfunction  Lambda type
	 *
	 * @return     The created slot object
	 */
	template<typename Tfunction>
	auto create(Tfunction slot)
	{
		// Lambda function promoted to function pointer
		// With * and + operators.
		return create(*+slot);
	}

	/**
	 * @brief      Create slot from instance and method pointer
	 *
	 * @param[in]  instance   The instance
	 * @param[in]  slot       The slot callback
	 *
	 * @tparam     Tinstance  The instance type
	 * @tparam     Treturn    The slot return type
	 * @tparam     Targs      The slot arguments types
	 *
	 * @return     The created slot object
	 */
	template<typename Tinstance, typename Treturn, typename ...Targs>
	auto create(const Tinstance & instance, Treturn (Tinstance::*slot)(Targs...))
	{
		return typename GenericSlot<Treturn, Targs...>::ptr(
			new MemberFunctionSlot<Tinstance, Treturn, Targs...>(instance, slot));
	}

	/**
	 * @brief      Create slot from instance and parent class method pointer
	 *
	 * @param[in]  instance         The instance
	 * @param[in]  slot             The slot callback
	 *
	 * @tparam     Tinstance        The instance type
	 * @tparam     TinstanceParent  The instance parent type
	 * @tparam     Treturn          The slot return type
	 * @tparam     Targs            The slot arguments types
	 *
	 * @return     The created slot object
	 */
	template<typename Tinstance, typename TinstanceParent, typename Treturn, typename ...Targs>
	auto create(const Tinstance & instance, Treturn (TinstanceParent::*slot)(Targs...))
	{
		DerivedFrom<Tinstance, TinstanceParent> constraint;
		return typename GenericSlot<Treturn, Targs...>::ptr(
			new MemberFunctionSlot<Tinstance, Treturn, Targs...>(instance, slot));
	}

}

template <typename Treturn, typename ...Targs>
class BaseSignal
{
protected:
	typedef std::list<typename GenericSlot<Treturn, Targs...>::ptr> SlotList;

	SlotList slots;

#ifdef SIGNAL_DEBUGGING
	std::string signalName;
#endif

public:

#ifdef SIGNAL_DEBUGGING
	BaseSignal() : signalName("no-name") { }
	BaseSignal(const char * n) : signalName(n) { }
#else
	BaseSignal() { }
	BaseSignal(const char * n) { (void)(n); }
#endif

	void connect(const typename GenericSlot<Treturn, Targs...>::ptr & slot)
	{
		slots.push_back(slot);
		SIG_LOGI("Add slot %p to %s", slot.get(), toString().c_str());
	}

#ifdef SIGNAL_DEBUGGING
	const char * name() const
	{
		return signalName.c_str();
	}

	std::string toString() const
	{
		std::ostringstream out;

		out << "Signal: '"
		    << signalName
		    << "', connected slots: "
		    << slots.size();

		return out.str();
	}
#endif
};

template <typename Treturn, typename ...Targs>
class Signal :
	public BaseSignal<Treturn, Targs...>
{
protected:

#ifdef SIGNAL_DEBUGGING
	std::string printLastResponseHelper(Treturn resp)
	{
		std::ostringstream out;

		out << resp;

		return out.str();
	}
#endif

public:
	Signal() : BaseSignal<Treturn, Targs...>() { }

	Signal(const char * n) : BaseSignal<Treturn, Targs...>(n) { }

	Treturn emit(Targs... args)
	{
		SIG_LOGI("emit: %s", this->toString().c_str());
		std::deque<typename Signal<Treturn, Targs...>::SlotList::iterator> toErase;
		Treturn lastResponse;

		for(auto it = this->slots.begin(); it != this->slots.end(); ++it)
		{
			auto slot = *it;
			SIG_LOGI("Slot: %p", slot.get());

			if(slot->isValid())
			{
				SIG_LOGI("Slot is valid, call slot.");
				lastResponse = slot->call(args...);
				SIG_LOGI("Slot response: %s", printLastResponseHelper(lastResponse).c_str());
			}
			else
			{
				SIG_LOGI("Slot is not valid, erase slot.");
				toErase.push_back(it);
			}
		}

		for(auto e : toErase)
		{
			SIG_LOGI("Erase slot %p", e->get());
			this->slots.erase(e);
		}

		SIG_LOGI("End of emit, return last response: %s",
			printLastResponseHelper(lastResponse).c_str());
		return lastResponse;
	}

	auto collect(Targs... args);

	Treturn operator()(Targs... args);
};

template<typename Treturn, typename ...Targs>
Treturn Signal<Treturn, Targs...>::operator()(Targs... args)
{
	return emit(args...);
}

template<typename Treturn, typename ...Targs>
auto Signal<Treturn, Targs...>::collect(Targs... args)
{
	SIG_LOGI("emit: %s", this->toString().c_str());
	std::deque<typename Signal<Treturn, Targs...>::SlotList::iterator> toErase;
	std::list<Treturn> responses;

	for(auto it = this->slots.begin(); it != this->slots.end(); ++it)
	{
		auto slot = *it;
		SIG_LOGI("Slot: %p", slot.get());

		if(slot->isValid())
		{
			SIG_LOGI("Slot is valid, call slot.");
			responses.push_back(slot->call(args...));
		}
		else
		{
			SIG_LOGI("Slot is not valid, erase slot.");
			toErase.push_back(it);
		}
	}

	for(auto e : toErase)
	{
		SIG_LOGI("Erase slot %p", e->get());
		this->slots.erase(e);
	}

	SIG_LOGI("End of emit, return %d responses.", (unsigned int)responses.size());
	return responses;
}

template<typename ...Targs>
class Signal<void, Targs...> : public BaseSignal<void, Targs...>
{
public:	
	Signal() : BaseSignal<void, Targs...>() { }

	Signal(const char * n) : BaseSignal<void, Targs...>(n) { }

	void emit(Targs... args);

	void operator()(Targs... args);
	
	auto collect(Targs... args) = delete;
};

template<typename ...Targs>
void Signal<void, Targs...>::emit(Targs... args)
{
	SIG_LOGI("emit: %s", this->toString().c_str());
	std::deque<typename Signal<void, Targs...>::SlotList::iterator> toErase;

	for(auto it = this->slots.begin(); it != this->slots.end(); ++it)
	{
		auto slot = *it;
		SIG_LOGI("Slot: %p", slot.get());

		if(slot->isValid())
		{
			SIG_LOGI("Slot is valid, call slot.");
			slot->call(args...);
		}
		else
		{
			SIG_LOGI("Slot is not valid, erase slot.");
			toErase.push_back(it);
		}
	}

	for(auto e : toErase)
	{
		SIG_LOGI("Erase slot %p", e->get());
		this->slots.erase(e);
	}

	SIG_LOGI("End of emit.");
}

template<typename ...Targs>
void Signal<void, Targs...>::operator()(Targs... args)
{
	emit(args...);
}

#ifdef UNDEF_LOG_TAG
#undef LOG_TAG
#endif

#endif // TESEO_HAL_SIGNAL_H