#ifndef INK_RUNNER_H
#define INK_RUNNER_H

#include "ink_story.h"

#include <story.h>
#include <runner.h>
#include <choice.h>

#include <godot_cpp/classes/ref.hpp>

using namespace godot;

class InkRunner : public RefCounted {
	GDCLASS(InkRunner, RefCounted)

private:
    ink::runtime::runner _runner;
    Array _globalTags;
    String _text;
    Array _tags;

    void _clear();
    static Array _get_tags(ink::runtime::runner runner);

protected:
    static void _bind_methods();

public:
    Ref<InkStory> ink_story;

    InkRunner();
    ~InkRunner();

    void set_ink_story(Ref<InkStory> value);
    Ref<InkStory> get_ink_story();

    String advance();
    bool can_continue();
    bool has_choices();
    Array get_choices();
    bool has_tags();
    Array get_tags();
    Array get_global_tags();
    void choose_choice_index(int index);
    bool move_to_path(String path);
    void bind_external_function(String ink_func_name, Callable fn);

};

#endif // INK_RUNNER_H
