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
#ifndef _every_sample_h
#define _every_sample_h

#include "timing.h"

/*
** maintain a list of functions to call every time the sample counter runs over
*/
struct every_sample {
  void (*function)(void);	/* function to call */
  struct every_sample *next;	/* next element in list */
};
typedef struct every_sample every_sample_t;

static every_sample_t *every_sample_head = NULL;
static elapsedSamples every_sample_counter;

#define every_sample(f) { static every_sample_t node; node.function = (f); every_sample_add(&node); }

static void every_sample_add(every_sample_t *node) {
  if (node->next != NULL)
    Serial.printf("every_sample_add: node pointer to add is already linked in list\n");
  else {
    node->next = every_sample_head;
    every_sample_head = node;
  }
}

static void every_sample_setup(void) {
}

static void every_sample_loop(void) {
  if (every_sample_counter > 0) {
    every_sample_counter = 0;
    for (every_sample_t *esp = every_sample_head; esp != NULL; esp = esp->next)
      esp->function();
  }
}

#endif
