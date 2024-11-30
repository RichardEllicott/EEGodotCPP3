#ifndef MACROS_H // header guard
#define MACROS_H

/*

PROPERTY EXPORT MACROS

a lot of boilerplate is require to show just one variable in C++ (compared to GDScript)

ie: 4-5 for the property declaration
at least 2 for the get/set (if using one liners)
3 lines in the _bind_methods function

so these macros reduce that

*/

// header file macro, to declare a property
#define DECLARE_PROPERTY(TYPE, NAME)      \
private:                                  \
	TYPE NAME;                            \
                                          \
public:                                   \
	void set_##NAME(const TYPE p_##NAME); \
	TYPE get_##NAME() const;

// header file macro, to declare a property
#define DECLARE_PROPERTY_AND_CREATE_GETTER_SETTER(TYPE, NAME) \
private:                                                      \
	TYPE NAME;                                                \
                                                              \
public:                                                       \
	void set_##NAME(const TYPE p_##NAME) { return NAME; };    \
	TYPE get_##NAME() const { NAME = p_##NAME; };

// cpp file macro, create the get/set functions
#define CREATE_GETTER_SETTER(CLASS, TYPE, NAME)     \
	TYPE CLASS::get_##NAME() const { return NAME; } \
	void CLASS::set_##NAME(const TYPE p_##NAME) { NAME = p_##NAME; }

// bindings macro
#define CREATE_CLASSDB_BINDINGS(CLASS, TYPE, NAME)                                \
	ClassDB::bind_method(D_METHOD("get_" #NAME), &CLASS::get_##NAME);             \
	ClassDB::bind_method(D_METHOD("set_" #NAME, "p_" #NAME), &CLASS::set_##NAME); \
	ADD_PROPERTY(PropertyInfo(Variant::TYPE, #NAME), "set_" #NAME, "get_" #NAME);

#endif