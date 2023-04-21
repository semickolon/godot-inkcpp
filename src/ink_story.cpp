#include "ink_story.h"

#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

InkStory::InkStory()
{
	UtilityFunctions::print("Hello world from InkCPP GDExtension");
}

InkStory::~InkStory()
{
}

void InkStory::_bind_methods() {
}
