/**
 * @brief Signal/Slot implementation
 * @file Signal.h
 * @author Baudouin Feildel <baudouin.feildel@st.com>
 * @copyright 2016, STMicroelectronics, All rights reserved.
 */

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

/**
 * @brief      Base class for object that contains slots
 * @details    The object is tracked through a shared pointer. When registering a slot, a weak
 * pointer to the instance is saved. Before calling the slot, the weak pointer is tested. If it has 
 * expired, the connection became invalid and the slot isn't called.
 */
class Trackable
{
private:
	std::shared_ptr<Trackable> shptr; ///< Shared pointer to the instance

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

/**
 * @brief      Base class for slots
 *
 * @tparam     Treturn  Slot return type
 * @tparam     Targs    Slot argument list
 */
template <typename Treturn, typename... Targs>
class GenericSlot
{
public:
	/**
	 * Slot pointer type
	 */
	typedef std::shared_ptr<GenericSlot<Treturn, Targs...>> ptr;

	/**
	 * @brief      Effectively call the slot
	 *
	 * @param[in]  args  Arguments to be passed to the slot
	 *
	 * @return     Return the return value of the slot
	 */
	virtual Treturn call(Targs... args) = 0;

	/**
	 * Shortcut to `Treturn call(Targs... args)`
	 */
	virtual Treturn operator()(Targs... args) { return call(args...); }

	/**
	 * @brief      Determines if the slot is valid.
	 *
	 * @return     True if valid, False otherwise.
	 */
	virtual bool isValid() { return true; }

	/**
	 * @brief      Slot destructor
	 */
	virtual ~GenericSlot() { }
};

/**
 * @brief      Class for function slot.
 *
 * @tparam     Treturn  Slot return type
 * @tparam     Targs    Slot argument list
 */
template <typename Treturn, typename ...Targs>
class FunctionSlot : public GenericSlot<Treturn, Targs...>
{
public:
	/**
	 * Slot function type
	 */
	typedef Treturn (*SlotType)(Targs...);

	FunctionSlot(SlotType s) :
		slot(s)
	{ }

	Treturn call(Targs... args)
	{
		return slot(args...);
	}

	/**
	 * @brief      Determines if the slot is valid.
	 * 
	 * @details    The function slot is considered valid if the function pointer isn't null.
	 *
	 * @return     True if valid, False otherwise.
	 */
	virtual bool isValid() { return slot != nullptr; }

private:
	SlotType slot;
};

/**
 * @brief      Class for member function slot.
 *
 * @tparam     Tinstance  Class of the slot method (must derive from Trackable)
 * @tparam     Treturn    Slot return type
 * @tparam     Targs      Slot argument list
 */
template <class Tinstance, typename Treturn, typename ...Targs>
class MemberFunctionSlot :
	public GenericSlot<Treturn, Targs...>,
	DerivedFrom<Tinstance, Trackable>
{
public:
	/**
	 * Slot function type
	 */
	typedef Treturn (Tinstance::*SlotType)(Targs...);

	MemberFunctionSlot(const Tinstance & i, SlotType s) :
		instance(i.getWeakPtr()),
		slot(s)
	{ }

	/**
	 * @brief      Call the slot method
	 * 
	 * @details    The instance is acquired using the weak pointer to Trackable, then by statically
	 * casting this pointer to Tinstance class.
	 *
	 * @param[in]  args  The arguments
	 *
	 * @return     Return the value return by the slot
	 */
	Treturn call(Targs... args)
	{
		// 1. Get trackable ptr
		Trackable * ti = instance.lock().get();

		// 2. Promote trackable to child class
		// Static cast performs no type checking (i.e.: it is unsafe)
		// but we are sure that our pointer to Trackable can be
		// downcasted to Tinstance because of the DerivedFrom
		// constraint.
		Tinstance * i = static_cast<Tinstance*>(ti);

		return (i->*slot)(args...);
	}

	/**
	 * @brief      Determines if the slot is valid.
	 * 
	 * @details    The member function slot is considered valid when the weak pointer to the tracked
	 * object isn't expired.
	 *
	 * @return     True if valid, False otherwise.
	 */	
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
		// Lambda function promoted to function pointer with * and + operators.
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

/**
 * @brief      Base class for signals.
 *
 * @tparam     Treturn  Signal return type
 * @tparam     Targs    Signal argument list
 */
template <typename Treturn, typename ...Targs>
class BaseSignal
{
protected:
	/**
	 * Slot list type
	 */
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

	/**
	 * @brief      Connect signal to slot
	 *
	 * @param[in]  slot  The slot
	 */
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
#else
	const char * name() const
	{
		return "signal-debugging-disabled";
	}

	std::string toString() const
	{
		return "Signal: debugging disabled.";
	}
#endif
};

/**
 * @brief      Signal class
 *
 * @tparam     Treturn  Signal return type
 * @tparam     Targs    Signal argument list
 */
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
#else
	std::string printLastResponseHelper(Treturn resp)
	{
		(void)(resp);
		return "Signal debugging disabled.";
	}
#endif

public:
	Signal() : BaseSignal<Treturn, Targs...>() { }

	Signal(const char * n) : BaseSignal<Treturn, Targs...>(n) { }

	/**
	 * @brief      Emit the signal
	 *
	 * @param[in]  args  The arguments
	 *
	 * @return     Value returned by the last called slot. Or Treturn default value if no slot is
	 * attached to this signal. The default value may be a random value in case of an POD type.
	 */
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

	/**
	 * @brief      Emit the signal and collect all the responses
	 *
	 * @param[in]  args  The arguments
	 *
	 * @return     List of all the slots responses
	 */
	auto collect(Targs... args);

	/**
	 * Shortcut to `Treturn emit(Targs... )`
	 */
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

/**
 * @brief      Signal template specialization for void-return signals
 *
 * @tparam     Targs  Signal argument list
 */
template<typename ...Targs>
class Signal<void, Targs...> : public BaseSignal<void, Targs...>
{
public:	
	Signal() : BaseSignal<void, Targs...>() { }

	Signal(const char * n) : BaseSignal<void, Targs...>(n) { }

	void emit(Targs... args);

	void operator()(Targs... args);
	
	/**
	 * @brief The collect method is deleted because we can't return a list of void.
	 */
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