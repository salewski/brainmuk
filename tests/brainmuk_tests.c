#include <string.h>
#include <unistd.h>

#include "greatest.h"

#include <bf_alloc.h>
#include <bf_slurp.h>

static const uint8_t X86_RET = 0xC3;
static const uint8_t X86_NOP = 0x90;

/********************* tests for slurp() and unslurp() *********************/

TEST normal_file_can_be_slurped_and_unslurped() {
#define test_filename __FILE__ ".fixtures/normal"
    char *str = slurp(test_filename);

    ASSERTm("Could not slurp file: " test_filename , str != NULL);

    ASSERT_STR_EQm("Unexpected file contents for " test_filename,
           "Hi, I'm a normal file.\n",
           str);

    ASSERTm("Could not unslurp " test_filename, unslurp(str));

    PASS();
#undef test_filename
}

/******************* tests for allocate_executable_space *******************/

TEST space_returned_can_be_executed_and_freed(void) {
    uint8_t* space = allocate_executable_space(1);

    ASSERT_OR_LONGJMPm("Allocation failed", space != NULL);

    /* Writing a single return instruction creates an executable function. */
    space[0] = X86_RET;

    /* Call the function. */
    ((void (*)(void)) space)();

    /* At this point, a seg fault would indicate failure. */

    /* Otherwise, deallocate! */
    ASSERTm("Could not free space", free_executable_space(space));

    PASS();
}

TEST space_returned_is_given_size(void) {
    /* Allocate some biggish size of executable memory (bigger than the size
     * of one page, at least). */
    size_t base_size = getpagesize() * 10 + 1;
    size_t size = base_size + 1;
    uint8_t* space = allocate_executable_space(size);

    ASSERT_OR_LONGJMPm("Allocation failed", space != NULL);

    /* Write a nop-sled! */
    memset(space, X86_NOP, base_size);
    /* at the end of the nop-sled, we'll return. */
    space[size - 1] = X86_RET;

    /* Call the function. */
    ((void (*)(void)) space)();

    /* At this point, a segmentation fault would indicate failure. */

    /* Otherwise, deallocate! */
    ASSERTm("Could not free space", free_executable_space(space));

    PASS();
}

/********************************** main() **********************************/

/* Add all the definitions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();      /* command-line arguments, initialization. */

    RUN_TEST(space_returned_can_be_executed_and_freed);
    RUN_TEST(space_returned_is_given_size);

    RUN_TEST(normal_file_can_be_slurped_and_unslurped);

    GREATEST_MAIN_END();        /* display results */
}
