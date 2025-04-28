#include <string.h>
#include <stdlib.h>
#include "table.h"
#include "functioning.h"
#include "charsequence.h"


/*In this section will return the pointer to the line with the last key that comes before the existing key
Or we return NULL if our key is in alphabetical order before the key of the header.*/
table search_last_row_bef(table *label, char *keystart, char *keyend)
{
    char temp;
    table curr , prior = NULL;

    /*if empty table or key comes before the key of the head.
    SO NULL */
    if ((*label) == NULL || str_bef(keystart, keyend, (*label)->key))
        return NULL;
    curr = *label;
    temp = *keyend;
    *keyend = '\0';
    while (KEYOF_NODE_BEF(curr, keystart))
    {
        prior = curr;
        curr = curr->next;
    }
    *keyend = temp;
    return prior;
}

table search_label_entry(table *label, char *keystart, char *keyend)
{
    char temp;
    table curr;

    /*if empty table or key comes before the key of the head.
    SO NULL */
    if ((*label) == NULL || str_bef(keystart, keyend, (*label)->key))
        return NULL;
    curr = *label;
    temp = *keyend;
    *keyend = '\0';
    while ((curr) != NULL && !str_equal(keystart, keyend, curr->key))
    {
        curr = curr->next;
    }
    *keyend = temp;
    return curr;
}

/* This kit adds a new item to the table, or it creates a new table with the item*/
boolean add_entry(table *label, char *keystart, char *keyend, row_data data)
{
    table prior;
    table next_node;
    char *s = strcpy_safe(keystart, keyend);

    /* if empty table or key comes before the key of the head.
    Add the new row on the spot */
    if ((*label) == NULL || custom_string_compare(s ,(*label)->key))
    {
        *label = new_row(keystart, keyend, data,*label);
        return TRUE;
    }

    prior = search_last_row_bef(label, keystart, keyend);

    /* Different from the previous one */
    if (!prior)
        return FALSE;

    next_node = prior->next;

    if (KEYOF_NODE_EQUALS(next_node, keystart, keyend))
        return FALSE;

    prior->next = new_row(keystart, keyend, data, next_node);  /* (pointer_name)->(variable_name) */
    return TRUE;
}

table search_entry(table *label, char *keystart, char *keyend)
{
    table prior;
    table target;
    prior = search_last_row_bef(label, keystart, keyend);

    /* check if Table empty, or key comes before head  */
    if (prior == NULL)
    {
        if (KEYOF_NODE_EQUALS(*label, keystart, keyend))
            return *label;
        return NULL;
    }

    if (prior->next == NULL)      /* (pointer_name)->(variable_name) */
        return NULL;

    target = prior->next;

    if (str_equal(keystart, keyend, target->key))
        return target;

    return NULL;
}

void free_table(table *label)
{
    table head;
    table temp;
    if (label == NULL)
        return;

    head = *label;
    while (head != NULL)
    {
        temp = head;
        head = head->next;        /* (pointer_name)->(variable_name) */
        free(temp->key);
        free(temp);
    }
}

/* This section creates a new row in the table. Using a copy of the key provided with and using pointers */
table new_row(char *keystart, char *keyend, row_data data, table next)
{
    char *key_copy;
    table result;

    key_copy = strcpy_safe(keystart, keyend);
    result = (table) malloc_safe(sizeof(table_row));

    result->key = key_copy;         /* (pointer_name)->(variable_name) */
    result->data = data;
    result->next = next;
    return result;
}