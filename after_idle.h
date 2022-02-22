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
#ifndef _after_idle_h
#define _every_sample_h

/*
** maintain a list of functions to call once at the end of the current loop
*/
struct after_idle {
  void (*function)(void);	/* function to call */
  struct after_idle *next;	/* next element in list */
};
typedef struct after_idle after_idle_t;

static after_idle_t after_idle_tail;
static after_idle_t *after_idle_head = &after_idle_tail;

/*
** the after_idle(function) macro allocates exactly one after_idle_t node to store the function to be called.
** If the node is already in use, the prior use keeps the node.
*/
#define after_idle(f) { static after_idle_t node; if (node.next == NULL) { node.function = (f); after_idle_add(&node);} }

static void after_idle_add(after_idle_t *node) {
  if (node->next != NULL)
    Serial.printf("after_idle_add: node pointer to add is already linked in list\n");
  else {
    node->next = after_idle_head;
    after_idle_head = node;
  }
}

static void after_idle_setup(void) {
}

/*
** run the after_idle queue
** an after_idle handler can post another after_idle request using the same node,
** that request will run at the end of the next loop.
*/
static void after_idle_loop(void) {
  after_idle_t *aip = after_idle_head; // copy the queue head
  after_idle_head = &after_idle_tail;  // zero the queue head
  while (aip != &after_idle_tail) {    // while the queue is not empty
    after_idle_t *next = aip->next;    // cache the next element pointer
    aip->next = NULL;		       // zero the node's next element pointer
    aip->function();		       // call the handler
    aip = next;			       // step to the next queue element
  }
}

#endif
