//====================================================================================
//     The MIT License (MIT)
//
//     Copyright (c) 2011 Kapparock LLC
//
//     Permission is hereby granted, free of charge, to any person obtaining a copy
//     of this software and associated documentation files (the "Software"), to deal
//     in the Software without restriction, including without limitation the rights
//     to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//     copies of the Software, and to permit persons to whom the Software is
//     furnished to do so, subject to the following conditions:
//
//     The above copyright notice and this permission notice shall be included in
//     all copies or substantial portions of the Software.
//
//     THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//     IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//     FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//     AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//     LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//     OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//     THE SOFTWARE.
//====================================================================================
#ifndef _NOTIFICATION_H
#define _NOTIFICATION_H
#include <functional>
#include <string>
#include <map>
#include <typeindex>
#include <memory>
namespace kapi
{
	namespace Notification
	{
	}

	namespace notify
	{
	using namespace std;

	//namespace {
	template<class F>
	struct function_traits;

	// function pointer
	template<class R, class... Args>
	struct function_traits<R(*)(Args...)> : public function_traits<R(Args...)>
	{};

	template<class R, class... Args>
	struct function_traits<R(Args...)>
	{
		using return_type = R;
		static constexpr std::size_t arity = sizeof...(Args);
		using wrapper = std::function<void(Args...)>;
		template <std::size_t N>
		struct argument
		{
			static_assert(N < arity, "error: invalid parameter index.");
			using type = typename std::tuple_element<N,std::tuple<Args...>>::type;
		};
	};
	// member function pointer
	template<class C, class R, class... Args>
	struct function_traits<R(C::*)(Args...)> : public function_traits<R(Args...)>
	{};

	// const member function pointer
	template<class C, class R, class... Args>
	struct function_traits<R(C::*)(Args...) const> : public function_traits<R(Args...)>
	{};

	template<class F>
	struct function_traits
	{
	private:
		using call_type = function_traits<decltype(&F::operator())>;
	public:
		using return_type = typename call_type::return_type;
		static constexpr std::size_t arity = call_type::arity;
		using wrapper = typename call_type::wrapper;
		template <std::size_t N>
		struct argument
		{
			static_assert(N < arity, "error: invalid parameter index.");
			using type = typename call_type::template argument<N>::type;
		};
	};

	template<class F>
	struct function_traits<F&> : public function_traits<F> {};

	template<class F>
	struct function_traits<F&&> : public function_traits<F> {};

	//==============================================================
	struct __Function {	};

	template <typename T>
	struct BasicFunction : __Function
	{
		using wrapper = typename function_traits<T>::wrapper;
		wrapper function;
		BasicFunction(wrapper function) : function(function) { }
	};

	typedef map< type_index, unique_ptr<__Function>> Handlers;

	Handlers& __container(const string& e, const string& u);
	//}
	template <typename F>
	static void handler(const string& event, const string& uri, F function)
	{
		using wrapper = typename function_traits<F>::wrapper;
		Handlers& H = __container(event,uri);
		type_index index(typeid(wrapper));
		unique_ptr<__Function> func_ptr(new BasicFunction<F>(wrapper(function)));
		H[index] = std::move(func_ptr);
	}

	template <typename... Args>
	static void trigger(const string& event, const string& uri, Args&&... args)
	{
		const Handlers& H = __container(event,uri);
		using wrapper = typename function_traits<void(Args...)>::wrapper;
		std::type_index index(typeid(wrapper));

		Handlers::const_iterator i = H.lower_bound(index);
		Handlers::const_iterator j = H.upper_bound(index);
		for (;i!=j; ++i)
		{
			const __Function &f = *i->second;
			wrapper func =  static_cast<const BasicFunction<void(Args...)> &>(f).function;
			func(std::forward<Args>(args)...);
		}
	}

	template <typename F>
	void simpleHandler(const string& uri, F function)
	{
		using wrapper = typename function_traits<F>::wrapper;
		Handlers& H = __container("_",uri);
		type_index index(typeid(wrapper));
		unique_ptr<__Function> func_ptr(new BasicFunction<F>(wrapper(function)));
		H[index] = move(func_ptr);
	}

	template <typename... Args>
	void simpleTrigger(const string& uri, Args&&... args)
	{
		const Handlers& H = __container("_",uri);
		using wrapper = typename function_traits<void(Args...)>::wrapper;
		type_index index(typeid(wrapper));

		Handlers::const_iterator i = H.lower_bound(index);
		Handlers::const_iterator j = H.upper_bound(index);
		for (;i!=j; ++i)
		{
			const __Function &f = *i->second;
			wrapper func =  static_cast<const BasicFunction<void(Args...)> &>(f).function;
			func(forward<Args>(args)...);
		}
	}

	}// namespace notify
}
#endif
