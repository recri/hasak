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
#ifndef _every_any_h
#define _every_any_h

#include "timing.h"

/*
** maintain a list of functions to call every time the sample counter runs over
*/
struct every_any {
  void (*function)(void);	/* function to call */
  struct every_any *next;	/* next element in list */
};
typedef struct every_any every_any_t;

static every_any_t *every_sample_head = NULL;
static elapsedSamples every_sample_counter;
static every_any_t *every_update_head = NULL;
static elapsedUpdates every_update_counter;
static every_any_t *every_milli_head = NULL;
static elapsedMillis every_milli_counter;

static void every_any_add(every_any_t *node, every_any_t **head) {
  if (node->next != NULL)
    Serial.printf("every_any_add: node pointer to add is already linked in list\n");
  else {
    node->next = *head;
    *head = node;
  }
}

static void every_sample_add(every_any_t *node) { every_any_add(node, &every_sample_head); }

static void every_update_add(every_any_t *node) { every_any_add(node, &every_update_head); }

static void every_milli_add(every_any_t *node) { every_any_add(node, &every_milli_head); }

#define every_sample(f) { static every_any_t node; node.function = (f); every_sample_add(&node); }

#define every_update(f) { static every_any_t node; node.function = (f); every_update_add(&node); }

#define every_milli(f) { static every_any_t node; node.function = (f); every_milli_add(&node); }

static void every_any_invoke(every_any_t *node) {
  while (node != NULL) {
    node->function();
    node = node->next;
  }
}

static void every_any_setup(void) {
}

static void every_any_loop(void) {
  if (every_sample_counter > 0) {
    every_sample_counter = 0;
    every_any_invoke(every_sample_head);
  }
  if (every_update_counter > 0) {
    every_update_counter = 0;
    every_any_invoke(every_update_head);
  }
  if (every_milli_counter > 0) {
    every_milli_counter = 0;
    every_any_invoke(every_milli_head);
  }
}

#endif
