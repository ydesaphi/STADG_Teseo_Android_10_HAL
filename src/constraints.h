#ifndef TESEO_HAL_TEMPLATE_CONSTRAINTS
#define TESEO_HAL_TEMPLATE_CONSTRAINTS

template<class T, class B>
struct DerivedFrom {
	static void constraints(T* p) { B* pb = p; (void)(pb); }
	DerivedFrom() { void(*p)(T*) = constraints; (void)(p); }
};

template<class T1, class T2>
struct CanCopy {
	static void constraints(T1 a, T2 b) { T2 c = a; b = a; }
	CanCopy() { void(*p)(T1,T2) = constraints; (void)(p); }
};

template<class T1, class T2 = T1>
struct CanCompare {
	static void constraints(T1 a, T2 b) { a==b; a!=b; a<b; }
	CanCompare() { void(*p)(T1,T2) = constraints; (void)(p); }
};

template<class T1, class T2, class T3 = T1>
struct CanMultiply {
	static void constraints(T1 a, T2 b, T3 c) { c = a*b; }
	CanMultiply() { void(*p)(T1,T2,T3) = constraints; (void)(p); }
};

#endif // TESEO_HAL_TEMPLATE_CONSTRAINTS