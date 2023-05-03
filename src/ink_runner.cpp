#include "ink_runner.h"

#include <runner.h>
#include <choice.h>

#include <godot_cpp/variant/utility_functions.hpp>

InkRunner::InkRunner() {
	_force_ended = false;
	_clear();
}

InkRunner::~InkRunner() {}

void InkRunner::_initialize(Ref<InkStory> ink_story, ink::runtime::runner runner) {
	_ink_story = ink_story;
	_runner = runner;
	_global_tags = get_tags_at_path("");
}

Ref<InkRunner> InkRunner::from_ink_story(Ref<InkStory> ink_story, bool new_globals) {
	auto ink_runner = Ref<InkRunner>();

	if (!ink_story.is_valid()) {
		UtilityFunctions::printerr("InkCPP: Attempted to create InkRunner from InkStory but the latter is invalid. Returning null.");
		return ink_runner;
	}

	auto runner = ink_story->create_runner(new_globals);
	ink_runner.instantiate();
	ink_runner->_initialize(ink_story, runner);
	return ink_runner;
}

void InkRunner::_clear() {
	_text = "";
	_tags = Array();
	_global_tags = Array();
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

Ref<InkStory> InkRunner::get_ink_story() {
	return _ink_story;
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
	return _runner->can_continue() && !_force_ended;
}

bool InkRunner::has_choices() {
	return _runner->has_choices() && !_force_ended;
}

Array InkRunner::get_choices() {
	Array choices = Array();

	if (_force_ended)
		return choices;

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
	if (_force_ended)
		return;

	if (idx >= 0 && idx < _runner->num_choices()) {
		_runner->choose(idx);
	} else {
		UtilityFunctions::printerr("InkCPP: Invalid choice index ignored.");
	}
}

bool InkRunner::move_to_path(String path) {
	if (_force_ended)
		return false;
	return _runner->move_to(_hash(path));
}

void InkRunner::bind_external_function(String ink_func_name, Callable fn) {
	if (_force_ended)
		return;
	_runner->bind_callable(ink_func_name, fn);
}

Array InkRunner::get_tags_at_path(String path) {
	// TODO: Can we dispose temp_runner?
	ink::runtime::runner temp_runner = _ink_story->create_runner(true);

	// If path.is_empty(), we're not moving to a different path and so we get the global tags
	if (temp_runner->move_to(_hash(path)) || path.is_empty()) {
		temp_runner->getline();
		return _get_tags(temp_runner);
	} else {
		UtilityFunctions::printerr("InkCPP: Attempted to get tags at path '" + path + "' but not found. Returning empty array.");
		return Array();
	}
}

void InkRunner::force_end() {
	_force_ended = true;
	_clear();
	emit_signal("ink_ended");
}

void InkRunner::_bind_methods() {
	ClassDB::bind_static_method("InkRunner", D_METHOD("from_ink_story", "ink_story", "new_globals"), &InkRunner::from_ink_story);

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
	ClassDB::bind_method(D_METHOD("force_end"), &InkRunner::force_end);

	ADD_SIGNAL(MethodInfo("ink_advanced", PropertyInfo(Variant::STRING, "text"), PropertyInfo(Variant::ARRAY, "tags")));
	ADD_SIGNAL(MethodInfo("ink_choices_set", PropertyInfo(Variant::ARRAY, "choices")));
	ADD_SIGNAL(MethodInfo("ink_ended"));
}
