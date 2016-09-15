require_relative "vmware_hypervisor_version.rb"
require 'ostruct'
#require "./vmware_hypervisor_version.rb"

# TODO: abstract Windows commands
module OS
  def OS.windows?
    (/cygwin|mswin|mingw|bccwin|wince|emx/ =~ RUBY_PLATFORM) != nil
  end

  def OS.mac?
   (/darwin/ =~ RUBY_PLATFORM) != nil
  end

  def OS.unix?
    !OS.windows?
  end

  def OS.ESXi?
		uname = `uname`
		uname.include? "VMkernel"
  end
  
  def OS.linux?
    OS.unix? and not OS.mac?
  end
end

VM_HOST_REPORT_VERSION="0.51"
REPORT_FILE="vmhost_report.log"
TMP_STDOUT_FILE="vmhost_report.stdout.log"
TMP_STDERR_FILE="vmhost_report.stderr.log"

VMW_PATH="bin.aux/linux32/vmw"
VM_BACKDUMP="bin.aux/linux32/vm_backdump"
VSOCKETS_TOOLS_PATH="bin.aux/linux32"
VSOCKETS_HOSTNAME="#{VSOCKETS_TOOLS_PATH}/vsockets_hostname"
VSOCKETS_NC="#{VSOCKETS_TOOLS_PATH}/vsockets_nc"



def vmhost_report_create_log_file
   File.open(REPORT_FILE, 'w') { |file| file.write("#############################################\n") }
end


def vmhost_report_print string
   File.open(REPORT_FILE, 'a') { |file| file.write(string) }
   print string
end

def vmhost_report_print_section_break
    vmhost_report_print "\n---X---X---X---X---X---X---X---X---X---\n\n"
end

def vmhost_report_system_return_output command
   system "#{command} > #{TMP_STDOUT_FILE} 2> #{TMP_STDERR_FILE}"
   output = `cat #{TMP_STDOUT_FILE} #{TMP_STDERR_FILE}` 
   system "cat #{TMP_STDOUT_FILE} #{TMP_STDERR_FILE} >> #{REPORT_FILE}"
   return output
end

#TODO: gravar todos o stderr ...
def vmhost_report_system command
	vmhost_report_system_return_output command
#   return system command+" >> #{REPORT_FILE}"
end

def get_running_user

#Linux + ESXi

	current_user_aux = `env | grep LOGNAME`
	current_user = current_user_aux.split("=")[1]
	
	return current_user
	
end

def check_running_user

	#current_user = `whoami`
	current_user = get_running_user

	vmhost_report_print "current user=#{current_user}\n"

	if current_user <=> "root" then
		vmhost_report_print "===> Running as root, all info available\n"
	else
		vmhost_report_print "===> WARNING: Running as unprivileged user, not all info will be available\n"
	end

	vmhost_report_print_section_break
end

def check_vm_host_version
	vmhost_report_print "#### VM Hypervisor Version \n"
    vmhost_report_print "VM hypervisor version=#{vmhost_get_version}\n"
    vmhost_report_print "patch level=#{vmhost_get_patch_level}\n"
	vmhost_report_print_section_break  
end

def vm_storage_stats

	vmhost_report_print "#### VM Guest devices (visible if isolation is disabled in .vmx)\n"
	vmhost_report_system "vmware-toolbox-cmd device list"

	vmhost_report_print "#### VM Guest virtual drive mountpoints\n"
	vmhost_report_system "vmware-toolbox-cmd disk list"

	print "#### VM storage stats:\n"

	begin  
		device_list_aux = `vmware-toolbox-cmd stat raw | grep vscsi`
		device_list = device_list_aux.split("\n")
	
		vmhost_report_print "device_list=#{device_list} \n"
	
		device_list.each do |device_i|
			vmhost_report_print "####    storage device #{device_i} stats:\n"
			vmhost_report_system "vmware-toolbox-cmd stat raw text #{device_i}"
			vmhost_report_print_section_break
		end
	
		
	rescue
	end

	vmhost_report_print_section_break
end

def vm_networking_stats

	print "#### VM networking stats:\n"

    begin
		device_list_aux = `vmware-toolbox-cmd stat raw | grep vnet`
		device_list = device_list_aux.split("\n")
	
		vmhost_report_print "device_list=#{device_list} \n"
	
		device_list.each do |device_i|
			vmhost_report_print "####    network device #{device_i} stats:\n"
			vmhost_report_system "vmware-toolbox-cmd stat raw text #{device_i}"
			vmhost_report_print_section_break
		end
				
	rescue
	
	end

	begin	
		vmhost_report_print "#### VM ARP networking stats:\n"
		if (!OS.ESXi?)
			vmhost_report_system "arp"
		else
			vmhost_report_system "/bin/esxcfg-route -n"
		end		

		vmhost_report_print "#### VM IP routing networking stats:\n"
		if (!OS.ESXi?)
			vmhost_report_system "route"
		else
			vmhost_report_system "/bin/esxcfg-route -l"
		end

		vmhost_report_print "#### VM IP networking stats:\n"
		if (!OS.ESXi?)
			vmhost_report_system "ifconfig"
		else
			vmhost_report_system "esxcfg-vmknic -l"
		end
		
	rescue
	
	end
	
	vmhost_report_print_section_break
end

def vm_bios_stats

	vmhost_report_print "#### BIOS strings:\n"
	if (!OS.ESXi?)
		vmhost_report_system "dmidecode|grep -i vmware"
	else
		vmhost_report_system "esxcfg-info -w | head -30"
	end
	vmhost_report_print_section_break

	if (!OS.ESXi?)
	
		vmhost_report_print "#### BIOS signature:\n"
		vmhost_report_system "dmidecode|head -16 | tail -10"
		vmhost_report_print_section_break

		vmhost_report_print "#### BIOS Release date:\n"
		vmhost_report_system "dmidecode|grep \"Release Date:\""
		vmhost_report_print_section_break
	
	end

	vmhost_report_print "#### PCI strings:\n"
	vmhost_report_system "lspci"
	vmhost_report_print_section_break

	vmhost_report_print "#### USB devices:\n"
	vmhost_report_system "lsusb"
	vmhost_report_print_section_break
	
	vmhost_report_print "#### CPU features:\n"
	if (!OS.ESXi?)	
		vmhost_report_system "lscpu"
	else
		vmhost_report_system "cat /proc/cpuinfo"
	end
	vmhost_report_print_section_break
	
	vmhost_report_print "#### Kernel modules:\n"
	if (!OS.ESXi?)
		vmhost_report_system "lsmod | grep vm"
	else
		vmhost_report_system "esxcfg-module  -l"
	end
	vmhost_report_print_section_break

end

def vm_script_stats
	vmhost_report_print "#### VM Guest operation scripts\n"
	vmhost_report_print "#### ... power\n"
	vmhost_report_system "vmware-toolbox-cmd script power current"
	vmhost_report_print "#### ... resume\n"
	vmhost_report_system "vmware-toolbox-cmd script resume current"
	vmhost_report_print "#### ... suspend\n"
	vmhost_report_system "vmware-toolbox-cmd script suspend current"
	vmhost_report_print "#### ... shutdown\n"
	vmhost_report_system "vmware-toolbox-cmd script shutdown current"
	vmhost_report_print_section_break
end

def vm_guest_legacy_stats
	vmhost_report_print "#### VM Guest stats (legacy)\n"

	vmhost_report_system "vmware-toolbox-cmd stat hosttime"
	vmhost_report_system "vmware-toolbox-cmd stat speed"
	vmhost_report_system "vmware-toolbox-cmd stat sessionid"
	vmhost_report_system "vmware-toolbox-cmd stat balloon"
	vmhost_report_system "vmware-toolbox-cmd stat swap"
	vmhost_report_system "vmware-toolbox-cmd stat memlimit"
	vmhost_report_system "vmware-toolbox-cmd stat memres"
	vmhost_report_system "vmware-toolbox-cmd stat cpures"
	vmhost_report_system "vmware-toolbox-cmd stat cpulimit"
	vmhost_report_print_section_break
	
	vmhost_report_print "#### VM check stats (legacy)\n"
	vmhost_report_print "#### -- Hypervisor type:\n"
	vmhost_report_system "vmware-checkvm -p"
	vmhost_report_print "#### -- VM hardware version:\n"
	vmhost_report_system "vmware-checkvm -h"
	vmhost_report_system "vmware-checkvm -r"
	vmhost_report_print_section_break

end

def vm_host_raw_stats

  vmhost_report_print "#### Available stats:\n"

  begin  
    
    stats_raw = `vmware-toolbox-cmd stat raw`

	vmhost_report_print "stats_raw=#{stats_raw}\n"

	if stats_raw=="" then
		vmhost_report_print "stats raw not available\n"
	else
		vmhost_report_print "#### Session stats:\n"

		vmhost_report_system "vmware-toolbox-cmd stat raw text session"

		vmhost_report_print "#### Host stats:\n"

		vmhost_report_system "vmware-toolbox-cmd stat raw text host"

		vmhost_report_print "#### VM stats:\n"

		vmhost_report_system "vmware-toolbox-cmd stat raw text resources"
		
		vmhost_report_print_section_break
	end
  rescue  
    puts 'I am rescued.\n'  
  end  
	
end

def vm_guest_tools_stats
	vmhost_report_print "#### VM Guest tools info\n"
	vmhost_report_system "vmtoolsd -v"

	vmhost_report_print "#### Host VM shares (HGFS):\n"
	vmhost_report_system "vmware-hgfsclient"
end

def vmguest_backdoor_info 

	vmhost_report_print "#### VM Guest backdoor low-level info\n"
	vmhost_report_print "#### ... Host screen size\n\n"
	#vmhost_report_system "#{VMW_PATH} s -d -v"
	vmhost_report_system "#{VMW_PATH} s "
	vmhost_report_print_section_break
	
	vmhost_report_print "#### ... BIOS UUID\n\n"
	vmhost_report_system "#{VMW_PATH} u "
	vmhost_report_print_section_break

	vmhost_report_print "#### ... Paste from clipboard\n\n"
	vmhost_report_system "#{VMW_PATH} p"
	vmhost_report_print_section_break

	vmhost_report_print "#### ... List connectable devices\n\n"
	vmhost_report_system "#{VMW_PATH} d"
	vmhost_report_print_section_break

	vmhost_report_print "#### ... RPC: machine.id.get\n\n"
	vmhost_report_system "#{VMW_PATH} rpc -e machine.id.get -v"
	vmhost_report_print "\n#### ... RPC: guestInfo\n"
	vmhost_report_print "#### ..... info-get guestinfo.joebanana\n\n"
	vmhost_report_system "#{VMW_PATH} r \"info-get guestinfo.joebanana\" -v"
	vmhost_report_print "\n"
	vmhost_report_print "#### ..... info-set guestinfo.joebanana\n\n"
	vmhost_report_system "#{VMW_PATH} r \"info-set guestinfo.joebanana banana\" -v"
	vmhost_report_print "\n"
	vmhost_report_print "#### ..... info-get guestinfo.joebanana\n\n"
	vmhost_report_system "#{VMW_PATH} r \"info-get guestinfo.joebanana\" -v"
	vmhost_report_print "\n"
	vmhost_report_print "#### ... RPC: log\n"
	vmhost_report_print "#### ..... log\n\n"
	vmhost_report_system "#{VMW_PATH} r \"log joebanana\" -v"
	vmhost_report_print "\n#### ..... log -e\n\n"
	vmhost_report_system "#{VMW_PATH} r -e \"log joebanana\" -v"
	vmhost_report_print_section_break

	#TODO: embelezar o código de ambos os binários
	
	vmhost_report_print "#### VM Guest backdoor dump (very low-level)\n"
	vmhost_report_system "#{VM_BACKDUMP}"
	vmhost_report_print_section_break

end

def convert_netmask broadcast_address, mask
# Like: 192.168.189.0/24

	broadcast_address_aux = broadcast_address.split(".")
	mask_aux = mask.split(".")
	
	no_of_bits = 0;
	
	netmask = ""
	
	for i in 0..3 
      #vmhost_report_print "#{broadcast_address_aux[i]}  #{mask_aux[i]}\n"
	  
	  byte_broadcast = broadcast_address_aux[i].to_i
	  byte_mask = mask_aux[i].to_i
	  
	  masked_byte_address = byte_broadcast & byte_mask
	  
	  netmask = netmask + "#{masked_byte_address}."
	  
	  bitmask = byte_mask.to_s(2)
	  
	  #vmhost_report_print "#{no_of_bits} #{bitmask}\n"
	  
	  for j in 0..7
	    if (bitmask[j] == '1')
			no_of_bits = no_of_bits + 1
		end
	  end
  
  	 
	end

   netmask = netmask.chomp(".") + "/#{no_of_bits}"
	
   return netmask
end



def vm_network_basic_scan netmask

# TODO: fix hardcoded devices
# get network address+mask from args

	vmhost_report_print "#### VM network basic scan:\n"
	vmhost_report_print "#### network mask: #{netmask}\n"
	vmhost_report_print "#### --- scanning for interesting services:\n"
	output = vmhost_report_system_return_output "nmap -sV --version-all -p 902,912,80,443,53 192.168.189.0/24  | grep -v closed"
	
	vmhost_report_print "#### --- output=#{output}\n"
	
	#host_candidates = output.scan(/Nmap scan report for ([0-9]+.[0-9]+.[0-9]+.[0-9]+)(.*)(?<!VMware)(VMware) /m)
	host_candidates = output.scan(/Nmap scan report for ([0-9]+.[0-9]+.[0-9]+.[0-9]+)(.*?)\(VMware\)/m)
	#host_candidates = host_candidates_aux.split("Nmap scan report for")
	
	vmhost_report_print "#### --- candidates=#{host_candidates}\n"

	# fill return struct
	vmreport_hosts = OpenStruct.new
	vmreport_hosts.others = Array.new
	vmreport_hosts.workstation_player_hypervisors = Array.new
	vmreport_hosts.network_devices = Array.new
	vmreport_hosts.ESXi_hypervisors = Array.new
	
	host_candidates.each do |host_i|  		
		signature=host_i[1]
		
		vmhost_report_print "#### host address=#{host_i[0]} \n\n signature=#{host_i[1]}\n"
		
		if signature.include? "VMware ESXi Server httpd"
			puts "String includes 'VMware ESXi Server httpd', looks like a VMware ESXi Server httpd"
			
			vmreport_hosts.ESXi_hypervisors.insert(0, host_i[0])
			
			next
		end
		
		if signature.include? "VMware Authentication Daemon"
			puts "String includes 'VMware Authentication Daemon', looks like a VMware Player hypervisor"
			
			vmreport_hosts.workstation_player_hypervisors.insert(0, host_i[0])
			
			next
		end
		
		if (signature.include? "apex-mesh") || (signature.include? "dnsmasq")
			puts "String includes 'dnsmasq' or 'apex-mesh', looks like a VMware network device"
			
			vmreport_hosts.network_devices.insert(0, host_i[0])
			
			next
		end
		
		# An unidentified VM
				
		vmhost_report_print "#### VMware virtual machine at IP=#{host_i}\n\n"
		vmreport_hosts.others.insert(0, host_i[0])
		
	end
	
	#vmhost_report_print "#### --- scanning for interesting VMware operating systems:\n"
	#vmhost_report_system "nmap -vv -O --osscan-guess -p 902,912,80,443,8080 #{netmask} | grep 'Nmap scan report for\|guesses\|Running\|Too many fingerprints' | grep -v 'host down'"
  
	vmhost_report_print_section_break
	
	return vmreport_hosts
end

def vm_esxi_basic_scan esxi_host

	vmhost_report_print "### ESXi basic scan:\n"
	vmhost_report_print "### ...ESXi:#{esxi_host}\n"

	vmhost_report_print "### ..... SSL certificate\n"
	vmhost_report_system "echo -n | openssl s_client -connect #{esxi_host}:443 2>/dev/null | openssl x509 -text -noout -issuer -subject"

	vmhost_report_print "### ..... API\n"
	vmhost_report_system "wget --server-response --no-check-certificate -O - https://#{esxi_host}/sdk/vimService.wsdl"

	vmhost_report_print_section_break
end

def vm_host_DNS_services_scan host
	vmhost_report_print "#### DNS services scan for IP=#{host}\n\n"
	vmhost_report_system "nmap -sV --version-all -p 53 #{host}"
end

def vm_host_os_scan host
    vmhost_report_print "#### OS scan for IP=#{host}\n\n"
	vmhost_report_system "nmap -vv -O --osscan-guess -sSU -p 902,912,80,443,8080 #{host}"
end

def vm_handle_network_hosts vmhosts

	vmhosts.workstation_player_hypervisors.each do |host_i|  
				
		vmhost_report_print "#### VMware Player hypervisor at IP=#{host_i}\n\n"
		
		# TODO: fazer mais coisas interessantes ...
		
		vm_host_os_scan host_i
	end

	vmhosts.network_devices.each do |host_i|  
				
		vmhost_report_print "#### VMware network device at IP=#{host_i}\n\n"
		
		# TODO: fazer mais coisas interessantes ...
		vm_host_os_scan host_i
		
		vm_host_DNS_services_scan host_i
	end
	
	vmhosts.ESXi_hypervisors.each do |host_i|  
				
		vmhost_report_print "#### VMware ESXi host at IP=#{host_i}\n\n"
		
		# TODO: fazer mais coisas interessantes ...
		vm_host_os_scan host_i
		
		vm_esxi_basic_scan host_i
	end
	
	vmhosts.others.each do |host_i|  
				
		vmhost_report_print "#### VMware virtual machine at IP=#{host_i}\n\n"
		
		# TODO: fazer mais coisas interessantes ...
		vm_host_os_scan host_i
		
		vm_host_DNS_services_scan host_i
	end
end

def vm_scan_local_networks

	if (!OS.ESXi?)
	
		# call "ifconfig" and get all the entries like:
		# 	inet addr:192.168.189.128  Bcast:192.168.189.255  Mask:255.255.255.0

		local_networks_aux = `ifconfig`
  
		local_networks = local_networks_aux.scan(/inet addr:([0-9]+.[0-9]+.[0-9]+.[0-9]+)  Bcast:([0-9]+.[0-9]+.[0-9]+.[0-9]+)  Mask:([0-9]+.[0-9]+.[0-9]+.[0-9]+)/)
		
	else
	
	    # like:
		#  esxcfg-vmknic -l
		#	Interface  Port Group/DVPort/Opaque Network        IP Family IP Address                              Netmask         Broadcast       MAC Address       MTU     TSO MSS   Enabled Type                NetStack
		#	vmk0       Management Network                      IPv4      192.168.189.134                         255.255.255.0   192.168.189.255 00:0c:29:bd:16:1f 1500    65535     true    DHCP                defaultTcpipStack
		#	vmk0       Management Network                      IPv6      fe80::20c:29ff:febd:161f                64                          
	
		local_networks_aux = `esxcfg-vmknic -l`
		
		local_networks = local_networks_aux.scan(/IPv4      ([0-9]+.[0-9]+.[0-9]+.[0-9]+)(.*?)([0-9]+.[0-9]+.[0-9]+.[0-9]+)(.*?)([0-9]+.[0-9]+.[0-9]+.[0-9]+)/)
	end

  vmhost_report_print "#### VM local networks #{local_networks} \n"
  
  local_networks.each do |network_i|
  
    if (!OS.ESXi?)
		network_address = network_i[1]
		network_mask = network_i[2]
	else
		network_address = network_i[4]
		network_mask = network_i[2]
	end
  
    vmhost_report_print "#### local network address=#{network_address} mask=#{network_mask}\n"
	
	# Like: 192.168.189.0/24
	netmask = convert_netmask network_address, network_mask
	vmhost_report_print "#### local network mask #{netmask}\n"
	
	vmhosts = vm_network_basic_scan netmask
	
	vm_handle_network_hosts vmhosts
	
  end

  
end

def vm_scan_vsockets
	vmhost_report_print "#### Scaning vsockets environment\n"
	vmhost_report_print "####      vsockets properties:\n"
	vmhost_report_system "#{VSOCKETS_HOSTNAME}"
	vmhost_report_print_section_break	
	
	vmhost_report_print "####      scaning open connection-oriented ports on Hypervisor:\n"
	vmhost_report_system "#{VSOCKETS_NC} -n"
	vmhost_report_print_section_break	
end

### MAIN

vmhost_report_create_log_file

NOW = `date`.strip

OSNAME = `uname -a`

vmhost_report_print "#### VMware Host Fingerprinting Report from Guest\n"
vmhost_report_print "####\n"
vmhost_report_print "#### (Starting at #{NOW})\n"
vmhost_report_print "####\n"
vmhost_report_print "#### Tool version: #{VM_HOST_REPORT_VERSION}\n"
vmhost_report_print "#### Platform:     #{RUBY_PLATFORM} - #{OSNAME} \n"

check_running_user

if (!OS.ESXi?)
	check_vm_host_version
else
    vmhost_report_print "#### Nested Hypervisor ESXi\n"
	vmhost_report_print "####        Guest Hypervisor Version\n"
	vmhost_report_system "vmware -vl"
	vmhost_report_print_section_break	
end


# Basic stats

vm_bios_stats

vm_guest_legacy_stats

vm_host_raw_stats

vm_guest_tools_stats

vm_script_stats

vm_storage_stats

vm_networking_stats

vm_scan_vsockets


# Advanced stats

vmguest_backdoor_info

vm_scan_local_networks



# ENDING, log to file

END_TIME = `date`.strip

vmhost_report_print "#### (Ending at #{END_TIME})\n"



