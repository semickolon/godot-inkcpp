#include <story.h>
#include <runner.h>
#include <compiler.h>
#include <choice.h>

int main(int argc, const char **argv) {
    ink::compiler::compilation_results *results;

    ink::compiler::run(argv[1], argv[2], results);
	
    try
	{
		using namespace ink::runtime;

		// Load story
		story* myInk = story::from_file(argv[2]);

		// Start runner
		runner thread = myInk->new_runner();

		while (true)
		{
			while (thread->can_continue())
				std::cout << thread->getline();

			if (thread->has_tags()){
				std::cout << "# tags: ";
				for (int i = 0; i < thread->num_tags(); ++i) {
					if(i != 0) std::cout << ", ";
					std::cout << thread->get_tag(i);
				}
				std::cout << std::endl;
			}
			if (thread->has_choices())
			{
				// Extra end line
				std::cout << std::endl;

				int index = 1;
				for (const ink::runtime::choice& c : *thread)
				{
					std::cout << index++ << ": " << c.text() << std::endl;
				}

				int c = 0;
				std::cin >> c;
				thread->choose(c - 1);
				std::cout << "?> ";
				continue;
				continue;
			}

			// out of content
			break;
		}

		return 0;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Unhandled ink runtime exception: " << e.what() << std::endl;
		return 1;
	}
}
