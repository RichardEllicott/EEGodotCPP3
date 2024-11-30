#ifndef TEMPLATE_CPP
#define TEMPLATE_CPP

#include <template.h>

#include <macros.h>
#include <helper.h>

using namespace godot;

Template::Template(){
    enabled = false;
    speed = 35.7;
}

Template::~Template(){
    
}


void Template::_process(double delta){

}

void Template::_draw(){

}

void Template::_physics_process(double delta){

}

// macros from macros.h
CREATE_GETTER_SETTER(Template, bool, enabled)
CREATE_GETTER_SETTER(Template, float, speed)


void Template::_bind_methods()
{
	// macros from macros.h
	CREATE_CLASSDB_BINDINGS(Template, BOOL, enabled)
	CREATE_CLASSDB_BINDINGS(Template, FLOAT, speed)

}

#endif