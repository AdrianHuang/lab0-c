#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

#undef BUBBLE_SORT
#define SELECTION_SORT

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (!q)
        return NULL;

    memset(q, 0, sizeof(queue_t));

    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    list_ele_t *p, *t;

    if (!q)
        return;

    for (p = q->head; p;) {
        /* Store the next element */
        t = p->next;

        /* Free current storage */
        free(p->value);
        free(p);

        p = t;
    }

    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    size_t n = strlen(s);
    list_ele_t *newh;

    if (!q || !n)
        return false;

    newh = malloc(sizeof(list_ele_t));
    if (!newh)
        return false;

    /* Don't forget to allocate space for the string and copy it */
    /* What if either call to malloc returns NULL? */
    newh->value = malloc(n + 1);
    if (!newh->value) {
        free(newh);
        return false;
    }

    strncpy(newh->value, s, n);
    newh->value[n] = '\0';

    newh->next = q->head;
    q->head = newh;
    q->n++;

    if (!q->tail)
        q->tail = newh;

    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    size_t n = strlen(s);
    list_ele_t *newh;

    if (!q || !n)
        return false;

    newh = malloc(sizeof(list_ele_t));
    if (!newh)
        return false;

    /* Don't forget to allocate space for the string and copy it */
    /* What if either call to malloc returns NULL? */
    newh->next = NULL;
    newh->value = malloc(n + 1);
    if (!newh->value) {
        free(newh);
        return false;
    }

    strncpy(newh->value, s, n);
    newh->value[n] = '\0';

    if (q->tail)
        q->tail->next = newh;

    q->tail = newh;
    q->n++;

    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    list_ele_t *old_head;

    if (!q || !q->n)
        return false;

    if (sp) {
        int n = bufsize >= strlen(q->head->value) ? strlen(q->head->value)
                                                  : bufsize - 1;
        strncpy(sp, q->head->value, n);
        sp[n] = '\0';
    }

    old_head = q->head;
    q->head = q->head->next;
    q->n--;

    free(old_head->value);
    free(old_head);

    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    if (!q)
        return 0;

    return q->n;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    list_ele_t *curr, *next, *tmp;

    if (!q || !q->head)
        return;

    for (curr = q->head, next = q->head->next; next;) {
        tmp = next->next;
        next->next = curr;

        curr = next;
        next = tmp;
    }

    q->head->next = NULL;
    q->tail = q->head;
    q->head = curr;
}

#ifdef BUBBLE_SORT
static void bubble_sort(queue_t *q)
{
    list_ele_t *curr, *curr_next, *prev;
    int i, j;

    for (i = q->n; i > 0; i--) {
        curr = q->head;
        prev = NULL;

        for (j = 0; j < i - 1; j++) {
            curr_next = curr->next;

            if (strcmp(curr->value, curr_next->value) > 0) {
                if (!prev)
                    q->head = curr_next;
                else
                    prev->next = curr->next;

                curr->next = curr_next->next;
                curr_next->next = curr;

                prev = curr_next;
            } else {
                prev = curr;
                curr = curr->next;
            }
        }
    }
}
#endif

#ifdef SELECTION_SORT
static list_ele_t **get_min_element(list_ele_t **min, list_ele_t **e)
{
    while (*e) {
        if (strcmp((*min)->value, (*e)->value) > 0)
            min = e;
        e = &(*e)->next;
    }

    return min;
}

static void selection_sort(queue_t *q)
{
    list_ele_t **curr; /* Current indirect element */
    list_ele_t *tmp;

    for (curr = &q->head; *curr; curr = &(*curr)->next) {
        list_ele_t **min; /* Minimal indirect element */

        min = get_min_element(curr, &(*curr)->next);

        /* No need to swap. */
        if (min == curr)
            continue;

        tmp = (*curr)->next;

        /* current indirect element is NOT next to minimal indirect element. */
        if (tmp != *min) {
            (*curr)->next = (*min)->next;
            (*min)->next = tmp;

            tmp = *min;
            *min = *curr;
            *curr = tmp;
        } else {
            (*curr)->next = (*min)->next;
            tmp->next = *curr;
            *curr = tmp;
        }
    }
}
#endif

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    if (!q)
        return;

#ifdef SELECTION_SORT
    selection_sort(q);
#elif defined(BUBBLE_SORT)
    bubble_sort(q);
#endif
}
