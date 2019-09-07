#include <errno.h>
#include <getopt.h>
#include <glob.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define eprintf(...) fprintf (stderr, "ERROR: "    __VA_ARGS__); app_help ()
#define iprintf(...) fprintf (stderr, "INFO: "     __VA_ARGS__)
#define wprintf(...) fprintf (stderr, "WARNING: "  __VA_ARGS__)

#define EINVALID_ARGS -1

#define START_AT_MAX UINT_MAX
#define START_AT_MIN 0

enum sort_type {
    ORIGINAL_ORDER,
    ALPHABETICAL_ORDER,
    DATE_ORDER
};

// TODO: allow option to reverse ordering

static void app_help () {
    printf ("app_help ()\n");
    // TODO: Proper app help
}

int main (int argc, char *argv[]) {
    int opt;
    char *start_at_optarg_end;

    int start_at = 1;
    int sort_method = ORIGINAL_ORDER;

    static struct option long_options[] =  {
        {"date",       no_argument,       0, 'd' },
        {"alphabet",   no_argument,       0, 'a' },
        {"start_at",   required_argument, 0, 's' },
        {0,            0,                 0, 0   }
    };

    while ((opt = getopt_long(argc, argv, "das:", long_options, NULL)) != -1) {
        switch (opt)
            {
            case 'd':
                sort_method = DATE_ORDER;
                break;
            case 'a':
                sort_method = ALPHABETICAL_ORDER;
                break;
            case 's':
                start_at  = strtoul (optarg, &start_at_optarg_end, 10);

                // Throw error if opt arg is not a number
                int optarg_len = strlen (optarg);
                if (start_at_optarg_end != optarg + optarg_len) {
                    eprintf ("argument '%s' is not a valid number.\n", optarg);
                    return EINVALID_ARGS;
                }

                // Throw error if opt arg is less than START_AT_MIN or
                // greater than START_AT_MAX (or if it causes a range
                // error in strtoul ())
                if (errno == ERANGE || start_at < START_AT_MIN || start_at > START_AT_MAX) {
                    eprintf ("'start at' value '%s' is out of range.\n", optarg);
                    return EINVALID_ARGS;
                }

                iprintf ("File numbering will start at %d\n", start_at);
                break;
            case '?':
            default:
                app_help ();
                return EINVALID_ARGS;
            }
    }

    switch (sort_method) {
    case (ORIGINAL_ORDER):
        iprintf ("Files will not be sorted (wildcards will be expanded in alphabetical order).\n");
        break;
    case (ALPHABETICAL_ORDER):
        iprintf ("Files will be sorted alphabetically.\n");
        break;
    case (DATE_ORDER):
        iprintf ("Files will be sorted by date.\n");
        break;
    default:
        eprintf ("Invalid sorting method\n");
        return EINVALID_ARGS;
    }

    // Calculate number of additional arguments we have that represent
    // files
    int num_extra_args = argc - optind;
    if (num_extra_args < 1) {
        eprintf("We require some files to be enumerated!\n");
        return EINVALID_ARGS;
    }

    // Expand any wildcards (glob () automatically expands these in
    // alphabetical order) and store the list of all files in a
    // glob. Note that, depending on the shell, the wildcards may have
    // been handled already.

    glob_t globbuf;

    for (int i = optind; i < argc; i++) {
        int result = glob (argv[i], GLOB_MARK | (i > optind ? GLOB_APPEND : 0), NULL, &globbuf);
        if (result != 0) {
            char error_reason[50];
            switch (result) {
            case GLOB_ABORTED:
                sprintf (error_reason, "a read error occurred");
                break;
            case GLOB_NOMATCH:
                sprintf (error_reason, "no matching files were found");
                break;
            default:
                sprintf (error_reason, "an error occurred");
                break;
            }

            wprintf ("Problem accessing file/file pattern '%s', as %s.\n", argv[i], error_reason);
        }
    }

    // For now, just print out the list of files
    for (int i = 0; i < globbuf.gl_pathc; i++) {
        printf ("%s\n", globbuf.gl_pathv[i]);
    }

    // TODO: Remove duplicated files in list (determined by their
    // inodes/device numbers)

    // TODO: Ignore files that are symlinks

    // TODO: Ignore directories

    // TODO: Check final value is not as large as UINT_MAX

    globfree (&globbuf);

    return 0;
}
