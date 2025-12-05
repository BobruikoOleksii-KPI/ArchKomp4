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
#include <linux/moduleparam.h>
#include <linux/errno.h>
#include <linux/list.h>
#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/bug.h>  // For BUG_ON

MODULE_AUTHOR("Serhii Popovych <serhii.popovych@globallogic.com>");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");

static uint times = 1;
module_param(times, uint, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(times, "Number of times to print Hello, world!");

struct my_data {
  ktime_t time;
  struct list_head list;
};

static LIST_HEAD(head);

static int __init hello_init(void)
{
  int i;
  struct my_data *data;

  if (times == 0) {
    BUG_ON(1);  // Forced crash for times=0
  }

  if (times > 10) {
    pr_err("Error: times %u > 10\n", times);
    return -EINVAL;
  }

  if (times > 5 && times <= 10) {
    pr_warn("Warning: times %u (6-10)\n", times);
  }

  for (i = 0; i < times; i++) {
    data = kmalloc(sizeof(*data), GFP_KERNEL);
    if (i == times - 1) {  // Force failure on last
      data = NULL;  // Simulate kmalloc 0
    }
    if (!data) {
      pr_err("Forced kmalloc failure\n");
      BUG_ON(1);  // Crash on failure
    }
    data->time = ktime_get();
    list_add(&data->list, &head);
    pr_emerg("Hello, world!\n");
  }
  return 0;
}

static void __exit hello_exit(void)
{
  struct my_data *data, *tmp;

  list_for_each_entry_safe(data, tmp, &head, list) {
    pr_info("Time: %lld ns\n", ktime_to_ns(data->time));
    list_del(&data->list);
    kfree(data);
  }
}

module_init(hello_init);
module_exit(hello_exit);
