/*
 * Copyright (c) 2017, GlobalLogic Ukraine LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the GlobalLogic.
 * 4. Neither the name of the GlobalLogic nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY GLOBALLOGIC UKRAINE LLC ``AS IS`` AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL GLOBALLOGIC UKRAINE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/list.h>
#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/errno.h>

MODULE_AUTHOR("Serhii Popovych <serhii.popovych@globallogic.com>");
MODULE_DESCRIPTION("Hello1: List and timing logic");
MODULE_LICENSE("Dual BSD/GPL");

struct my_data {
  ktime_t start;
  ktime_t end;
  struct list_head list;
};

static LIST_HEAD(head);

int print_hello(uint times)
{
  int i;
  struct my_data *data;

  if (times > 10) {
    pr_err("Error: 'times' = %u > 10\n", times);
    return -EINVAL;
  }

  if (times == 0 || (times > 5 && times <= 10)) {
    pr_warn("Warning: 'times' = %u (0 or 6-10)\n", times);
  }

  for (i = 0; i < times; i++) {
    data = kmalloc(sizeof(*data), GFP_KERNEL);
    if (!data) {
      pr_err("Error: kmalloc failed\n");
      return -ENOMEM;
    }
    data->start = ktime_get();
    pr_info("Hello, world!\n");
    data->end = ktime_get();
    list_add(&data->list, &head);
  }

  return 0;
}
EXPORT_SYMBOL(print_hello);

static int __init hello1_init(void)
{
  return 0;
}

static void __exit hello1_exit(void)
{
  struct my_data *data, *tmp;

  list_for_each_entry_safe(data, tmp, &head, list) {
    s64 duration_ns = ktime_to_ns(ktime_sub(data->end, data->start));
    pr_info("Print duration: %lld ns\n", duration_ns);
    list_del(&data->list);
    kfree(data);
  }
}

module_init(hello1_init);
module_exit(hello1_exit);
