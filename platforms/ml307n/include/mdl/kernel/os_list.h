/**
 *************************************************************************************
 * @copyright 版权所有 (C) 2023, 芯昇科技有限公司
 *            保留所有权利。
 *
 * @file      os_list.h
 *
 * @brief     os链表头文件.
 *
 * @revision  1.0
 *
 * @par 修改日志:
 * <table>
 * <tr><th>Date        <th>Version   <th>Author     <th>Description
 * <tr><td>2023-06-21  <td>1.0       <td>ICT Team   <td>初始版本
 * </table>
 ************************************************************************************
 */


#ifndef __OS_LIST_H__
#define __OS_LIST_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup KernelService
 */

/**@{*/

/**
 * osContainerOf - return the member address of ptr, if the type of ptr is the
 * struct type.
 */
#define osContainerOf(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

/**
 * osOffsetOf - return the member offset of the type.
 *
 */
#define osOffsetOf(type, member) \
    ((size_t) & ((type *)0)->member)

/**
 * @brief initialize a list object
 */
#define OS_LIST_OBJECT_INIT(object) { &(object), &(object) }

/**
 * @brief initialize a list
 *
 * @param l list to be initialized
 */
OS_INLINE void osListInit(osList_t *l)
{
    l->next = l->prev = l;
}

/**
 * @brief insert a node after a list
 *
 * @param l list to insert it
 * @param n new node to be inserted
 */
OS_INLINE void osListInsertAfter(osList_t *l, osList_t *n)
{
    l->next->prev = n;
    n->next = l->next;

    l->next = n;
    n->prev = l;
}

/**
 * @brief insert a node before a list
 *
 * @param n new node to be inserted
 * @param l list to insert it
 */
OS_INLINE void osListInsertBefore(osList_t *l, osList_t *n)
{
    l->prev->next = n;
    n->prev = l->prev;

    l->prev = n;
    n->next = l;
}

OS_INLINE void __osListRemove(osList_t *p, osList_t *n)
{
    n->prev = p;
    p->next = n;
}

/**
 * @brief remove node from list.
 * @param n the node to remove from the list.
 */
OS_INLINE void osListRemove(osList_t *n)
{
    n->next->prev = n->prev;
    n->prev->next = n->next;

    n->next = n->prev = n;
}

/**
 * @brief tests whether a list is empty
 * @param l the list to test.
 */
OS_INLINE int osListIsEmpty(const osList_t *l)
{
    return l->next == l;
}

/**
 * @brief get the list length
 * @param l the list to get.
 */
OS_INLINE unsigned int osListLen(const osList_t *l)
{
    unsigned int len = 0;
    const osList_t *p = l;
    while (p->next != l)
    {
        p = p->next;
        len ++;
    }

    return len;
}

/* Initialize a double list, this micro is only as right value */
#define OS_LIST_INIT(name)                                                                                             \
    {                                                                                                                  \
        &(name), &(name)                                                                                               \
    }

/**
 * @brief get the struct for this entry
 * @param node the entry point
 * @param type the type of structure
 * @param member the name of list in structure
 */
#define osListEntry(node, type, member) \
    osContainerOf(node, type, member)

/**
 * osListForEach - iterate over a list
 * @pos:    the osList_t * to use as a loop cursor.
 * @head:   the head for your list.
 */
#define osListForEach(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * osListForEachSafe - iterate over a list safe against removal of list entry
 * @pos:    the osList_t * to use as a loop cursor.
 * @n:      another osList_t * to use as temporary storage
 * @head:   the head for your list.
 */
#define osListForEachSafe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
        pos = n, n = pos->next)

/**
 * osListForEachEntry  -   iterate over list of given type
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 */
#ifdef _MSC_VER
#define osListForEachEntry(type, pos, head, member) \
    for (pos = osListEntry((head)->next, type, member); \
         &pos->member != (head); \
         pos = osListEntry(pos->member.next, type, member))
#else
#define osListForEachEntry(pos, head, member) \
    for (pos = osListEntry((head)->next, typeof(*pos), member); \
         &pos->member != (head); \
         pos = osListEntry(pos->member.next, typeof(*pos), member))
#endif

/**
 * osListForEachEntryReverse  -  reverse iterate over list of given type
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 */
#ifdef _MSC_VER
#define osListForEachEntryReverse(type, pos, head, member) \
    for (pos = osListEntry((head)->prev, type, member); \
         &pos->member != (head); \
         pos = osListEntry(pos->member.prev, type, member))
#else
#define osListForEachEntryReverse(pos, head, member) \
    for (pos = osListEntry((head)->prev, typeof(*pos), member); \
         &pos->member != (head); \
         pos = osListEntry(pos->member.prev, typeof(*pos), member))
#endif

/**
 * osListForEachEntrySafe - iterate over list of given type safe against removal of list entry
 * @pos:    the type * to use as a loop cursor.
 * @n:      another type * to use as temporary storage
 * @head:   the head for your list.
 * @member: the name of the list_struct within the struct.
 */
#ifdef _MSC_VER
#define osListForEachEntrySafe(typp, pos, typn, n, head, member) \
    for (pos = osListEntry((head)->next, typp, member), \
         n = osListEntry(pos->member.next, typp, member); \
         &pos->member != (head); \
         pos = n, n = osListEntry(n->member.next, typn, member))
#else
#define osListForEachEntrySafe(pos, n, head, member) \
    for (pos = osListEntry((head)->next, typeof(*pos), member), \
         n = osListEntry(pos->member.next, typeof(*pos), member); \
         &pos->member != (head); \
         pos = n, n = osListEntry(n->member.next, typeof(*n), member))
#endif

/**
 * osListFirstEntry - get the first element from a list
 * @ptr:    the list head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the list_struct within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define osListFirstEntry(ptr, type, member) \
    osListEntry((ptr)->next, type, member)

/**
 * osListFirstEntryOrNull - get the first element from a list
 * @ptr:    the list head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the list_struct within the struct.
 *
 * Note, that list may be empty. If empty, return OS_NULL.
 */
#define osListFirstEntryOrNull(ptr, type, member) \
    (!osListIsEmpty(ptr) ? osListFirstEntry(ptr, type, member) : OS_NULL)

#define OS_SLIST_OBJECT_INIT(object) { OS_NULL }

/**
 * @brief initialize a single list
 *
 * @param l the single list to be initialized
 */
OS_INLINE void osSlistInit(osSlist_t *l)
{
    l->next = OS_NULL;
}

OS_INLINE void osSlistAppend(osSlist_t *l, osSlist_t *n)
{
    struct osSlistNode *node;

    node = l;
    while (node->next) node = node->next;

    /* append the node to the tail */
    node->next = n;
    n->next = OS_NULL;
}

OS_INLINE void osSlistInsert(osSlist_t *l, osSlist_t *n)
{
    n->next = l->next;
    l->next = n;
}

OS_INLINE unsigned int osSlistLen(const osSlist_t *l)
{
    unsigned int len = 0;
    const osSlist_t *list = l->next;
    while (list != OS_NULL)
    {
        list = list->next;
        len ++;
    }

    return len;
}

OS_INLINE osSlist_t *osSlistRemove(osSlist_t *l, osSlist_t *n)
{
    /* remove slist head */
    struct osSlistNode *node = l;
    while (node->next && node->next != n) node = node->next;

    /* remove node */
    if (node->next != (osSlist_t *)0) node->next = node->next->next;

    return l;
}

OS_INLINE osSlist_t *osSlistFirst(osSlist_t *l)
{
    return l->next;
}

OS_INLINE osSlist_t *osSlistTail(osSlist_t *l)
{
    while (l->next) l = l->next;

    return l;
}

OS_INLINE osSlist_t *osSlistNext(osSlist_t *n)
{
    return n->next;
}

OS_INLINE int osSlistIsEmpty(osSlist_t *l)
{
    return l->next == OS_NULL;
}

/**
 * @brief get the struct for this single list node
 * @param node the entry point
 * @param type the type of structure
 * @param member the name of list in structure
 */
#define osSlistEntry(node, type, member) \
    osContainerOf(node, type, member)

/**
 * osSlistForEach - iterate over a single list
 * @pos:    the osSlist_t * to use as a loop cursor.
 * @head:   the head for your single list.
 */
#define osSlistForEach(pos, head) \
    for (pos = (head)->next; pos != OS_NULL; pos = pos->next)

/**
 * osSlistForEachEntry  -   iterate over single list of given type
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your single list.
 * @member: the name of the list_struct within the struct.
 */
#ifdef _MSC_VER
#define osSlistForEachEntry(type, pos, head, member) \
    for (pos = osSlistEntry((head)->next, type, member); \
         &pos->member != (OS_NULL); \
         pos = osSlistEntry(pos->member.next, type, member))
#else
#define osSlistForEachEntry(pos, head, member) \
    for (pos = osSlistEntry((head)->next, typeof(*pos), member); \
         &pos->member != (OS_NULL); \
         pos = osSlistEntry(pos->member.next, typeof(*pos), member))
#endif

/**
 * osSlistFirstEntry - get the first element from a slist
 * @ptr:    the slist head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the slist_struct within the struct.
 *
 * Note, that slist is expected to be not empty.
 */
#define osSlistFirstEntry(ptr, type, member) \
    osSlistEntry((ptr)->next, type, member)

/**
 * osSlistTailEntry - get the tail element from a slist
 * @ptr:    the slist head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the slist_struct within the struct.
 *
 * Note, that slist is expected to be not empty.
 */
#define osSlistTailEntry(ptr, type, member) \
    osSlistEntry(osSlistTail(ptr), type, member)

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
