/* -*- mode: c++; tab-width: 8 -*- */
/*
 * hasak (ham and swiss army knife) keyer for Teensy 4.X, 3.X
 * Copyright (c) 2021 by Roger Critchlow, Charlestown, MA, USA
 * ad5dz, rec@elf.org
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef listener_h_
#define listener_h_

#ifndef LISTENER_POOL
#define LISTENER_POOL 1024
#endif

typedef struct listener_t {
  struct listener_t *next;
  void (*listener)(int);
} listener_t;

static listener_t listener_pool[LISTENER_POOL];

static listener_t *listener_free_list = NULL;

static void listener_free(listener_t *p) {
  p->next = listener_free_list;
  listener_free_list = p;
}

static listener_t *listener_new(listener_t *next, void (*listener)(int)) {
  if (listener_free_list == NULL) {
    Serial.printf("listener_new: no free nodes\n");
    static listener_t node;
    node.next = next;
    node.listener = listener;
    return &node;
  } else {
    listener_t *p = listener_free_list;
    listener_free_list = p->next;
    p->next = next;
    p->listener = listener;
    return p;
  }
}

static void listener_add(listener_t **head, void (*listener)(int)) {
  listener_t *p = listener_new(*head, listener);
  *head = p;
}

static int listener_invoke(listener_t *p, int arg) {
  int n = 0;
  while (p != NULL) {
    n += 1;
    p->listener(arg);
    p = p->next;
  }
  return n;
}

static void listener_invoke_and_free(listener_t *p, int arg) {
  while (p != NULL) {
    listener_t *n = p->next;
    p->listener(arg);
    p->next = listener_free_list;
    listener_free_list = p;
    p = n;
  }
}

static listener_t *after_idle_head = NULL;

static void listener_setup(void) {
  listener_t *p = &listener_pool[LISTENER_POOL];
  while (--p >= &listener_pool[0]) listener_free(p);
}

/*
** run a function once at the end of the current loop.
** used to avoid listener loops or to defer a computation
*/
static void after_idle(void (*func)(int)) { listener_add(&after_idle_head, func); }

/*
** run the after_idle queue
** an after_idle handler can post another after_idle request using the same node,
** that request will run at the end of the next loop.
*/
static void listener_loop(void) {
  listener_t *head = after_idle_head;
  after_idle_head = NULL;
  listener_invoke_and_free(head, -1);
}

#endif
