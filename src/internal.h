/*
 *    Regex Pattern Analyzer
 *
 *    Copyright Zoltan Herczeg (hzmester@freemail.hu). All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice, this list of
 *      conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright notice, this list
 *      of conditions and the following disclaimer in the documentation and/or other materials
 *      provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER(S) AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDER(S) OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef REPAN_INTERNAL_H
#define REPAN_INTERNAL_H

#include <stdlib.h>
#include <string.h>

#ifdef DEBUG_REPAN

#include <stdio.h>

#define REPAN_ASSERT(x) \
    if (!(x)) { \
        printf("ASSERTION FAILURE in %s:%d\n", __FILE__, __LINE__); \
        abort(); \
    }

#else /* !DEBUG_REPAN */

#define REPAN_ASSERT(x)

#endif /* DEBUG_REPAN */

/* True/false constants. */
#define REPAN_TRUE 1
#define REPAN_FALSE 0

/* Private functions. */
#define REPAN_PRIV(func_name)  repan__priv_ ## func_name

/* Represents infinite repeats. */
#define REPAN_DECIMAL_INF UINT32_MAX

/* Maximum number of brackets, names, name lengths, etc.. */
#define REPAN_RESOURCE_MAX 0xffffff

/* These set up the core infrastructure. */
#include "repan.h"
#include "alloc.h"
#include "literal.h"
#include "nodes.h"

#endif /* REPAN_INTERNAL_H */
