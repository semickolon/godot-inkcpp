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
    Ref<InkStory> _ink_story;
    ink::runtime::runner _runner;
    Array _global_tags;
    String _text;
    Array _tags;
    bool _force_ended;

    void _initialize(Ref<InkStory> _ink_story, ink::runtime::runner runner);
    void _clear();
    static Array _get_tags(ink::runtime::runner runner);
    ink::hash_t _hash(String path);

protected:
    static void _bind_methods();

public:
    InkRunner();
    ~InkRunner();

    static Ref<InkRunner> from_ink_story(Ref<InkStory> ink_story, bool new_globals);

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
    Array get_tags_at_path(String path);
    void force_end();

};

#endif // INK_RUNNER_H
