#pragma once
// Lang::CwC

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "parser.h"


/**
 * Enum representing different states of parsing command line arguments.
 */
enum class ParseState {
    DEFAULT,
    FLAG_F,
    FLAG_FROM,
    FLAG_LEN,
    FLAG_COL_TYPE,
    FLAG_COL_IDX_COL,
    FLAG_COL_IDX_OFF,
    FLAG_MISSING_IDX_COL,
    FLAG_MISSING_IDX_OFF
};

/**
 * Helper function to terminate with an error print if the given bool is not true.
 * @param test The bool
 */
void cli_assert(bool test) {
    if (!test) {
        printf("Unexpected command line arguments provided\n");
        exit(-1);
    }
}

/**
 * Asserts that the given ssize_t has not already been changed from -1, and then
 * parses the given c-style string as long to set it.
 * @param arg_loc The location of the ssize_t to work with
 * @param arg A string containing a long to parse
 */
void parse_size_t_arg(ssize_t* arg_loc, char* arg) {
    cli_assert(*arg_loc == -1);
    *arg_loc = atol(arg);
}

/**
 * Parses command line args given by argc and argv. Updates the given ssize_t pointers to
 * -1 for each arg that is not present on the command line (nullptr for file), or the value of that
 * command line argument.
 * @param argc, argv The command line arguments
 * @param file Pointer to result of parsing -f
 * @param start Pointer to result of parsing -from
 * @param len Pointer to result of parsing -len
 * @param col_type Pointer to result of parsing -print_col_type
 * @param col_idx_col, col_idx_off Pointer to result of parsing -print_col_idx
 * @param missing_idx_col, missing_idx_off Pointer to result of parsing -is_missing_idx
 */
void parse_args(int argc, char* argv[], char** file, ssize_t* start, ssize_t* len,
                ssize_t* col_type, ssize_t* col_idx_col, ssize_t* col_idx_off,
                ssize_t* missing_idx_col, ssize_t* missing_idx_off) {
    *file = nullptr;
    // -1 represents argument not provided
    *start = -1;
    *len = -1;
    *col_type = -1;
    *col_idx_col = -1;
    *col_idx_off = -1;
    *missing_idx_col = -1;
    *missing_idx_off = -1;

    ParseState state = ParseState::DEFAULT;

    for (int i = 1; i < argc; i++) {
        char* arg = argv[i];
        switch (state) {
            case ParseState::DEFAULT:
                if (strcmp(arg, "-f") == 0) {
                    state = ParseState::FLAG_F;
                } else if (strcmp(arg, "-from") == 0) {
                    state = ParseState::FLAG_FROM;
                } else if (strcmp(arg, "-len") == 0) {
                    state = ParseState::FLAG_LEN;
                } else if (strcmp(arg, "-print_col_type") == 0) {
                    state = ParseState::FLAG_COL_TYPE;
                } else if (strcmp(arg, "-print_col_idx") == 0) {
                    state = ParseState::FLAG_COL_IDX_COL;
                } else if (strcmp(arg, "-is_missing_idx") == 0) {
                    state = ParseState::FLAG_MISSING_IDX_COL;
                } else {
                    cli_assert(false);
                }
                break;
            case ParseState::FLAG_F:
                cli_assert(*file == nullptr);
                *file = arg;
                state = ParseState::DEFAULT;
                break;
            case ParseState::FLAG_FROM:
                parse_size_t_arg(start, arg);
                state = ParseState::DEFAULT;
                break;
            case ParseState::FLAG_LEN:
                parse_size_t_arg(len, arg);
                state = ParseState::DEFAULT;
                break;
            case ParseState::FLAG_COL_TYPE:
                parse_size_t_arg(col_type, arg);
                state = ParseState::DEFAULT;
                break;
            case ParseState::FLAG_COL_IDX_COL:
                parse_size_t_arg(col_idx_col, arg);
                state = ParseState::FLAG_COL_IDX_OFF;
                break;
            case ParseState::FLAG_COL_IDX_OFF:
                parse_size_t_arg(col_idx_off, arg);
                state = ParseState::DEFAULT;
                break;
            case ParseState::FLAG_MISSING_IDX_COL:
                parse_size_t_arg(missing_idx_col, arg);
                state = ParseState::FLAG_MISSING_IDX_OFF;
                break;
            case ParseState::FLAG_MISSING_IDX_OFF:
                parse_size_t_arg(missing_idx_off, arg);
                state = ParseState::DEFAULT;
                break;
            default:
                cli_assert(false);
        }
    }
    cli_assert(state == ParseState::DEFAULT);
}

/**
 * Gets the column at the given index from the given set. Terminates with an error print if which
 * is not valid.
 * @param set The ColumnSet to get from
 * @param which The column index to get
 */
BaseColumn* getColumnChecked(ColumnSet* set, size_t which) {
    if (which >= set->getLength()) {
        printf("No such column\n");
        exit(-1);
    }
    return set->getColumn(which);
}

/**
 * Terminates if the given column is not large enough to have the given entry index.
 * @param col The column
 * @param which The entry index
 */
void checkColumnEntry(BaseColumn* col, size_t which) {
    if (which >= col->getLength()) {
        printf("No such entry\n");
        exit(-1);
    }
}