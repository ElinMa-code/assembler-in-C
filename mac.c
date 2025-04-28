#include "utilities.h"
#include "mac.h"
#include "table.h"
#include "charsequence.h"
#define MAC_ATTR(node) ((node)->data).mac
#define POSITION(node) MAC_ATTR(node).position
#define LINES(node) MAC_ATTR(node).lines


#define BREAK_IF_TOO_LONG(line, file) \
    if (!(*next('\n', line)) && (*line != '\n') && !feof(file)) \
    { \
        printf("\nFatal: cannot process line with more than %d characters", MAX_LINE_LENGTH); \
        return FALSE; \
    }

#define RUN_BREAK_IF_ERROR(func) \
    if (!(func)) \
        return FALSE;

/* Writes the result to the file out.
Returns EOF if this is the end of the file, FALSE if the line is too long, and TRUE if it happens otherwise
 */
static int mmove_process_line(FILE *, char *, table *, FILE *);

boolean mac_move(FILE *source, FILE *out)
{
    table macs_table = NULL;
    char line[MAX_LINE_LENGTH];
    int result = TRUE;

    while (result != EOF && result)
    {
        result = mmove_process_line(source, line, &macs_table, out);
    }
    fseek(out, 0, SEEK_SET);
    /* if found a line  long,or end of file */
    return result == EOF;
}

static int mmove_process_definition(FILE *, table *, char *, char *);

static int mmove_process_reference(FILE *, char *, table , FILE *);

static int mmove_process_line(FILE *source, char *line, table *macs, FILE *out)
{
    char *opstart, *opend, *keystart, *keyend;
    table existing_definition;

    if (fgets(line, MAX_LINE_LENGTH, source) == NULL)
        return EOF;
    BREAK_IF_TOO_LONG(line, source)

    opstart = next_nonnotewhite(line);
    opend = next_notewhite(opstart);

    if (str_equal(opstart, opend, "mcro"))
    {
        keystart = next_nonnotewhite(opend);
        keyend = next_notewhite(keystart);
        RUN_BREAK_IF_ERROR(mmove_process_definition(source, macs, keystart, keyend))
    }

    else if ((existing_definition = search_entry(macs, opstart, opend)) != NULL)
    {
        mmove_process_reference(source, line, existing_definition, out);
    }
    else if (!(line[0] == ';' || equals(line, "endmcro\r\n") || equals(line, "\r\n")))
        fputs(line, out);
    return TRUE;
}

static int mmove_process_definition(FILE *source, table *macs, char *keystart, char *keyend)
{
    /* To prevent memory leakage then need to preserve the memory of the line before */
    static char line[MAX_LINE_LENGTH];
    fpos_t current_position;
    row_data mac_data;
    unsigned int linespan = 0;
    char *opstart;

    fgetpos(source, &current_position);

    do{
        fgets(line, MAX_LINE_LENGTH, source);
        BREAK_IF_TOO_LONG(line, source)
        linespan++;
        opstart = next_nonnotewhite(line);
        /* opstart+7 because of the length of 'endmcro' */
    } while (!str_equal(opstart, opstart+7, "endmcro") && !feof(source));

    linespan--;

    mac_data.mac.position = current_position;
    mac_data.mac.lines = linespan;
    add_entry(macs, keystart, keyend, mac_data);

    return TRUE;
}

static int mmove_process_reference(FILE *source, char *line, table mac, FILE *out)
{
    fpos_t temp;
    int i;
    fgetpos(source, &temp);
    fsetpos(source, &POSITION(mac));

    for(i = 0; i < LINES(mac); i++)
    {
        fgets(line, MAX_LINE_LENGTH, source);
        fputs(line, out);
    }

    fsetpos(source, &temp);
    return 0;
}
