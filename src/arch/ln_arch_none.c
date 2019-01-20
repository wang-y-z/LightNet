/*
 * Copyright (c) 2018 Zhao Zhixu
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "ln_arch.h"

extern ln_op ln_opimpl_slice;
extern ln_op ln_opimpl_reshape;
extern ln_op ln_opimpl_maxreduce;
extern ln_op ln_opimpl_elew;
extern ln_op ln_opimpl_transpose;
extern ln_op ln_opimpl_zeros;
extern ln_op ln_opimpl_create;
extern ln_op ln_opimpl_conv2d;
extern ln_op ln_opimpl_relu;
extern ln_op ln_opimpl_maxpool2d;
extern ln_op ln_opimpl_softmax;
extern ln_op ln_opimpl_concat;
extern ln_op ln_opimpl_batchnorm;
extern ln_op ln_opimpl_upsample;
extern ln_op ln_opimpl_maxreduce_arg;
extern ln_op ln_opimpl_print;
extern ln_op ln_opimpl_sigmoid;
/* end of declare none ops */

/* TODO: use a hash */
static ln_op *ops_none[] = {
    &ln_opimpl_slice,
    &ln_opimpl_reshape,
    &ln_opimpl_maxreduce,
    &ln_opimpl_elew,
    &ln_opimpl_transpose,
    &ln_opimpl_zeros,
    &ln_opimpl_create,
    &ln_opimpl_conv2d,
    &ln_opimpl_relu,
    &ln_opimpl_maxpool2d,
    &ln_opimpl_softmax,
    &ln_opimpl_concat,
    &ln_opimpl_batchnorm,
    &ln_opimpl_upsample,
    &ln_opimpl_maxreduce_arg,
    &ln_opimpl_print,
    &ln_opimpl_sigmoid,
/* end of init none ops */
    NULL
};

ln_expander_func ep_funcs_none[] = {
    NULL
};

ln_combiner_func cb_funcs_none[] = {
    NULL
};

ln_arch ln_arch_none = {
    .init_func = NULL,
    .cleanup_func = NULL,
    .reg_ops = ops_none,
    .ep_funcs = ep_funcs_none,
    .cb_funcs = cb_funcs_none,
    .arch_name = "none",
};