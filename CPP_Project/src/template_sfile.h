#ifndef TEMPLATE_SFILE_H
#define TEMPLATE_SFILE_H

#include <macros.h> // my macros to help declare properties
#include <helper.h> // includes a print function

#include <godot_cpp/classes/sprite2d.hpp>

#include <godot_cpp/classes/random_number_generator.hpp>


using namespace godot;

class TemplateSFile : public Sprite2D
{
    GDCLASS(TemplateSFile, Sprite2D)

    // MACROS from macros.h
    // DECLARE_PROPERTY(bool, enabled) // we need to also add two more lines to the cpp file per a property we want to @export
    // DECLARE_PROPERTY(float, speed)

    //
    //
    // DECLARE_PROPERTY_AND_CREATE_GETTER_SETTER_EXPERIMENTAL(bool, enabled) // doesn't crash alone
    // DECLARE_PROPERTY_AND_CREATE_GETTER_SETTER_EXPERIMENTAL(float, speed)

    DECLARE_PROPERTY_AND_CREATE_GETTER_SETTER_EXPERIMENTAL(Ref<Texture2D>, texture222) // ah use ref!
    // DECLARE_PROPERTY_AND_CREATE_GETTER_SETTER_EXPERIMENTAL(Ref<RandomNumberGenerator>, rng) // ah use ref!

    

private:
protected:
    static void _bind_methods()
    {
        // CREATE_CLASSDB_BINDINGS(TemplateSFile, BOOL, enabled); // just crashes with signal or not
        // CREATE_CLASSDB_BINDINGS(TemplateSFile, Variant::FLOAT, speed)

        CREATE_CLASSDB_BINDINGS2(TemplateSFile, "Texture2D", texture222)
        // CREATE_CLASSDB_BINDINGS2(TemplateSFile, "RandomNumberGenerator", rng)


        #pragma region SIGNALS
    	// ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed", PROPERTY_HINT_RANGE, "0,20,0.01"), "set_speed", "get_speed");
        // ADD_SIGNAL(MethodInfo("position_changed", PropertyInfo(Variant::OBJECT, "node"), PropertyInfo(Variant::VECTOR2, "new_pos")));
        #pragma endregion
    }

public:
    TemplateSFile()
    {
        print("hello from single file template!");
    };
    ~TemplateSFile()
    {
        print("bye bye from single file template!");
    };

    void _ready() override;
    void _process(double delta) override;
    void _physics_process(double delta) override;
    void _draw() override;
};

#endif