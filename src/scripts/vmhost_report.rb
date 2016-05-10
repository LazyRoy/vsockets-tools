require_relative "vmware_hypervisor_version.rb"

def check_running_user

	current_user = `whoami`

	print "current user=", current_user,"\n"

	if current_user <=> "root" then
		print "===> Running as root, all info available\n"
	else
		print "===> WARNING: Running as unprivileged user, not all info will be available\n"
	end

end

def vm_storage_stats

	print "#### VM storage stats:\n"

	system "vmware-toolbox-cmd stat raw text vscsi scsi0:0"

	system "vmware-toolbox-cmd stat raw text vscsi ide0:0"

# TODO: fix hardcoded devices

end

def vm_networking_stats

	print "#### VM networking stats:\n"

# TODO: fix hardcoded devices

	system "vmware-toolbox-cmd stat raw text vnet 00:0c:29:1e:23:f3"
	system "vmware-toolbox-cmd stat raw text vnet 00:0c:29:32:0a:6a"

	print "#### VM ARP networking stats:\n"
	system "arp"
	system "/bin/esxcfg-route -n"

	print "#### VM IP routing networking stats:\n"
	system "route"
	system "/bin/esxcfg-route -l"

	print "#### VM IP networking stats:\n"
	system "ifconfig"


end

def vm_bios_stats

	print "#### BIOS strings:\n"
	system "dmidecode|grep -i vmware"
	system "esxcfg-info -w | head -30"

	print "#### BIOS signature:\n"
	system "dmidecode|head -16 | tail -10"

	print "#### BIOS Release date:\n"
	system "dmidecode|grep \"Release Date:\""

	print "#### PCI strings:\n"
	 system "lspci"

	print "#### USB devices:\n"
	system "lsusb"
	
	print "#### CPU features:\n"
	system "lscpu"
	system "cat /proc/cpuinfo"
	
	print "#### Kernel modules:\n"
	system "lsmod | grep vm"
	system "esxcfg-module  -l"

end

def vmguest_backdoor_info 

	print "#### VM Guest backdoor low-level info\n"
	print "#### ... Host screen size\n"
	system "vmw s -d -v"

	print "#### ... BIOS UUID\n"
	system "vmw u -v -d"

	print "#### ... Paste from clipboard\n"
	system "vmw p -v -d"

	print "#### ... List connectable devices\n"
	system "vmw d -v -d"

	print "#### ... RPC: machine.id.get\n"
	system "vmw rpc -e machine.id.get -v"
	print "#### ... RPC: guestInfo\n"
	print "#### ..... info-get guestinfo.joebanana\n"
	system "vmw r \"info-get guestinfo.joebanana\" -v"
	print "\n"
	print "#### ..... info-set guestinfo.joebanana\n"
	system "vmw r \"info-set guestinfo.joebanana banana\" -v"
	print "\n"
	print "#### ..... info-get guestinfo.joebanana\n"
	system "vmw r \"info-get guestinfo.joebanana\" -v"
	print "\n"
	print "#### ... RPC: log\n"
	print "#### ..... log\n"
	system "vmw r \"log joebanana\" -v"
	print "#### ..... log -e\n"
	system "vmw r -e \"log joebanana\" -v"

end


def vm_network_basic_scan

# TODO: fix hardcoded devices
# get network address+mask from args

	print "#### VM network basic scan:\n"
	print "#### --- scanning for interesting services:\n"
	system "nmap -sV --version-all -p 902,912,80,443 192.168.189.0/24  | grep -v closed"

	print "#### --- scanning for interesting VMware operating systems:\n"
	system "nmap -vv -O --osscan-guess -p 902,912,80,443,8080 192.168.189.0/24 | grep 'Nmap scan report for\|guesses\|Running\|Too many fingerprints' | grep -v 'host down'"

# TODO: return EXSi servers, VMware player brigdes, servers

end

def vm_esxi_basic_scan esxi_host

print "### ESXi basic scan:\n"
print "### ...ESXi:", esxi_host,  "\n"

print "### ..... SSL certificate\n"
system "echo -n | openssl s_client -connect "+esxi_host+":443 2>/dev/null | openssl x509 -text -noout -issuer -subject"

print "### ..... API\n"
system "wget --server-response --no-check-certificate -O - https://"+esxi_host+"/sdk/vimService.wsdl"

end

### MAIN

print "#### VMware Host Fingerprinting Report from Guest\n"
print "####\n"
print "#### (Starting at ", `date`.strip, ")\n"

check_running_user

print "#### Available stats:\n"

stats_raw = `vmware-toolbox-cmd stat raw`

print "stats_raw=",stats_raw,"\n"

if stats_raw=="" then
  print "stats raw not available\n"
end;

print "#### Session stats:\n"

system "vmware-toolbox-cmd stat raw text session"

print "#### Host stats:\n"

system "vmware-toolbox-cmd stat raw text host"

print "#### VM stats:\n"

system "vmware-toolbox-cmd stat raw text resources"


print "#### VM check stats (legacy)\n"
print "#### -- Hypervisor type:\n"
system "vmware-checkvm -p"
print "#### -- VM hardware version:\n"
system "vmware-checkvm -h"
system "vmware-checkvm -r"

check_vm_host_version


print "#### VM Guest stats (legacy)\n"

system "vmware-toolbox-cmd stat hosttime"
system "vmware-toolbox-cmd stat speed"
system "vmware-toolbox-cmd stat sessionid"
system "vmware-toolbox-cmd stat balloon"
system "vmware-toolbox-cmd stat swap"
system "vmware-toolbox-cmd stat memlimit"
system "vmware-toolbox-cmd stat memres"
system "vmware-toolbox-cmd stat cpures"
system "vmware-toolbox-cmd stat cpulimit"
 


print "#### VM Guest devices (visible if isolation is disabled in .vmx)\n"
system "vmware-toolbox-cmd device list"

print "#### VM Guest virtual drive mountpoints\n"
system "vmware-toolbox-cmd disk list"

vm_storage_stats

vm_networking_stats

vm_bios_stats

print "#### VM Guest operation scripts\n"
print "#### ... power\n"
system "vmware-toolbox-cmd script power current"
print "#### ... resume\n"
system "vmware-toolbox-cmd script resume current"
print "#### ... suspend\n"
system "vmware-toolbox-cmd script suspend current"
print "#### ... shutdown\n"
system "vmware-toolbox-cmd script shutdown current"


print "#### VM Guest tools info\n"
system "vmtoolsd -v"

print "#### Host VM shares (HGFS):\n"
system "vmware-hgfsclient"


vmguest_backdoor_info

vm_network_basic_scan

vm_esxi_basic_scan "192.168.189.134"

print "#### (Ending at ", `date`.strip, ")\n"



