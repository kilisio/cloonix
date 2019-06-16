/*****************************************************************************/
/*    Copyright (C) 2006-2019 cloonix@cloonix.net License AGPL-3             */
/*                                                                           */
/*  This program is free software: you can redistribute it and/or modify     */
/*  it under the terms of the GNU Affero General Public License as           */
/*  published by the Free Software Foundation, either version 3 of the       */
/*  License, or (at your option) any later version.                          */
/*                                                                           */
/*  This program is distributed in the hope that it will be useful,          */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of           */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            */
/*  GNU Affero General Public License for more details.a                     */
/*                                                                           */
/*  You should have received a copy of the GNU Affero General Public License */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*                                                                           */
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/statvfs.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>

#include "io_clownix.h"
#include "rpc_clownix.h"
#include "cfg_store.h"
#include "commun_daemon.h"
#include "pid_clone.h"
#include "dpdk_ovs.h"
#include "dpdk_dyn.h"
#include "dpdk_msg.h"
#include "dpdk_fmt.h"
#include "dpdk_tap.h"


/****************************************************************************/
int dpdk_fmt_tx_add_tap(int tid, char *name, int tap_id)
{
  int result;
  char cmd[MAX_PATH_LEN];
  memset(cmd, 0, MAX_PATH_LEN);
  snprintf(cmd, MAX_PATH_LEN-1, "cloonixovs_add_tap name=%s tap_id=%d",
           name, tap_id);
  result = dpdk_ovs_try_send_diag_msg(tid, cmd);
  return result;
}
/*--------------------------------------------------------------------------*/

/****************************************************************************/
int dpdk_fmt_tx_del_tap(int tid, char *name)
{
  int result;
  char cmd[MAX_PATH_LEN];
  memset(cmd, 0, MAX_PATH_LEN);
  snprintf(cmd, MAX_PATH_LEN-1, "cloonixovs_del_tap name=%s", name);
  result = dpdk_ovs_try_send_diag_msg(tid, cmd);
  return result;
}
/*--------------------------------------------------------------------------*/

/****************************************************************************/
int dpdk_fmt_tx_add_lan_tap(int tid, char *lan, char *name)
{
  int result;
  char cmd[MAX_PATH_LEN];
  memset(cmd, 0, MAX_PATH_LEN);
  snprintf(cmd, MAX_PATH_LEN-1, "cloonixovs_add_lan_tap lan_name=%s name=%s",
           lan, name);
  result = dpdk_ovs_try_send_diag_msg(tid, cmd);
  return result;
}
/*--------------------------------------------------------------------------*/

/****************************************************************************/
int dpdk_fmt_tx_del_lan_tap(int tid, char *lan, char *name)
{
  int result;
  char cmd[MAX_PATH_LEN];
  memset(cmd, 0, MAX_PATH_LEN);
  snprintf(cmd, MAX_PATH_LEN-1, "cloonixovs_del_lan_tap lan_name=%s name=%s",
           lan, name);
  result = dpdk_ovs_try_send_diag_msg(tid, cmd);
  return result;
}
/*--------------------------------------------------------------------------*/

/****************************************************************************/
int dpdk_fmt_tx_add_lan(int tid, char *lan, int spy)
{
  int result;
  char cmd[MAX_PATH_LEN];
  memset(cmd, 0, MAX_PATH_LEN);
  snprintf(cmd, MAX_PATH_LEN-1, "cloonixovs_add_lan lan_name=%s spy=%d",
                                lan, spy);
  result = dpdk_ovs_try_send_diag_msg(tid, cmd);
  return result;
}
/*--------------------------------------------------------------------------*/

/****************************************************************************/
int dpdk_fmt_tx_del_lan(int tid, char *lan, int spy)
{
  int result;
  char cmd[MAX_PATH_LEN];
  memset(cmd, 0, MAX_PATH_LEN);
  snprintf(cmd, MAX_PATH_LEN-1, "cloonixovs_del_lan lan_name=%s spy=%d",
                                lan, spy);
  result = dpdk_ovs_try_send_diag_msg(tid, cmd);
  return result;
}
/*--------------------------------------------------------------------------*/

/****************************************************************************/
int dpdk_fmt_tx_add_eth(int tid, t_vm *kvm, char *name, int num)
{
  int i, result;
  char cmd[2*MAX_PATH_LEN];
  char strmac[MAX_NAME_LEN];
  char *mc;
  memset(cmd, 0, 2*MAX_PATH_LEN);
  sprintf(cmd, "cloonixovs_add_eth name=%s num=%d vm_id=%d",
               name, num, kvm->kvm.vm_id);
  for (i=0; i<num; i++)
    {
    mc = kvm->kvm.eth_params[i].mac_addr;
    sprintf(strmac, " mac=%02X:%02X:%02X:%02X:%02X:%02X",
    mc[0]&0xFF, mc[1]&0xFF, mc[2]&0xFF, mc[3]&0xFF, mc[4]&0xFF, mc[5]&0xFF);
    if ((strlen(cmd) + strlen(strmac)) > 2*MAX_PATH_LEN)
      KOUT("%d %d", (int)strlen(cmd), (int)strlen(strmac));
    strcat(cmd, strmac);
    }
  result = dpdk_ovs_try_send_diag_msg(tid, cmd);
  return result;
}
/*--------------------------------------------------------------------------*/

/****************************************************************************/
int dpdk_fmt_tx_del_eth(int tid, char *name, int num)
{
  int result;
  char cmd[MAX_PATH_LEN];
  memset(cmd, 0, MAX_PATH_LEN);
  sprintf(cmd, "cloonixovs_del_eth name=%s num=%d", name, num);
  result = dpdk_ovs_try_send_diag_msg(tid, cmd);
  return result;
}
/*--------------------------------------------------------------------------*/

/****************************************************************************/
int dpdk_fmt_tx_add_lan_eth(int tid, char *lan_name, char *name, int num)
{
  int result;
  char cmd[MAX_PATH_LEN];
  memset(cmd, 0, MAX_PATH_LEN);
  snprintf(cmd, MAX_PATH_LEN-1,
  "cloonixovs_add_lan_eth lan_name=%s name=%s num=%d", lan_name, name, num);
  result = dpdk_ovs_try_send_diag_msg(tid, cmd);
  return result;
}

/*--------------------------------------------------------------------------*/

/****************************************************************************/
int dpdk_fmt_tx_del_lan_eth(int tid, char *lan_name, char *name, int num)
{
  int result;
  char cmd[MAX_PATH_LEN];
  memset(cmd, 0, MAX_PATH_LEN);
  snprintf(cmd, MAX_PATH_LEN-1,
  "cloonixovs_del_lan_eth lan_name=%s name=%s num=%d", lan_name, name, num);
  result = dpdk_ovs_try_send_diag_msg(tid, cmd);
  return result;
}
/*--------------------------------------------------------------------------*/

/****************************************************************************/
void dpdk_fmt_rx_rpct_recv_diag_msg(int llid, int tid, char *line)
{
  int num, spy;
  char lan_name[MAX_NAME_LEN];
  char name[MAX_NAME_LEN];
  if (sscanf(line,
      "KO cloonixovs_add_eth name=%s num=%d", name, &num) == 2)
    {
    KERR("%s", line);
    dpdk_msg_ack_eth(tid, name, num, 1, 1, "KO");
    }
  else if (sscanf(line,
      "OK cloonixovs_add_eth name=%s num=%d", name, &num) == 2)
    {
    dpdk_msg_ack_eth(tid, name, num, 1, 0, "OK");
    }
  else if (sscanf(line,
      "KO cloonixovs_del_eth name=%s num=%d", name, &num) == 2)
    {
    KERR("%s", line);
    dpdk_msg_ack_eth(tid, name, num, 0, 1, "KO");
    }
  else if (sscanf(line,
      "OK cloonixovs_del_eth name=%s num=%d", name, &num) == 2)
    {
    dpdk_msg_ack_eth(tid, name, num, 0, 0, "OK");
    }
  else if (sscanf(line,
      "KO cloonixovs_add_lan lan_name=%s spy=%d", lan_name, &spy) == 2)
    {
    KERR("%s", line);
    dpdk_msg_ack_lan(tid, lan_name, 1, 1, "KO");
    }
  else if (sscanf(line,
      "OK cloonixovs_add_lan lan_name=%s spy=%d", lan_name, &spy) == 2)
    {
    dpdk_msg_ack_lan(tid, lan_name, 1, 0, "OK");
    }
  else if (sscanf(line,
      "KO cloonixovs_del_lan lan_name=%s spy=%d", lan_name, &spy) == 2)
    {
    KERR("%s", line);
    dpdk_msg_ack_lan(tid, lan_name, 0, 1, "KO");
    }
  else if (sscanf(line,
      "OK cloonixovs_del_lan lan_name=%s spy=%d", lan_name, &spy) == 2)
    {
    dpdk_msg_ack_lan(tid, lan_name, 0, 0, "OK");
    }
  else if (sscanf(line,
      "KO cloonixovs_add_lan_eth lan_name=%s name=%s num=%d",
      lan_name, name, &num) == 3)
    dpdk_msg_ack_lan_endp(tid, lan_name, name, num, 1, 1, "KO");
  else if (sscanf(line,
      "OK cloonixovs_add_lan_eth lan_name=%s name=%s num=%d",
      lan_name, name, &num) == 3)
    dpdk_msg_ack_lan_endp(tid, lan_name, name, num, 1, 0, "OK");
  else if (sscanf(line,
      "KO cloonixovs_del_lan_eth lan_name=%s name=%s num=%d",
      lan_name, name, &num) == 3)
    dpdk_msg_ack_lan_endp(tid, lan_name, name, num, 0, 1, "KO");
  else if (sscanf(line,
       "OK cloonixovs_del_lan_eth lan_name=%s name=%s num=%d",
      lan_name, name, &num) == 3)
    dpdk_msg_ack_lan_endp(tid, lan_name, name, num, 0, 0, "OK");
  else if (sscanf(line, "OK cloonixovs_add_tap name=%s", name) == 1)
    dpdk_tap_resp_add(0, name);
  else if (sscanf(line, "KO cloonixovs_add_tap name=%s", name) == 1)
    dpdk_tap_resp_add(1, name);
  else if (sscanf(line, "OK cloonixovs_del_tap name=%s", name) == 1)
    dpdk_tap_resp_del(name);
  else if (sscanf(line, "OK cloonixovs_add_lan_tap lan_name=%s name=%s",
                         lan_name, name) == 2)
    dpdk_msg_ack_lan_endp(tid, lan_name, name, 0, 1, 0, "OK");
  else if (sscanf(line, "KO cloonixovs_add_lan_tap lan_name=%s name=%s",
                         lan_name, name) == 2)
    dpdk_msg_ack_lan_endp(tid, lan_name, name, 0, 1, 1, "KO");
  else if (sscanf(line, "OK cloonixovs_del_lan_tap lan_name=%s name=%s", 
                         lan_name, name) == 2)
    dpdk_msg_ack_lan_endp(tid, lan_name, name, 0, 0, 0, "OK");
  else if (sscanf(line, "KO cloonixovs_del_lan_tap lan_name=%s name=%s",
                         lan_name, name) == 2)
    dpdk_msg_ack_lan_endp(tid, lan_name, name, 0, 0, 1, "KO");
  else
    KERR("%s", line);
}
/*--------------------------------------------------------------------------*/

/****************************************************************************/
void dpdk_fmt_rx_rpct_recv_evt_msg(int llid, int tid, char *line)
{
  int ms, tidx, ptx, btx, prx, brx, vm_id, num;
  t_vm *vm;
  if (sscanf(line,"endp_eventfull_tx_rx %u %d %d %d %d %d",
                   &ms, &tidx, &ptx, &btx, &prx, &brx) == 6)
    {
    vm_id = tidx/MAX_VM;
    num = tidx%MAX_VM;
    vm = find_vm_with_id(vm_id);
    if (vm && (num >= 0) && (num < vm->kvm.nb_dpdk))
      dpdk_ovs_fill_eventfull(vm->kvm.name, num, ms, ptx, prx, btx, brx);
    else if (dpdk_tap_eventfull(vm_id, num, ms, ptx, btx, prx, brx))
      KERR("%d %d %d", tidx, vm_id, num);
    }
    else
      KERR("%s", line);
}
/*--------------------------------------------------------------------------*/

