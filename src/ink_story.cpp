#include "ink_story.h"

#include <runner.h>
#include <choice.h>

#include <godot_cpp/variant/utility_functions.hpp>

InkStory::InkStory() {}

InkStory::~InkStory() {
	delete _story;
}

Ref<InkStory> InkStory::open(Ref<FileAccess> file) {
	auto ink_story = Ref<InkStory>();

	if (!file.is_valid()) {
		UtilityFunctions::printerr("InkCPP: Attempted to load null file to InkStory.");
		return ink_story;
	}

	try {
		uint64_t file_len = file->get_length();
		unsigned char *file_data = new unsigned char[file_len];

		while (file->get_position() < file_len) {
			uint64_t file_pos = file->get_position();
			file_data[file_pos] = file->get_8();
		}

		ink_story.instantiate();
		ink_story->_story = ink::runtime::story::from_binary(file_data, file_len, true);
	} catch (const std::exception& e) {
		UtilityFunctions::printerr("InkCPP: Unhandled ink runtime exception: " + String(e.what()));
	}

	return ink_story;
}

ink::runtime::runner InkStory::create_runner(bool new_globals) {
	if (new_globals) {
		return _story->new_runner();
	} else {
		if (_globals == nullptr) {
			_globals = _story->new_globals();
		}
		return _story->new_runner(_globals);
	}
}

void InkStory::_bind_methods() {
	ClassDB::bind_static_method("InkStory", D_METHOD("open", "file"), &InkStory::open);
}
