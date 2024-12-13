#ifndef MACROS_H // header guard
#define MACROS_H

/*

WARNING

MY MACROS HAVE TOO SIMILAR NAMES!!!


NOTE
a good practise in C++ may be to remove the semicolons from these macros, this will create an error if we don't call them with an semicolon

*/

#define DECLARE_PROPERTY(TYPE, NAME)      \
                                          \
public:                                   \
	void set_##NAME(const TYPE p_##NAME); \
	TYPE get_##NAME() const;              \
                                          \
private:                                  \
	TYPE NAME;

// EXPERIMENTAL (putting the functions on the header file, breaks!!!)
// // header file macro, to declare a property (supports only variants)
// // this all in one macro changes the public/private scope

//
//
// i think const causes problem!!

// note ending has no ;
// allows:
//
// DECLARE_PROPERTY_SINGLE_FILE(float, pulse_width) = 0.5;
//
#define DECLARE_PROPERTY_SINGLE_FILE(TYPE, NAME) \
public:                                                                    \
	TYPE get_##NAME() { return NAME; };                                    \
	void set_##NAME(const TYPE p_##NAME) { NAME = p_##NAME; };             \
                                                                           \
private:                                                                   \
	TYPE NAME;



#define DECLARE_PROPERTY_SINGLE_FILE_DEFAULT(TYPE, NAME, DEFAULT) \
public:                                                                    \
	TYPE get_##NAME() { return NAME; };                                    \
	void set_##NAME(const TYPE p_##NAME) { NAME = p_##NAME; };             \
                                                                           \
private:                                                                   \
	TYPE NAME = DEFAULT;



// cpp file macro, create the get/set functions for @export
//
// no semicolons are required for this as it's on the cpp body and these are the functions
//
#define CREATE_GETTER_SETTER(CLASS, TYPE, NAME)     \
	TYPE CLASS::get_##NAME() const { return NAME; } \
	void CLASS::set_##NAME(const TYPE p_##NAME) { NAME = p_##NAME; }




// COPY
// bindings macro
// run inside _bind_methods on the cpp file
// works only for variants and needs you to find a special constant for each type (like BOOL, FLOAT, VECTOR2I)
//
// Examples:
// CREATE_CLASSDB_BINDINGS(MeshGenerator, BOOL, enabled)
// CREATE_CLASSDB_BINDINGS(MeshGenerator, FLOAT, height)
// CREATE_CLASSDB_BINDINGS(MeshGenerator, VECTOR2I, grid_size)
//
#define CREATE_VAR_BINDINGS(CLASS, TYPE, NAME)                                \
	ClassDB::bind_method(D_METHOD("get_" #NAME), &CLASS::get_##NAME);             \
	ClassDB::bind_method(D_METHOD("set_" #NAME, "p_" #NAME), &CLASS::set_##NAME); \
	ADD_PROPERTY(PropertyInfo(Variant::TYPE, #NAME), "set_" #NAME, "get_" #NAME);

// this one works for clases, like Texture2D etc
//
// BIND_CLASSDB_PROPERTY2(MeshGenerator, "Texture2D", texture2d);
//
// i have only tested Texture2D so far
//
//
#define CREATE_CLASS_BINDINGS(CLASS, TYPE, NAME)                         \
	ClassDB::bind_method(D_METHOD("set_" #NAME, TYPE), &CLASS::set_##NAME); \
	ClassDB::bind_method(D_METHOD("get_" #NAME), &CLASS::get_##NAME);       \
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, #NAME, PROPERTY_HINT_RESOURCE_TYPE, TYPE), "set_" #NAME, "get_" #NAME);





#endif