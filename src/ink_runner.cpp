#include "ink_runner.h"

#include <runner.h>
#include <choice.h>

#include <godot_cpp/variant/utility_functions.hpp>

InkRunner::InkRunner() {
	_clear();
}

InkRunner::~InkRunner() {
	// TODO: How to dispose internal runner?
}

void InkRunner::_clear() {
	ink_story = Ref<InkStory>();
	// TODO: dispose previous runner?
    _runner = ink::runtime::runner(); 
    _global_tags = Array();
    _text = "";
    _tags = Array();
}

Array InkRunner::_get_tags(ink::runtime::runner runner) {
	Array tags = Array();

	for (int i = 0; i < runner->num_tags(); ++i) {
		tags.append(String(runner->get_tag(i)));
	}

	return tags;
}

ink::hash_t InkRunner::_hash(String path) {
	const char *p = path.ascii().get_data();
	return ink::hash_string(p);
}

void InkRunner::set_ink_story(Ref<InkStory> ink_story) {
	_clear();
	
	if (!ink_story.is_valid()) {
		UtilityFunctions::print("InkCPP: Attempted to assign invalid InkStory to InkRunner.");
		return;
	}

	this->ink_story = ink_story;
	_runner = ink_story->_create_runner();
	_global_tags = get_tags_at_path("");
}

Ref<InkStory> InkRunner::get_ink_story() {
	return ink_story;
}

String InkRunner::advance() {
	if (can_continue()) {
		_text = String(_runner->getline().c_str());
		_tags = _get_tags(_runner);

		emit_signal("ink_advanced", _text, _tags);
		if (has_choices()) {
			emit_signal("ink_choices_set", get_choices());
		}

		return _text;
	} else if (!_runner->has_choices()) {
		emit_signal("ink_ended");
	}

	return "";
}

bool InkRunner::can_continue() {
	return _runner->can_continue();
}

bool InkRunner::has_choices() {
	return _runner->has_choices();
}

Array InkRunner::get_choices() {
	Array choices = Array();

	for (int i = 0; i < _runner->num_choices(); ++i) {
		const ink::runtime::choice *ch = _runner->get_choice(i);
		choices.append(String(ch->text()));
	}

	return choices;
}

bool InkRunner::has_tags() {
	return _tags.size() > 0;
}

Array InkRunner::get_tags() {
	return _tags;
}

Array InkRunner::get_global_tags() {
	return _global_tags;
}

void InkRunner::choose_choice_index(int idx) {
	if (idx >= 0 && idx < _runner->num_choices()) {
		_runner->choose(idx);
	} else {
		UtilityFunctions::printerr("InkCPP: Invalid choice index ignored.");
	}
}

bool InkRunner::move_to_path(String path) {
	return _runner->move_to(_hash(path));
}

void InkRunner::bind_external_function(String ink_func_name, Callable fn) {
	_runner->bind_callable(ink_func_name, fn);
}

Array InkRunner::get_tags_at_path(String path) {
	// TODO: Can we dispose temp_runner?
	ink::runtime::runner temp_runner = ink_story->_create_runner();

	// If path.is_empty(), we're not moving to a different path and so we get the global tags
	if (temp_runner->move_to(_hash(path)) || path.is_empty()) {
		temp_runner->getline();
		return _get_tags(temp_runner);
	} else {
		UtilityFunctions::printerr("InkCPP: Attempted to get tags at path '" + path + "' but not found. Returning empty array.");
		return Array();
	}
}

void InkRunner::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_ink_story", "ink_story"), &InkRunner::set_ink_story);
	ClassDB::bind_method(D_METHOD("get_ink_story"), &InkRunner::get_ink_story);

	ClassDB::bind_method(D_METHOD("advance"), &InkRunner::advance);
	ClassDB::bind_method(D_METHOD("can_continue"), &InkRunner::can_continue);
	ClassDB::bind_method(D_METHOD("has_choices"), &InkRunner::has_choices);
	ClassDB::bind_method(D_METHOD("get_choices"), &InkRunner::get_choices);
	ClassDB::bind_method(D_METHOD("has_tags"), &InkRunner::has_tags);
	ClassDB::bind_method(D_METHOD("get_tags"), &InkRunner::get_tags);
	ClassDB::bind_method(D_METHOD("get_global_tags"), &InkRunner::get_global_tags);
	ClassDB::bind_method(D_METHOD("choose_choice_index", "idx"), &InkRunner::choose_choice_index);
	ClassDB::bind_method(D_METHOD("move_to_path", "path"), &InkRunner::move_to_path);
	ClassDB::bind_method(D_METHOD("bind_external_function", "ink_func_name", "fn"), &InkRunner::bind_external_function);
	ClassDB::bind_method(D_METHOD("get_tags_at_path", "path"), &InkRunner::get_tags_at_path);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "ink_story"), "set_ink_story", "get_ink_story");

	ADD_SIGNAL(MethodInfo("ink_advanced", PropertyInfo(Variant::STRING, "text"), PropertyInfo(Variant::ARRAY, "tags")));
	ADD_SIGNAL(MethodInfo("ink_choices_set", PropertyInfo(Variant::ARRAY, "choices")));
	ADD_SIGNAL(MethodInfo("ink_ended"));
}
