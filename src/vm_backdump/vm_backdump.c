/*
 * vm_backdump.c - vmware backdoor commands dump tool
 *
 * Copyright (C) 2016 Pedro Mendes da Silva
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#include <string.h>
#include <stdio.h>

/* Some includes from the open_vm_tools project */
#include "vm_basic_types.h"
#include "backdoor_defs.h"


#include "vsockets_tools_version.h"


/* 
 * Description strings for each Backdoor command 
 */

char *backdoor_cmd_strs[BDOOR_CMD_MAX+1];

void Backdump_init_strings()
{
	backdoor_cmd_strs[	BDOOR_CMD_APMFUNCTION	] =	"BDOOR_CMD_APMFUNCTION";
	backdoor_cmd_strs[	BDOOR_CMD_GETDISKGEO	] =	"BDOOR_CMD_GETDISKGEO";
	backdoor_cmd_strs[	BDOOR_CMD_GETPTRLOCATION	] =	"BDOOR_CMD_GETPTRLOCATION";
	backdoor_cmd_strs[	BDOOR_CMD_SETPTRLOCATION	] =	"BDOOR_CMD_SETPTRLOCATION";
	backdoor_cmd_strs[	BDOOR_CMD_GETSELLENGTH	] =	"BDOOR_CMD_GETSELLENGTH";
	backdoor_cmd_strs[	BDOOR_CMD_GETNEXTPIECE	] =	"BDOOR_CMD_GETNEXTPIECE";
	backdoor_cmd_strs[	BDOOR_CMD_SETSELLENGTH	] =	"BDOOR_CMD_SETSELLENGTH";
	backdoor_cmd_strs[	BDOOR_CMD_SETNEXTPIECE	] =	"BDOOR_CMD_SETNEXTPIECE";
	backdoor_cmd_strs[	BDOOR_CMD_GETVERSION	] =	"BDOOR_CMD_GETVERSION";
	backdoor_cmd_strs[	BDOOR_CMD_GETDEVICELISTELEMENT	] =	"BDOOR_CMD_GETDEVICELISTELEMENT";
	backdoor_cmd_strs[	BDOOR_CMD_TOGGLEDEVICE	] =	"BDOOR_CMD_TOGGLEDEVICE";
	backdoor_cmd_strs[	BDOOR_CMD_GETGUIOPTIONS	] =	"BDOOR_CMD_GETGUIOPTIONS";
	backdoor_cmd_strs[	BDOOR_CMD_SETGUIOPTIONS	] =	"BDOOR_CMD_SETGUIOPTIONS";
	backdoor_cmd_strs[	BDOOR_CMD_GETSCREENSIZE	] =	"BDOOR_CMD_GETSCREENSIZE";
	backdoor_cmd_strs[	BDOOR_CMD_MONITOR_CONTROL	] =	"BDOOR_CMD_MONITOR_CONTROL";
	backdoor_cmd_strs[	BDOOR_CMD_GETHWVERSION	] =	"BDOOR_CMD_GETHWVERSION";
	backdoor_cmd_strs[	BDOOR_CMD_OSNOTFOUND	] =	"BDOOR_CMD_OSNOTFOUND";
	backdoor_cmd_strs[	BDOOR_CMD_GETUUID	] =	"BDOOR_CMD_GETUUID";
	backdoor_cmd_strs[	BDOOR_CMD_GETMEMSIZE	] =	"BDOOR_CMD_GETMEMSIZE";
	backdoor_cmd_strs[	BDOOR_CMD_HOSTCOPY	] =	"BDOOR_CMD_HOSTCOPY";
	backdoor_cmd_strs[	BDOOR_CMD_GETTIME	] =	"BDOOR_CMD_GETTIME";
	backdoor_cmd_strs[	BDOOR_CMD_STOPCATCHUP	] =	"BDOOR_CMD_STOPCATCHUP";
	backdoor_cmd_strs[	BDOOR_CMD_PUTCHR	] =	"BDOOR_CMD_PUTCHR";
	backdoor_cmd_strs[	BDOOR_CMD_ENABLE_MSG	] =	"BDOOR_CMD_ENABLE_MSG";
	backdoor_cmd_strs[	BDOOR_CMD_GOTO_TCL	] =	"BDOOR_CMD_GOTO_TCL";
	backdoor_cmd_strs[	BDOOR_CMD_INITPCIOPROM	] =	"BDOOR_CMD_INITPCIOPROM";
	backdoor_cmd_strs[	BDOOR_CMD_MESSAGE	] =	"BDOOR_CMD_MESSAGE";
	backdoor_cmd_strs[	BDOOR_CMD_SIDT	] =	"BDOOR_CMD_SIDT";
	backdoor_cmd_strs[	BDOOR_CMD_SGDT	] =	"BDOOR_CMD_SGDT";
	backdoor_cmd_strs[	BDOOR_CMD_SLDT_STR	] =	"BDOOR_CMD_SLDT_STR";
	backdoor_cmd_strs[	BDOOR_CMD_ISACPIDISABLED	] =	"BDOOR_CMD_ISACPIDISABLED";
	backdoor_cmd_strs[	BDOOR_CMD_ISMOUSEABSOLUTE	] =	"BDOOR_CMD_ISMOUSEABSOLUTE";
	backdoor_cmd_strs[	BDOOR_CMD_PATCH_SMBIOS_STRUCTS	] =	"BDOOR_CMD_PATCH_SMBIOS_STRUCTS";
	backdoor_cmd_strs[	BDOOR_CMD_MAPMEM	] =	"BDOOR_CMD_MAPMEM";
	backdoor_cmd_strs[	BDOOR_CMD_ABSPOINTER_DATA	] =	"BDOOR_CMD_ABSPOINTER_DATA";
	backdoor_cmd_strs[	BDOOR_CMD_ABSPOINTER_STATUS	] =	"BDOOR_CMD_ABSPOINTER_STATUS";
	backdoor_cmd_strs[	BDOOR_CMD_ABSPOINTER_COMMAND	] =	"BDOOR_CMD_ABSPOINTER_COMMAND";
	backdoor_cmd_strs[	BDOOR_CMD_PATCH_ACPI_TABLES	] =	"BDOOR_CMD_PATCH_ACPI_TABLES";
	backdoor_cmd_strs[	BDOOR_CMD_GETHZ	] =	"BDOOR_CMD_GETHZ";
	backdoor_cmd_strs[	BDOOR_CMD_GETTIMEFULL	] =	"BDOOR_CMD_GETTIMEFULL";
	
	backdoor_cmd_strs[	BDOOR_CMD_CHECKFORCEBIOSSETUP	] =	"BDOOR_CMD_CHECKFORCEBIOSSETUP";
	backdoor_cmd_strs[	BDOOR_CMD_LAZYTIMEREMULATION	] =	"BDOOR_CMD_LAZYTIMEREMULATION";
	backdoor_cmd_strs[	BDOOR_CMD_BIOSBBS	] =	"BDOOR_CMD_BIOSBBS";
	backdoor_cmd_strs[	BDOOR_CMD_ISGOSDARWIN	] =	"BDOOR_CMD_ISGOSDARWIN";
	backdoor_cmd_strs[	BDOOR_CMD_DEBUGEVENT	] =	"BDOOR_CMD_DEBUGEVENT";
	backdoor_cmd_strs[	BDOOR_CMD_OSNOTMACOSXSERVER	] =	"BDOOR_CMD_OSNOTMACOSXSERVER";
	backdoor_cmd_strs[	BDOOR_CMD_GETTIMEFULL_WITH_LAG	] =	"BDOOR_CMD_GETTIMEFULL_WITH_LAG";
	backdoor_cmd_strs[	BDOOR_CMD_ACPI_HOTPLUG_DEVICE	] =	"BDOOR_CMD_ACPI_HOTPLUG_DEVICE";
	backdoor_cmd_strs[	BDOOR_CMD_ACPI_HOTPLUG_MEMORY	] =	"BDOOR_CMD_ACPI_HOTPLUG_MEMORY";
	backdoor_cmd_strs[	BDOOR_CMD_ACPI_HOTPLUG_CBRET	] =	"BDOOR_CMD_ACPI_HOTPLUG_CBRET";
	backdoor_cmd_strs[	BDOOR_CMD_ACPI_HOTPLUG_CPU	] =	"BDOOR_CMD_ACPI_HOTPLUG_CPU";
	backdoor_cmd_strs[	BDOOR_CMD_XPMODE	] =	"BDOOR_CMD_XPMODE";
	backdoor_cmd_strs[	BDOOR_CMD_NESTING_CONTROL	] =	"BDOOR_CMD_NESTING_CONTROL";
	backdoor_cmd_strs[	BDOOR_CMD_FIRMWARE_INIT	] =	"BDOOR_CMD_FIRMWARE_INIT";
	backdoor_cmd_strs[	BDOOR_CMD_FIRMWARE_ACPI_SERVICES	] =	"BDOOR_CMD_FIRMWARE_ACPI_SERVICES";
	backdoor_cmd_strs[	BDOOR_CMD_SENDPSHAREHINTS	] =	"BDOOR_CMD_SENDPSHAREHINTS";
	backdoor_cmd_strs[	BDOOR_CMD_ENABLE_USB_MOUSE	] =	"BDOOR_CMD_ENABLE_USB_MOUSE";
	backdoor_cmd_strs[	BDOOR_CMD_GET_VCPU_INFO	] =	"BDOOR_CMD_GET_VCPU_INFO";
	backdoor_cmd_strs[	BDOOR_CMD_EFI_SERIALCON_CONFIG	] =	"BDOOR_CMD_EFI_SERIALCON_CONFIG";
	backdoor_cmd_strs[	BDOOR_CMD_BUG328986	] =	"BDOOR_CMD_BUG328986";
	backdoor_cmd_strs[	BDOOR_CMD_FIRMWARE_ERROR	] =	"BDOOR_CMD_FIRMWARE_ERROR";
	backdoor_cmd_strs[	BDOOR_CMD_VMK_INFO	] =	"BDOOR_CMD_VMK_INFO";
	backdoor_cmd_strs[	BDOOR_CMD_EFI_BOOT_CONFIG	] =	"BDOOR_CMD_EFI_BOOT_CONFIG";
	backdoor_cmd_strs[	BDOOR_CMD_GET_HW_MODEL	] =	"BDOOR_CMD_GET_HW_MODEL";
	backdoor_cmd_strs[	BDOOR_CMD_GET_SVGA_CAPABILITIES	] =	"BDOOR_CMD_GET_SVGA_CAPABILITIES";
	backdoor_cmd_strs[	BDOOR_CMD_SET_PCI_HOLE	] =	"BDOOR_CMD_SET_PCI_HOLE";
	backdoor_cmd_strs[	BDOOR_CMD_GET_PCI_HOLE	] =	"BDOOR_CMD_GET_PCI_HOLE";
	backdoor_cmd_strs[	BDOOR_CMD_GET_PCI_BAR	] =	"BDOOR_CMD_GET_PCI_BAR";
	backdoor_cmd_strs[	BDOOR_CMD_SHOULD_GENERATE_SYSTEMID	] =	"BDOOR_CMD_SHOULD_GENERATE_SYSTEMID";
	backdoor_cmd_strs[	BDOOR_CMD_READ_DEBUG_FILE	] =	"BDOOR_CMD_READ_DEBUG_FILE";
	backdoor_cmd_strs[	BDOOR_CMD_SCREENSHOT	] =	"BDOOR_CMD_SCREENSHOT";
	backdoor_cmd_strs[	BDOOR_CMD_INJECT_KEY	] =	"BDOOR_CMD_INJECT_KEY";
	backdoor_cmd_strs[	BDOOR_CMD_INJECT_MOUSE	] =	"BDOOR_CMD_INJECT_MOUSE";
	backdoor_cmd_strs[	BDOOR_CMD_MKS_GUEST_STATS	] =	"BDOOR_CMD_MKS_GUEST_STATS";
	backdoor_cmd_strs[	BDOOR_CMD_ABSPOINTER_RESTRICT	] =	"BDOOR_CMD_ABSPOINTER_RESTRICT";
	backdoor_cmd_strs[	BDOOR_CMD_GUESTINTEGRITY	] =	"BDOOR_CMD_GUESTINTEGRITY";
	backdoor_cmd_strs[	BDOOR_CMD_MKSSTATS_SNAPSHOT	] =	"BDOOR_CMD_MKSSTATS_SNAPSHOT";
	backdoor_cmd_strs[	BDOOR_CMD_MAX	] =	"BDOOR_CMD_MAX";
}


/* 
 * Utilily print functions
 */

void Backdump_print_assembly_instruction(int32 rEDX, int32 rEAX)
{
	printf("\nIN: ===> [ASM] IN DX, EAX ;--- DX=0x%04hx (backdoor port) , EAX=0x%08x (backdoor cmd)\n", (unsigned short) rEDX & 0xFFFF, rEAX);
}

void Backdump_print_registers(   uint32 r_EAX, uint32 r_EBX, uint32 r_ECX, uint32 r_EDX, char *direction_str)
{

   printf("%s   ", direction_str);
   printf("EAX=%08x   ", r_EAX);
   printf("EBX=%08x   ", r_EAX);
   printf("ECX=%08x   ", r_EAX);
   printf("EDX=%08x   \n", r_EAX);
	
}

int
Backdump_Hostinfo_NestingSupported(void)
{
#if defined(__i386__) || defined(__x86_64__)
   uint32 cmd = NESTING_CONTROL_QUERY << 16 | BDOOR_CMD_NESTING_CONTROL;
   uint32 result;
   uint32 result_b, result_c, result_d;

   printf("\nBackdoor Command(%s) --- IN: ", "NestingSupported - BDOOR_CMD_NESTING_CONTROL");
   //printf("cmd=%08x\n", cmd);

   Backdump_print_assembly_instruction(BDOOR_PORT, cmd);

   __asm__ __volatile__(
      "inl %%dx, %%eax"
      : "=a" (result), "=b" (result_b) , "=c" (result_c) , "=d" (result_d) 
      :	"0"  (BDOOR_MAGIC),
        "c"  (cmd),
        "d"  (BDOOR_PORT)
   );

   Backdump_print_registers(result, result_b, result_c, result_d, "OUT:");
   
   if (result >= NESTING_CONTROL_QUERY && result != ~0U) {
      return TRUE;
   }
#endif
   return FALSE;
}


/* 
 * Dump functions
 */

int
Backdump_Hostinfo_NestingControl(short subcmd)
{
#if defined(__i386__) || defined(__x86_64__)
   uint32 cmd = (subcmd << 16) | BDOOR_CMD_NESTING_CONTROL;
   uint32 result;
   uint32 result_b, result_c, result_d;

   printf("\nBackdoor Command(%s) --- IN: ", "NestingControl - BDOOR_CMD_NESTING_CONTROL");
   //printf("cmd=%08x\n", cmd);
   Backdump_print_assembly_instruction(BDOOR_PORT, cmd);

   __asm__ __volatile__(
      "inl %%dx, %%eax"
      : "=a" (result), "=b" (result_b) , "=c" (result_c) , "=d" (result_d) 
      :	"0"  (BDOOR_MAGIC),
        "b" (0),
        "c"  (cmd),
        "d"  (BDOOR_PORT)
   );

   Backdump_print_registers(result, result_b, result_c, result_d, "OUT:");
   
   /*
   printf("OUT: ");
   printf("EAX=%08x   ", result);
   printf("EBX=%08x   ", result_b);
   printf("ECX=%08x   ", result_c);
   printf("EDX=%08x\n", result_d);
   */

   if (result >= NESTING_CONTROL_QUERY && result != ~0U) {
      return TRUE;
   }
#endif
   return FALSE;
}

int
Backdump_Hostinfo_BackdoorCommand(uint32 cmd, char * str)
{
#if defined(__i386__) || defined(__x86_64__)
   uint32 result;
   uint32 result_b, result_c, result_d;

   printf("\nBackdoor Cmd(%s) ", str);
   //printf("cmd hex=0x%08x decimal=%08d\n", cmd, cmd);
   Backdump_print_assembly_instruction(BDOOR_PORT, cmd);


   __asm__ __volatile__(
      "inl %%dx, %%eax"
      : "=a" (result), "=b" (result_b) , "=c" (result_c) , "=d" (result_d) 
      :	"0"  (BDOOR_MAGIC),
        "b" (0),
        "c"  (cmd),
        "d"  (BDOOR_PORT)
   );

   /* Backdump_print_registers(result, result_b, result_c, result_d, "OUT:"); */
   
   printf("OUT: ");
   printf("EAX=%08x %s   ", result, (result==BDOOR_MAGIC) ? "(Unchanged)" :((result != ~0U) ? "(OK)" : "(ERR)"));
   printf("EBX=%08x %s   ", result_b, (result_b!=0) ? "(Changed)" : "(Unchanged)");
   printf("ECX=%08x %s   ", result_c, (result_c!=cmd) ? "<==(Changed)" : "(Unchanged)" );
   printf("EDX=%08x %s   \n", result_d, (result_d==BDOOR_PORT) ? "(Unchanged)" : "<==Changed");

  return TRUE;

#endif
   return FALSE;
}


int
Backdump_Hostinfo_VCPUInfoBackdoor(unsigned bit)
{
#if defined(__i386__) || defined(__x86_64__)
   uint32 result;
   uint32 result_b, result_c, result_d;

   printf("\nVCPUInfoBackdoor\n");
   //printf("cmd=%08x\n", BDOOR_CMD_GET_VCPU_INFO);
   Backdump_print_assembly_instruction(BDOOR_PORT, BDOOR_CMD_GET_VCPU_INFO);

   __asm__ __volatile__(
      "inl %%dx, %%eax"
      : "=a" (result), "=b" (result_b) , "=c" (result_c) , "=d" (result_d) 
      :	"0"  (BDOOR_MAGIC),
        "c"  (BDOOR_CMD_GET_VCPU_INFO),
        "d"  (BDOOR_PORT)
   );

   Backdump_print_registers(result, result_b, result_c, result_d, "OUT:");

   /*
   printf("OUT: ");
   printf("EAX=%08x   ", result);
   printf("EBX=%08x   ", result_b);
   printf("ECX=%08x   ", result_c);
   printf("EDX=%08x\n", result_d);
   */

   if (result & (1 << BDOOR_CMD_VCPU_SLC64) )
   {
      printf("BDOOR_CMD_VCPU_SLC64 supported\n");
   }
   if (result & (1 << BDOOR_CMD_VCPU_SYNC_VTSCS) )
   {
      printf("BDOOR_CMD_VCPU_SYNC_VTSCS supported\n");
   }
   if (result & (1 << BDOOR_CMD_VCPU_HV_REPLAY_OK) )
   {
      printf("BDOOR_CMD_VCPU_HV_REPLAY_OK supported\n");
   }
   if (result & (1 << BDOOR_CMD_VCPU_LEGACY_X2APIC_OK) )
   {
      printf("BDOOR_CMD_VCPU_LEGACY_X2APIC_OK supported\n");
   }
   if (result & (1 << BDOOR_CMD_VCPU_MMIO_HONORS_PAT) )
   {
      printf("BBDOOR_CMD_VCPU_MMIO_HONORS_PAT supported\n");
   }



   printf("BDOOR_CMD_VCPU_RESERVED=%04x\n", BDOOR_CMD_VCPU_RESERVED);
   printf("(1 << BDOOR_CMD_VCPU_RESERVED)=%04x\n", (1 << BDOOR_CMD_VCPU_RESERVED));
   printf("(1 << bit)=%04x\n", (1 << bit));
  
   /* If reserved bit is 1, this command wasn't implemented. */
   return (result & (1 << BDOOR_CMD_VCPU_RESERVED)) == 0 &&

          (result & (1 << bit))                     != 0;
#endif
   return FALSE;
}

int
Backdump_Hostinfo_NestedHVReplaySupported(void)
{
#if defined(__i386__) || defined(__x86_64__)
//   printf("BDOOR_CMD_VCPU_HV_REPLAY_OK=%04x\n", BDOOR_CMD_VCPU_HV_REPLAY_OK);

   return Backdump_Hostinfo_VCPUInfoBackdoor(BDOOR_CMD_VCPU_HV_REPLAY_OK);
#else
   return FALSE;
#endif
}

/* 
 * Backdump_DumpNestingRelatedCommands
 * 
 * Some backdoor commands seem to be related with hypervisor nesting
 *


 Test with 1 machine in a regular hypervisor and 1 machine in a nested hypervisor

			BackdoorCommand(brute force)					BackdoorCommand(brute force
			)
			cmd=0x00000029 00000041						cmd=0x00000029 00000041
			EAX=00000001 (Success)					      |	EAX=00000000 (Success)
			EBX=00000000 (Unchanged)					EBX=00000000 (Unchanged)
			ECX=00000029 (Unchanged)					ECX=00000029 (Unchanged)
			EDX=00005658 (Unchanged)					EDX=00005658 (Unchanged)

			Diffs

			- SIDT, ...
			- GetFullTime
			- Abs Pointer Cmd

*/
void Backdump_DumpNestingRelatedCommands()
{
    int res;

	printf("@@@@@ Backdump_Dump Hypervisor Nesting Related Commands\n");
	printf("@@@@@ \n\n");

	res = Backdump_Hostinfo_NestingSupported();
	printf("Nesting supported=%d\n", res);

	Backdump_Hostinfo_NestingControl(0);
	Backdump_Hostinfo_NestingControl(1);
	Backdump_Hostinfo_NestingControl(2);

	res = Backdump_Hostinfo_NestedHVReplaySupported();

	printf("NestedHVReplaySupported supported=%d\n", res);

	Backdump_Hostinfo_BackdoorCommand(BDOOR_CMD_SIDT, "BDOOR_CMD_SIDT");
	Backdump_Hostinfo_BackdoorCommand(BDOOR_CMD_SGDT, "BDOOR_CMD_SGDT");

	Backdump_Hostinfo_BackdoorCommand(BDOOR_CMD_SLDT_STR,"BDOOR_CMD_SLDT_STR");
	Backdump_Hostinfo_BackdoorCommand(BDOOR_CMD_ISACPIDISABLED,"BDOOR_CMD_ISACPIDISABLED");
	Backdump_Hostinfo_BackdoorCommand(BDOOR_CMD_ABSPOINTER_DATA,"BDOOR_CMD_ABSPOINTER_DATA");
	Backdump_Hostinfo_BackdoorCommand(BDOOR_CMD_ABSPOINTER_STATUS,"BDOOR_CMD_ABSPOINTER_STATUS");
	Backdump_Hostinfo_BackdoorCommand(BDOOR_CMD_ABSPOINTER_COMMAND,"BDOOR_CMD_ABSPOINTER_COMMAND");
	Backdump_Hostinfo_BackdoorCommand(BDOOR_CMD_GETHZ,"BDOOR_CMD_GETHZ");
	Backdump_Hostinfo_BackdoorCommand(BDOOR_CMD_GETTIMEFULL,"BDOOR_CMD_GETTIMEFULL");
	Backdump_Hostinfo_BackdoorCommand(BDOOR_CMD_ISGOSDARWIN,"BDOOR_CMD_ISGOSDARWIN");
	Backdump_Hostinfo_BackdoorCommand(BDOOR_CMD_DEBUGEVENT,"BDOOR_CMD_DEBUGEVENT");
	Backdump_Hostinfo_BackdoorCommand(BDOOR_CMD_GETTIMEFULL_WITH_LAG,"BDOOR_CMD_GETTIMEFULL_WITH_LAG");
	Backdump_Hostinfo_BackdoorCommand(BDOOR_CMD_ENABLE_USB_MOUSE,"BDOOR_CMD_ENABLE_USB_MOUSE");
	Backdump_Hostinfo_BackdoorCommand(BDOOR_CMD_VMK_INFO,"BDOOR_CMD_VMK_INFO");
	Backdump_Hostinfo_BackdoorCommand(BDOOR_CMD_ABSPOINTER_RESTRICT,"BDOOR_CMD_ABSPOINTER_RESTRICT");
	Backdump_Hostinfo_BackdoorCommand(BDOOR_CMD_GUESTINTEGRITY,"BDOOR_CMD_GUESTINTEGRITY");
	
	printf("\n\n@@@@@<<<<<\n\n");
}

void Backdump_BruteForceDumpAllCommands()
{
	printf("@@@@@ Backdump_Dump all backdoor commands (even unknown/undocumented)\n");
	printf("@@@@@ \n\n");

	int i;

	for(i=1;i<BDOOR_CMD_MAX;i++) {
	 Backdump_Hostinfo_BackdoorCommand(i, backdoor_cmd_strs[i]	);
	}

	printf("\n\n@@@@@ End (Backdump_Dump all backdoor commands) <<<<<\n\n");
}

void Backdump_PrintBanner()
{
	printf("vm_backdump : vmware backdoor commands dump tool \n");
	printf("=================================================\n");
	printf("   %s %s\n", BUILD_DATE_STRING, PLATFORM_STRING);
	printf("=================================================\n");
	
	Backdump_Hostinfo_BackdoorCommand(BDOOR_CMD_GETVERSION, "Get Version (Hypervisor version)");
	
	printf("\n=================================================\n\n");
}

int main()
{
	Backdump_init_strings();
	
	Backdump_PrintBanner();

	Backdump_BruteForceDumpAllCommands();

	Backdump_DumpNestingRelatedCommands();

	return 0;
}


