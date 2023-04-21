#ifndef INK_STORY_H
#define INK_STORY_H

#include <godot_cpp/classes/ref.hpp>

using namespace godot;

class InkStory : public RefCounted {
	GDCLASS(InkStory, RefCounted)

protected:
    static void _bind_methods();

public:
    InkStory();
    ~InkStory();

};

#endif // INK_STORY_H
