#ifndef INK_STORY_H
#define INK_STORY_H

#include <story.h>

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/file_access.hpp>

using namespace godot;

class InkStory : public RefCounted {
	GDCLASS(InkStory, RefCounted)

private:
    ink::runtime::story *_story;

    void _clear_story();

protected:
    static void _bind_methods();

public:
    InkStory();
    ~InkStory();

    Error load_from_file(Ref<FileAccess> file);

    ink::runtime::runner _create_runner();
};

#endif // INK_STORY_H
