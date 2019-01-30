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

#include "ln_tensor.h"
#include "ln_util.h"
#include "ln_msg.h"
#include "ln_name.h"

ln_tensor_list_entry *ln_tensor_list_entry_create(const char *arg_name,
                                                  const char *name)
{
    ln_tensor_list_entry *entry;

    entry = ln_alloc(sizeof(ln_tensor_list_entry));
    entry->arg_name = ln_strdup(arg_name);
    entry->name = ln_strdup(name);
    entry->offset = 0;
    return entry;
}

void ln_tensor_list_entry_free(ln_tensor_list_entry *entry)
{
    ln_free(entry->arg_name);
    ln_free(entry->name);
    ln_free(entry);
}

static void list_entry_free_wrapper(void *entry)
{
    ln_tensor_list_entry_free(entry);
}

ln_list *ln_tensor_list_append(ln_list *list, const char *arg_name,
                               const char *name)
{
    ln_tensor_list_entry *entry;

    entry = ln_tensor_list_entry_create(arg_name, name);
    list = ln_list_append(list, entry);
    return list;
}

void ln_tensor_list_free(ln_list *list)
{
    ln_list_free_deep(list, list_entry_free_wrapper);
}

ln_list *ln_tensor_list_copy(ln_list *list)
{
    ln_list *new_list = NULL;
    ln_tensor_list_entry *entry;

    LN_LIST_FOREACH(entry, list) {
        new_list = ln_tensor_list_append(new_list, entry->arg_name, entry->name);
    }
    return new_list;
}

static int cmp_by_arg_name(const void *data1, const void *data2)
{
    const ln_tensor_list_entry *e1 = data1;
    const ln_tensor_list_entry *e2 = data2;

    return strcmp(e1->arg_name, e2->arg_name);
}

static int cmp_by_name(const void *data1, const void *data2)
{
    const ln_tensor_list_entry *e1 = data1;
    const ln_tensor_list_entry *e2 = data2;

    return strcmp(e1->name, e2->name);
}

char *ln_tensor_list_find_name(ln_list *list, const char *arg_name)
{
    ln_tensor_list_entry cmp_entry;
    ln_tensor_list_entry *result_entry;

    cmp_entry.arg_name = (char *)arg_name;
    result_entry = ln_list_find_custom(list, &cmp_entry, cmp_by_arg_name);
    if (!result_entry)
        return NULL;
    return result_entry->name;
}

ln_tensor_list_entry *ln_tensor_list_find_by_arg_name(ln_list *list,
                                                      const char *arg_name)
{
    ln_tensor_list_entry cmp_entry;

    cmp_entry.arg_name = (char *)arg_name;
    return ln_list_find_custom(list, &cmp_entry, cmp_by_arg_name);
}

ln_tensor_list_entry *ln_tensor_list_find_by_name(ln_list *list,
                                                  const char *name)
{
    ln_tensor_list_entry cmp_entry;

    cmp_entry.name = (char *)name;
    return ln_list_find_custom(list, &cmp_entry, cmp_by_name);
}

int ln_tensor_list_length(ln_list *list)
{
    return ln_list_length(list);
}

ln_tensor_entry *ln_tensor_entry_create(const char *name, tl_tensor *tensor)
{
    ln_tensor_entry *entry;

    entry = ln_alloc(sizeof(ln_tensor_entry));
    entry->name = ln_strdup(name);
    entry->tensor = tensor;
    entry->owner = NULL;
    entry->creater = NULL;
    entry->offset = 0;
    entry->isstatic = 0;
    entry->mtype = LN_MEM_NONE;

    return entry;
}

void ln_tensor_entry_free(ln_tensor_entry *entry)
{
    ln_free(entry->name);
    ln_free(entry->owner);
    ln_free(entry->creater);
    ln_free(entry);
}

void ln_tensor_entry_free_tensor_too(ln_tensor_entry *entry)
{
    ln_free(entry->name);
    tl_tensor_free(entry->tensor);
    ln_free(entry);
}

static void tensor_entry_free_tensor_too_wrapper(void *p)
{
    ln_tensor_entry_free_tensor_too(p);
}

void ln_tensor_entry_set_owner(ln_tensor_entry *entry, ln_hash *tensor_table,
                               char *direct_owner)
{
    ln_tensor_entry *te;

    assert(entry->name != direct_owner);

    te = ln_tensor_table_find(tensor_table, direct_owner);
    while (te->owner)
        te = ln_tensor_table_find(tensor_table, te->owner);
    entry->owner = ln_strdup(te->name);
}

void ln_tensor_entry_set_creater(ln_tensor_entry *entry, const char *creater)
{
    entry->creater = ln_strdup(creater);
}

ln_hash *ln_tensor_table_create(void)
{
    return ln_hash_create(ln_str_hash, ln_str_cmp, NULL,
                          tensor_entry_free_tensor_too_wrapper);
}

int ln_tensor_table_insert(ln_hash *table, ln_tensor_entry *entry)
{
    return ln_hash_insert(table, entry->name, entry);
}

int ln_tensor_table_remove(ln_hash *table, const char *name)
{
    return ln_hash_remove(table, (char *)name);
}

ln_tensor_entry *ln_tensor_table_find(ln_hash *table, const char *name)
{
    return ln_hash_find(table, (char *)name);
}

void ln_tensor_table_free(ln_hash *table)
{
    ln_hash_free(table);
}

#define TRT_WEIGHT_ERR(file, fmt, varg...)                    \
    ln_msg_error("load_trt_weight_file(): invalid weight file %s: "fmt, \
                 (file), ##varg)
#define TRT_WEIGHT_WARN(file, fmt, varg...)                              \
    ln_msg_warn("load_trt_weight_file(): weight file %s: "fmt, \
                   (file), ##varg)

static void next_line(FILE *fp, const char *file)
{
    char *p = NULL;
    size_t n = 0;

    if (getline(&p, &n, fp) < 0)
        TRT_WEIGHT_ERR(file, "error skip one line");
    ln_free(p);
}

static void copy_weight_float(FILE *fp, ln_tensor_entry *te, const char *name,
                              int len, const char *file)
{
    int n, i;
    uint32_t val;
    ln_copy_func copy;

    if (te->tensor->dtype != TL_FLOAT)
        TRT_WEIGHT_ERR(file, "data type of weight %s not match", name);
    ln_msg_debug("loading data %s to 0x%p", name, te->tensor->data);
    copy = ln_mem_copy_func(te->mtype, LN_MEM_CPU);
    for (i = 0; i < len; i++) {
        n = fscanf(fp, "%x", &val);
        if (n != 1)
            TRT_WEIGHT_ERR(file, "error reading weight %s", name);
        copy(&((uint32_t *)te->tensor->data)[i], &val, sizeof(uint32_t));
    }
}

static void copy_weight_int8(FILE *fp, ln_tensor_entry *te, const char *name,
                             int len, const char *file)
{
    int n, i;
    uint8_t val;
    ln_copy_func copy;

    if (te->tensor->dtype != TL_INT8)
        TRT_WEIGHT_ERR(file, "data type of weight %s not match", name);
    ln_msg_debug("loading data %s to 0x%p", name, te->tensor->data);
    copy = ln_mem_copy_func(te->mtype, LN_MEM_CPU);
    for (i = 0; i < len; i++) {
        n = fscanf(fp, "%hhx", &val);
        if (n != 1)
            TRT_WEIGHT_ERR(file, "error reading weight %s", name);
        copy(&((uint8_t *)te->tensor->data)[i], &val, sizeof(uint8_t));
    }
}

void ln_tensor_table_load_trt_weight_file(ln_hash *table, const char *file)
{
    FILE *fp;
    int count, n;
    char name[LN_MAX_NAME_LEN];
    int type, len;
    ln_tensor_entry *te;

    if (!(fp = fopen(file, "r")))
        ln_msg_error_sys("trt_weight_err(): cannot open %s", file);

    if ((n = fscanf(fp, "%d", &count)) != 1)
        TRT_WEIGHT_ERR(file, "error reading count number");
    if (count < 0)
        TRT_WEIGHT_ERR(file, "negative count number");

    while (count--) {
        if ((n = fscanf(fp, "%s", name)) != 1)
            TRT_WEIGHT_ERR(file, "error reading name");
        if ((n = fscanf(fp, "%d %d", &type, &len)) != 2)
            TRT_WEIGHT_ERR(file, "error reading the type and len of weight %s",
                                     name);
        if (type < 0)
            TRT_WEIGHT_ERR(file, "negative type of weight %s", name);
        if (len < 0)
            TRT_WEIGHT_ERR(file, "negative len of weight %s", name);

        te = ln_tensor_table_find(table, name);
        if (!te) {
            TRT_WEIGHT_WARN(file, "ignore unused weight %s", name);
            next_line(fp, file);
            continue;
        }
        if (len != te->tensor->len)
            TRT_WEIGHT_ERR(file, "length %d of weight %s doesn't match %d", len,
                           name, te->tensor->len);

        switch (type) {
        case 0:                 /* float */
            copy_weight_float(fp, te, name, len, file);
            break;
        case 1:                 /* half */
            TRT_WEIGHT_ERR(file, "unsupported type of weight %s", name);
            break;
        case 2:                 /* int8 */
            copy_weight_int8(fp, te, name, len, file);
            break;
        default:
            TRT_WEIGHT_ERR(file, "unsupported type of weight %s", name);
            break;
        }
    }

    fclose(fp);
}
