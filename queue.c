#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"
#include "strnatcmp.h"

static void merge_sort(queue_t *q);
static void selection_sort(queue_t *q);
static void bubble_sort(queue_t *q);

void (*sort_func[SORT_METHOD_NUM])(queue_t *q) = {
    merge_sort,
    selection_sort,
    bubble_sort,
};

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
    q->size++;

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
    q->size++;

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

    if (!q || !q->size)
        return false;

    if (sp) {
        int n = bufsize >= strlen(q->head->value) ? strlen(q->head->value)
                                                  : bufsize - 1;
        strncpy(sp, q->head->value, n);
        sp[n] = '\0';
    }

    old_head = q->head;
    q->head = q->head->next;
    q->size--;

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
    return q ? q->size : 0;
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

static void merge_sort_divide(queue_t *q, queue_t *second_half)
{
    list_ele_t *current, *mid;

    if (!q)
        return;

    if (!(mid = q->head)) {
        second_half->head = NULL;
        return;
    }

    /* Divide the queue into two sub-queues. */
    for (current = mid->next; current;) {
        current = current->next;
        if (!current)
            break;

        current = current->next;
        mid = mid->next;
    }

    second_half->head = mid->next;
    mid->next = NULL;

    second_half->size = q->size >> 1;
    q->size = (q->size + 1) >> 1;
}

static void merge(queue_t *q, queue_t *second_half, queue_t *out)
{
    list_ele_t *p1, *p2, *out_p;

    p1 = q->head;
    p2 = second_half->head;

    /* Find who the head is if we want to merge two queues. */
    if (strnatcmp(p1->value, p2->value) > 0) {
        out->head = p2;
        p2 = p2->next;
    } else {
        out->head = p1;
        p1 = p1->next;
    }

    out_p = out->head;

    /* Merge two queues. */
    while (p1 && p2) {
        if (strnatcmp(p1->value, p2->value) > 0) {
            out_p->next = p2;
            p2 = p2->next;
        } else {
            out_p->next = p1;
            p1 = p1->next;
        }

        out_p = out_p->next;
    }

    out_p->next = p1 ? p1 : p2;

    out->size = q->size + second_half->size;
}

static void merge_sort(queue_t *q)
{
    queue_t second_half;

    if (!q || q->size <= 1)
        return;

    merge_sort_divide(q, &second_half);
    merge_sort(q);
    merge_sort(&second_half);
    merge(q, &second_half, q);
}

static void bubble_sort(queue_t *q)
{
    list_ele_t *curr, *curr_next;
    int i, j;

    if (!q || q->size <= 1)
        return;

    for (i = q->size; i > 0; i--) {
        list_ele_t *prev;

        curr = q->head;
        prev = NULL;

        for (j = 0; j < i - 1; j++) {
            curr_next = curr->next;

            if (strnatcmp(curr->value, curr_next->value) > 0) {
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

static list_ele_t **get_min_element(list_ele_t **min, list_ele_t **e)
{
    while (*e) {
        if (strnatcmp((*min)->value, (*e)->value) > 0)
            min = e;
        e = &(*e)->next;
    }

    return min;
}

static void selection_sort(queue_t *q)
{
    list_ele_t **curr; /* Current indirect element */
    list_ele_t *tmp;

    if (!q || q->size <= 1)
        return;

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

/*
 * Register the sorting method.
 */
void q_sort_register_method(int sort_method)
{
    /* Sanity check */
    if (sort_method < MERGE_SORT || sort_method >= SORT_METHOD_NUM)
        sort_method = MERGE_SORT;

    q_sort = sort_func[sort_method];
}
