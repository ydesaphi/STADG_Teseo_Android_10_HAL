/*
* This file is part of Teseo Android HAL
*
* Copyright (c) 2016-2017, STMicroelectronics - All Rights Reserved
* Author(s): Baudouin Feildel <baudouin.feildel@st.com> for STMicroelectronics.
*
* License terms: Apache 2.0.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/
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
namespace signal_debug {
void signal_log_verbose(const char * fmt, ...);
void signal_log_debug(const char * fmt, ...);
void signal_log_info(const char * fmt, ...);
void signal_log_warning(const char * fmt, ...);
void signal_log_error(const char * fmt, ...);
} // namespace signal_debug

#define SIG_LOGV(...) signal_debug::signal_log_verbose(__VA_ARGS__)
#define SIG_LOGD(...) signal_debug::signal_log_debug(__VA_ARGS__)
#define SIG_LOGI(...) signal_debug::signal_log_info(__VA_ARGS__)
#define SIG_LOGW(...) signal_debug::signal_log_warning(__VA_ARGS__)
#define SIG_LOGE(...) signal_debug::signal_log_error(__VA_ARGS__)
#else
#define SIG_LOGV(...) 
#define SIG_LOGD(...) 
#define SIG_LOGI(...) 
#define SIG_LOGW(...) 
#define SIG_LOGE(...) 
#endif

template<typename T>
struct EmptyDeleter {
	void operator()(T * ptr)
	{
		#ifdef TESEO_UNIT_TEST
		std::cout << "Empty deleter called on " << ptr << "." << std::endl;
		#else
		SIG_LOGD("Empty deleter called on %p.", ptr);
		#endif
	}
};

template<typename T>
auto make_empty_deleter(T *)
{
	return EmptyDeleter<T>();
}

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
	Trackable() : shptr(this, EmptyDeleter<Trackable>())
	{ }

	~Trackable()
	{ }

	using weak_ptr = std::weak_ptr<Trackable>;

	weak_ptr getWeakPtr() const { return shptr; }
};

template<class Tfunc> class GenericSlot;

/**
 * @brief      Base class for slots
 *
 * @tparam     Treturn  Slot return type
 * @tparam     Targs    Slot argument list
 */
template <typename Treturn, typename... Targs>
class GenericSlot<Treturn(Targs...)>
{
public:
	/**
	 * Slot pointer type
	 */
	typedef std::shared_ptr<GenericSlot<Treturn(Targs...)>> ptr;

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
class FunctionSlot : public GenericSlot<Treturn (Targs...)>
{
public:
	/**
	 * Slot function type
	 */
	using SlotType = std::function<Treturn (Targs...)>;

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
	virtual bool isValid() { return static_cast<bool>(slot); }

private:
	SlotType slot;
};

/**
 * @brief      Class for class method slot.
 *
 * @tparam     Tinstance  Class of the slot method (must derive from Trackable)
 * @tparam     Treturn    Slot return type
 * @tparam     Targs      Slot argument list
 */
template <class Tinstance, typename Treturn, typename ...Targs>
class ClassMethodSlot :
	public GenericSlot<Treturn (Targs...)>,
	DerivedFrom<Tinstance, Trackable>
{
public:
	/**
	 * Slot function type
	 */
	typedef Treturn (Tinstance::*SlotType)(Targs...);

	ClassMethodSlot(const Tinstance & i, SlotType s) :
		instance(i.getWeakPtr()),
		slot(s)
	{
		if(slot == nullptr)
			throw std::runtime_error("Can't register ClassMethodSlot with nullptr slot.");
	}

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
	Trackable::weak_ptr instance;
	SlotType slot;
};

namespace signal_implementation {

template <bool Debug=false>
struct SignalDebugger {
	template<typename ...T>
	void logv(const char *, T...)
	{ }

	template<typename ...T>
	void logd(const char *, T...)
	{ }

	template<typename ...T>
	void logi(const char *, T...)
	{ }

	template<typename ...T>
	void logw(const char *, T...)
	{ }

	template<typename ...T>
	void loge(const char *, T...)
	{ }
};

template <>
struct SignalDebugger<true> {
	template<typename ...T>
	void logv(const char * fmt, T... args)
	{
		SIG_LOGV(fmt, args...);
	}

	template<typename ...T>
	void logd(const char * fmt, T... args)
	{
		SIG_LOGD(fmt, args...);
	}

	template<typename ...T>
	void logi(const char * fmt, T... args)
	{
		SIG_LOGI(fmt, args...);
	}

	template<typename ...T>
	void logw(const char * fmt, T... args)
	{
		SIG_LOGW(fmt, args...);
	}

	template<typename ...T>
	void loge(const char * fmt, T... args)
	{
		SIG_LOGE(fmt, args...);
	}
};
	
template <bool DebugFlag, typename Treturn, typename ...Targs>
class AbstractSignal
{
protected:
	SignalDebugger<DebugFlag> dbg;

	/**
	 * Slot list type
	 */
	typedef std::list<typename GenericSlot<Treturn (Targs...)>::ptr> SlotList;
	 
	SlotList slots;

	std::string signalName;

	friend class AbstractSignal<!DebugFlag, Treturn, Targs...>;

public:
	AbstractSignal() :
		signalName("no-name")
	{
		this->dbg.logw("You are debugging a Signal without giving him a name. "
			           "This can be hard, good luck.");
	}

	AbstractSignal(const char * n) :
		signalName(n)
	{ }

	virtual ~AbstractSignal()
	{ }

	/**
	 * @brief      Connect signal to slot
	 *
	 * @param[in]  slot  The slot
	 */
	void connect(const typename GenericSlot<Treturn (Targs...)>::ptr & slot)
	{
		slots.push_back(slot);
		this->dbg.logi("Add slot %p to %s", slot.get(), toString().c_str());
	}

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
};

/**
 * @brief      Base class for signals.
 *
 * @tparam     Treturn  Signal return type
 * @tparam     Targs    Signal argument list
 */
template <bool DebugFlag, typename Treturn, typename ...Targs>
class BaseSignal :
	public AbstractSignal<DebugFlag, Treturn, Targs...>
{
protected:
	Treturn call(Targs... args)
	{
		this->dbg.logd("Signal forwarded to %s", this->toString().c_str());
		return emit(args...);
	}

public:

	BaseSignal() : AbstractSignal<DebugFlag, Treturn, Targs...>()
	{ }

	BaseSignal(const char * n) : AbstractSignal<DebugFlag, Treturn, Targs...>(n)
	{ }

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
		this->dbg.logi("emit: %s", this->toString().c_str());
		std::deque<typename AbstractSignal<DebugFlag, Treturn, Targs...>::SlotList::iterator> toErase;
		Treturn lastResponse;

		for(auto it = this->slots.begin(); it != this->slots.end(); ++it)
		{
			auto slot = *it;
			this->dbg.logi("Slot: %p", slot.get());

			if(slot->isValid())
			{
				this->dbg.logi("Slot is valid, call slot.");
				lastResponse = slot->call(args...);
				this->dbg.logi("Slot response: %s",
					(std::ostringstream() << lastResponse).str().c_str());
			}
			else
			{
				this->dbg.logi("Slot is not valid, erase slot.");
				toErase.push_back(it);
			}
		}

		for(auto e : toErase)
		{
			this->dbg.logi("Erase slot %p", e->get());
			this->slots.erase(e);
		}

		this->dbg.logi("End of emit, return last response: %s",
			(std::ostringstream() << lastResponse).str().c_str());
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

template<bool DebugFlag, typename Treturn, typename ...Targs>
Treturn BaseSignal<DebugFlag, Treturn, Targs...>::operator()(Targs... args)
{
	return emit(args...);
}

template<bool DebugFlag, typename Treturn, typename ...Targs>
auto BaseSignal<DebugFlag, Treturn, Targs...>::collect(Targs... args)
{
	this->dbg.logi("emit: %s", this->toString().c_str());
	std::deque<typename AbstractSignal<DebugFlag, Treturn, Targs...>::SlotList::iterator> toErase;
	std::list<Treturn> responses;

	for(auto it = this->slots.begin(); it != this->slots.end(); ++it)
	{
		auto slot = *it;
		this->dbg.logi("Slot: %p", slot.get());

		if(slot->isValid())
		{
			this->dbg.logi("Slot is valid, call slot.");
			responses.push_back(slot->call(args...));
		}
		else
		{
			this->dbg.logi("Slot is not valid, erase slot.");
			toErase.push_back(it);
		}
	}

	for(auto e : toErase)
	{
		this->dbg.logi("Erase slot %p", e->get());
		this->slots.erase(e);
	}

	this->dbg.logi("End of emit, return %d responses.", (unsigned int)responses.size());
	return responses;
}

/**
 * @brief      Signal template specialization for void-return signals
 *
 * @tparam     Targs  Signal argument list
 */
 template<bool DebugFlag, typename ...Targs>
 class BaseSignal<DebugFlag, void, Targs...> :
	public AbstractSignal<DebugFlag, void, Targs...>
 {
 protected:
	void call(Targs... args)
	{
		this->dbg.logd("Signal forwarded to %s", this->toString().c_str());
		emit(args...);
	}
 
 public:

	BaseSignal() : AbstractSignal<DebugFlag, void, Targs...>()
	{ }

	BaseSignal(const char * n) : AbstractSignal<DebugFlag, void, Targs...>(n)
	{ }

	void emit(Targs... args);
 
	void operator()(Targs... args);
	 
	/**
	 * @brief The collect method is deleted because we can't return a list of void.
	 */
	auto collect(Targs... args) = delete;
 };
 
template<bool DebugFlag, typename ...Targs>
void BaseSignal<DebugFlag, void, Targs...>::emit(Targs... args)
{
	this->dbg.logi("emit: %s", this->toString().c_str());
	std::deque<typename AbstractSignal<DebugFlag, void, Targs...>::SlotList::iterator> toErase;

	for(auto it = this->slots.begin(); it != this->slots.end(); ++it)
	{
		auto slot = *it;
		this->dbg.logi("Slot: %p", slot.get());

		if(slot->isValid())
		{
			this->dbg.logi("Slot is valid, call slot.");
			slot->call(args...);
		}
		else
		{
			this->dbg.logi("Slot is not valid, erase slot.");
			toErase.push_back(it);
		}
	}

	for(auto e : toErase)
	{
		this->dbg.logi("Erase slot %p", e->get());
		this->slots.erase(e);
	}

	this->dbg.logi("End of emit.");
}

template<bool DebugFlag, typename ...Targs>
void BaseSignal<DebugFlag, void, Targs...>::operator()(Targs... args)
{
	emit(args...);
}

} // namespace signal_implementation

/**
 * @brief      Signal class
 *
 * @tparam     Treturn  Signal return type
 * @tparam     Targs    Signal argument list
 */
template <typename Treturn, typename ...Targs>
class Signal :
	public signal_implementation::BaseSignal<false, Treturn, Targs...>
{
public:
	Signal() :
	signal_implementation::BaseSignal<false, Treturn, Targs...>()
	{ }

	Signal(const char * n) :
	signal_implementation::BaseSignal<false, Treturn, Targs...>(n)
	{ }
};

template<typename Treturn, typename ...Targs>
class SignalToDebug :
	public signal_implementation::BaseSignal<true, Treturn, Targs...>
{
public:
	SignalToDebug(const char * n) :
	signal_implementation::BaseSignal<true, Treturn, Targs...>(n)
	{ }
};

namespace SlotFactory
{
	/**
	 * @brief      Create slot from function object
	 *
	 * @param[in]  slot       The function object
	 *
	 * @tparam     Treturn  The slot return type
	 * @tparam     Targs    The slot arguments types
	 *
	 * @return     The created slot object
	 */
	template<typename Treturn, typename ...Targs>
	auto create(std::function<Treturn (Targs...)> && slot)
	{
		return typename GenericSlot<Treturn (Targs...)>::ptr(
			new FunctionSlot<Treturn, Targs...>(slot));
	}

	/**
	 * @brief      Create slot from function pointer
	 *
	 * @param[in]  slot       The function slot
	 *
	 * @tparam     Treturn  The slot return type
	 * @tparam     Targs    The slot arguments types
	 *
	 * @return     The created slot object
	 */
	template<typename Treturn, typename ...Targs>
	auto create(Treturn (*slot)(Targs...))
	{
		return create(std::function<Treturn (Targs...)>(slot));
	}

	/**
	 * @brief      Create slot from a signal
	 *
	 * @param[in]  sig      The signal to create a slot from
	 *
	 * @tparam     Treturn  The signal return type
	 *
	 * @return     The created slot object
	 *
	 * @details    Works only for signal with no arguments.
	 */
	template<typename Treturn>
	auto create(Signal<Treturn> & sig)
	{
		auto fn = std::bind(&Signal<Treturn>::emit, &sig);
		return create(std::function<Treturn ()>(fn));
	}

	/**
	 * @brief      Create slot from a signal
	 *
	 * @param[in]  sig      The signal to create a slot from
	 *
	 * @tparam     Treturn  The signal return type
	 * @tparam     T1       The first parameter type
	 *
	 * @return     The created slot object
	 *
	 * @details    Works only for signal with one argument.
	 */
	template<typename Treturn, typename T1>
	auto create(Signal<Treturn, T1> & sig)
	{
		using namespace std::placeholders;
		auto fn = std::bind(&Signal<Treturn, T1>::emit, &sig, _1);
		return create(std::function<Treturn (T1)>(fn));
	}

	/**
	 * @brief      Create slot from a signal
	 *
	 * @param[in]  sig      The signal to create a slot from
	 *
	 * @tparam     Treturn  The signal return type
	 * @tparam     T1       The first parameter type
	 * @tparam     T2       The second parameter type
	 *
	 * @return     The created slot object
	 *
	 * @details    Works only for signal with two arguments.
	 */
	template<typename Treturn, typename T1, typename T2>
	auto create(Signal<Treturn, T1, T2> & sig)
	{
		using namespace std::placeholders;
		auto fn = std::bind(&Signal<Treturn, T1, T2>::emit, &sig, _1, _2);
		return create(std::function<Treturn (T1, T2)>(fn));
	}

	/**
	 * @brief      Create slot from a signal
	 *
	 * @param[in]  sig      The signal to create a slot from
	 *
	 * @tparam     Treturn  The signal return type
	 * @tparam     T1       The first parameter type
	 * @tparam     T2       The second parameter type
	 * @tparam     T3       The third parameter type
	 *
	 * @return     The created slot object
	 *
	 * @details    Works only for signal with three arguments.
	 */
	template<typename Treturn, typename T1, typename T2, typename T3>
	auto create(Signal<Treturn, T1, T2, T3> & sig)
	{
		using namespace std::placeholders;
		auto fn = std::bind(&Signal<Treturn, T1, T2, T3>::emit, &sig, _1, _2, _3);
		return create(std::function<Treturn (T1, T2, T3)>(fn));
	}
	
	/**
	 * @brief      Create slot from a signal
	 *
	 * @param[in]  sig      The signal to create a slot from
	 *
	 * @tparam     Treturn  The signal return type
	 * @tparam     T1       The first parameter type
	 * @tparam     T2       The second parameter type
	 * @tparam     T3       The third parameter type
	 * @tparam     T4       The fourth parameter type
	 *
	 * @return     The created slot object
	 *
	 * @details    Works only for signal with four arguments.
	 */
	template<typename Treturn, typename T1, typename T2, typename T3, typename T4>
	auto create(Signal<Treturn, T1, T2, T3, T4> & sig)
	{
		using namespace std::placeholders;
		auto fn = std::bind(&Signal<Treturn, T1, T2, T3, T4>::emit, &sig, _1, _2, _3, _4);
		return create(std::function<Treturn (T1, T2, T3, T4)>(fn));
	}
	
	/**
	 * @brief      Create slot from a signal
	 *
	 * @param[in]  sig      The signal to create a slot from
	 *
	 * @tparam     Treturn  The signal return type
	 * @tparam     T1       The first parameter type
	 * @tparam     T2       The second parameter type
	 * @tparam     T3       The third parameter type
	 * @tparam     T4       The fourth parameter type
	 * @tparam     T5       The fith parameter type
	 *
	 * @return     The created slot object
	 *
	 * @details    Works only for signal with five arguments. To create a slot from a signal with
	 * more than five arguments you must add a `create` overload.
	 */
	template<typename Treturn, typename T1, typename T2, typename T3, typename T4, typename T5>
	auto create(Signal<Treturn, T1, T2, T3, T4, T5> & sig)
	{
		using namespace std::placeholders;
		auto fn = std::bind(&Signal<Treturn, T1, T2, T3, T4, T5>::emit, &sig, _1, _2, _3, _4, _5);
		return create(std::function<Treturn (T1, T2, T3, T4, T5)>(fn));
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
		return typename GenericSlot<Treturn (Targs...)>::ptr(
			new ClassMethodSlot<Tinstance, Treturn, Targs...>(instance, slot));
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
		return typename GenericSlot<Treturn (Targs...)>::ptr(
			new ClassMethodSlot<Tinstance, Treturn, Targs...>(instance, slot));
	}

}

#endif // TESEO_HAL_SIGNAL_H