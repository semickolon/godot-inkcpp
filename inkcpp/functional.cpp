#include "functional.h"

#include "value.h"
#include "stack.h"
#include "string_table.h"

#ifdef INK_ENABLE_UNREAL
#include "InkVar.h"
#endif

#ifdef INK_ENABLE_GODOT
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;
#endif

namespace ink::runtime::internal
{
	template<>
	int32_t function_base::pop<int32_t>(basic_eval_stack* stack)
	{
		value val = stack->pop();
		inkAssert(val.type() == value_type::int32, "Type missmatch!");
		return val.get<value_type::int32>();
	}

	template<>
	const char* function_base::pop<const char*>(basic_eval_stack* stack)
	{
		value val = stack->pop();
		inkAssert(val.type() == value_type::string, "Type missmatch!");
		return val.get<value_type::string>().str;
	}

	template<>
	void function_base::push<int32_t>(basic_eval_stack* stack, const int32_t& v)
	{
		stack->push(value{}.set<value_type::int32>(v));
	}

	void function_base::push_string(basic_eval_stack* stack, const char* dynamic_string)
	{
		stack->push(value{}.set<value_type::string>(dynamic_string, true));
	}

	char* function_base::allocate(string_table& strings, size_t len)
	{
		return strings.create(len);
	}

	// Generate template implementations for all significant types

#ifdef INK_ENABLE_STL
	template<>
	std::string function_base::pop<std::string>(basic_eval_stack* stack) {
		return std::string(pop<const char*>(stack));
	}
#endif
#ifdef INK_ENABLE_UNREAL
	SUPPORT_TYPE_PARAMETER_ONLY(FString);

	template<>
	FInkVar function_base::pop<FInkVar>(basic_eval_stack* stack)
	{
		value v = stack->pop();
		switch (v.type())
		{
		case value_type::null:
		case value_type::divert:
			inkFail("Trying to pass null or divert as ink parameter to external function");
			break;
		case value_type::integer:
			return FInkVar(v.get<int>());
		case value_type::decimal:
			return FInkVar(v.get<float>());
		case value_type::string:
			return FInkVar(v.get<FString>());
		}

		return FInkVar();
	}

	template<>
	void function_base::push<FInkVar>(basic_eval_stack* stack, const FInkVar& value)
	{
		switch (value.type)
		{
		case EInkVarType::None:
			{
				internal::value v;
				stack->push(v);
			}
			break;
		case EInkVarType::Int:
			stack->push(value.intVar);
			break;
		case EInkVarType::Float:
			stack->push(value.floatVar);
			break;
		case EInkVarType::String:
			inkFail("NOT IMPLEMENTED"); // TODO: String support
			return;
		}
	}
#endif
#ifdef INK_ENABLE_GODOT
	template<>
	Variant function_base::pop<Variant>(basic_eval_stack* stack)
	{
		value v = stack->pop();

		switch (v.type())
		{
		case value_type::null:
			break;
		case value_type::divert:
			UtilityFunctions::printerr("InkCPP: Cannot use divert as external function return value");
			break;
		case value_type::int32:
			return Variant(v.get<value_type::int32>());
		case value_type::float32:
			return Variant(v.get<value_type::float32>());
		case value_type::boolean:
			return Variant(v.get<value_type::boolean>());
		case value_type::string:
			return Variant(v.get<value_type::string>());
		}

		return Variant();
	}

	template<>
	void function_base::push<Variant>(basic_eval_stack* stack, const Variant& v)
	{
		switch (v.get_type())
		{
		case Variant::Type::NIL:
			stack->push(value{});
			break;
		case Variant::Type::INT:
			stack->push(internal::value{}.set<value_type::int32>(static_cast<int32_t>(v)));
			break;
		case Variant::Type::FLOAT:
			stack->push(internal::value{}.set<value_type::float32>(static_cast<float_t>(v)));
			break;
		case Variant::Type::BOOL:
			stack->push(internal::value{}.set<value_type::boolean>(static_cast<bool>(v)));
			break;
		case Variant::Type::STRING:
		case Variant::Type::STRING_NAME:
			UtilityFunctions::printerr("InkCPP: Direct conversion of strings from Godot to Ink is not supported. Use `allocate` and `push_string` instead.");
			return;
		}
	}

	void function_array_callable::call(basic_eval_stack* stack, size_t length, string_table& strings)
	{
		Array args;
		
		for (size_t i = 0; i < length; i++) {
			args.append(pop<Variant>(stack));
		}

		Variant result = _callable.callv(args);
		Variant::Type result_type = result.get_type();

		if (result_type == Variant::Type::STRING ||
			result_type == Variant::Type::STRING_NAME)
		{
			CharString cs = static_cast<String>(result).ascii();
			int len = cs.length();
			const char *src = cs.get_data();
			char* buffer = allocate(strings, len + 1);
			
			// Copy
			char* ptr = buffer;
			while (*src != '\0')
				*(ptr++) = *(src++);
			*ptr = 0;

			// push string result
			push_string(stack, buffer);
		} else {
			push(stack, result);
		}
	}
#endif
}
