#include "ink_story.h"

#include <runner.h>
#include <choice.h>

#include <godot_cpp/variant/utility_functions.hpp>

InkStory::InkStory() {
	_story = nullptr;
}

InkStory::~InkStory() {
	_clear_story();
}

void InkStory::_clear_story() {
	if (_story != nullptr) {
		delete _story;
		_story = nullptr;
	}
}

Error InkStory::load_from_file(Ref<FileAccess> file) {
	if (!file.is_valid()) {
		UtilityFunctions::printerr("InkCPP: Attempted to load null file to InkStory.");
		return Error::ERR_FILE_NOT_FOUND;
	}

	_clear_story();

	try {
		uint64_t file_len = file->get_length();
		unsigned char *file_data = new unsigned char[file_len];

		while (file->get_position() < file_len) {
			uint64_t file_pos = file->get_position();
			file_data[file_pos] = file->get_8();
		}

		_story = ink::runtime::story::from_binary(file_data, file_len, true);
	} catch (const std::exception& e) {
		UtilityFunctions::printerr("InkCPP: Unhandled ink runtime exception: " + String(e.what()));
		return Error::FAILED;
	}

	return Error::OK;
}

bool InkStory::is_file_loaded() {
	return _story != nullptr;
}

ink::runtime::runner InkStory::_create_runner() {
	if (_story == nullptr) {
		UtilityFunctions::printerr("InkCPP: Attempted to create runner but story has not been loaded yet. Creating null runner instead.");
		return ink::runtime::runner();
	}
	return _story->new_runner();
}

void InkStory::_bind_methods() {
	ClassDB::bind_method(D_METHOD("load_from_file", "file"), &InkStory::load_from_file);
	ClassDB::bind_method(D_METHOD("is_file_loaded"), &InkStory::is_file_loaded);
}
