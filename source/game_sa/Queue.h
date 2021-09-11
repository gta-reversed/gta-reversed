/*
*  Rockstar's Queue
*      This Queue is used for thead interaction.
*      Thread A should be limited to the function AddToQueue and thread B to the functions RemoveFirstInQueue and GetFirstInQueue
*      No need for mutual exclusion by design.
*
*  This source code is offered for use in the public domain. You may
*  use, modify or distribute it freely.
*
*  This code is distributed in the hope that it will be useful but
*  WITHOUT ANY WARRANTY. ALL WARRANTIES, EXPRESS OR IMPLIED ARE HEREBY
*  DISCLAIMED. This includes but is not limited to warranties of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/
#pragma once

struct Queue {
    int32* m_queue;
    int32  m_head;
    int32  m_tail;
    int32  m_size;
};
VALIDATE_SIZE(Queue, 0x10);

inline Queue* InitialiseQueue(Queue* q, int32 size) {
    q->m_queue = (int32*)LocalAlloc(LPTR, sizeof(int32) * size);
    q->m_size = size;
    q->m_head = q->m_tail = 0;
    return q;
}

inline void FinalizeQueue(Queue* q) {
    LocalFree(q->m_queue);
    q->m_size = q->m_head = q->m_tail = 0;
}

inline void AddToQueue(Queue* q, int32 i) {
    q->m_queue[q->m_tail] = i;
    q->m_tail = (q->m_tail + 1) % q->m_size;
}

inline int32 GetFirstInQueue(Queue* q) {
    return (q->m_head == q->m_tail ? -1 : q->m_queue[q->m_head]);
}

inline void RemoveFirstInQueue(Queue* q) {
    if (q->m_head != q->m_tail)
        q->m_head = (q->m_head + 1) % q->m_size;
}
