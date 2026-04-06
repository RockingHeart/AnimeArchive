#pragma once
#include "type_traits"

template <class, bool>
struct FunPar {
	using Type = void;
};

template <class ParType>
struct FunPar<ParType, false> {
	using Type = ParType;
};

template <class ParType>
struct FunPar<ParType, true> {
	using Type = std::add_lvalue_reference<ParType>;
};