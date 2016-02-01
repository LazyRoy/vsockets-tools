#!/bin/bash
echo "#### VMware Host Fingerprinting Report from Guest"
echo "####"
echo "#### (Starting at " $(date)")"

echo "#### Available stats:"

vmware-toolbox-cmd stat raw

echo "#### Session stats:"

vmware-toolbox-cmd stat raw text session

echo "#### Host stats:"

vmware-toolbox-cmd stat raw text host

echo "#### VM stats:"

vmware-toolbox-cmd stat raw text resources

echo "#### VM Guest stats (legacy)"

vmware-toolbox-cmd stat hosttime
vmware-toolbox-cmd stat speed
vmware-toolbox-cmd stat sessionid
vmware-toolbox-cmd stat balloon
vmware-toolbox-cmd stat swap
vmware-toolbox-cmd stat memlimit
vmware-toolbox-cmd stat memres
vmware-toolbox-cmd stat cpures
vmware-toolbox-cmd stat cpulimit

echo "#### VM Guest devices (visible if isolation is disabled in .vmx)"
vmware-toolbox-cmd device list

echo "#### VM Guest virtual drive mountpoints"
vmware-toolbox-cmd disk list

echo "#### VM Guest operation scripts"
echo "#### ... power"
vmware-toolbox-cmd script power current
echo "#### ... resume"
vmware-toolbox-cmd script resume current
echo "#### ... suspend"
vmware-toolbox-cmd script suspend current
echo "#### ... shutdown"
vmware-toolbox-cmd script shutdown current

echo "#### VM Guest tools stats"
vmtoolsd -v

echo "#### Host VM shares (HGFS):"
vmware-hgfsclient

echo "#### VM Guest backdoor low-level info"
echo "#### ... Host screen size"
vmw s -d -v
echo "#### ... BIOS UUID"
vmw u -v -d
echo "#### ... Paste from clipboard"
vmw p -v -d
echo "#### ... List connectable devices"
vmw d -v -d
echo "#### ... RPC: machine.id.get"
vmw rpc -e machine.id.get -v
echo "#### ... RPC: guestInfo"
echo "#### ..... info-get guestinfo.joebanana"
vmw r "info-get guestinfo.joebanana" -v
echo -n
echo "#### ..... info-set guestinfo.joebanana"
vmw r "info-set guestinfo.joebanana banana" -v
echo -n
echo "#### ..... info-get guestinfo.joebanana"
vmw r "info-get guestinfo.joebanana" -v
echo -n
echo "#### ... RPC: log"
echo "#### ..... log"
vmw r "log joebanana" -v
echo "#### ..... log -e"
vmw r -e "log joebanana" -v


echo "#### VM check stats (legacy)"
echo "#### -- Hypervisor type:"
vmware-checkvm -p
echo "#### -- VM hardware version:"
vmware-checkvm -h
vmware-checkvm -r

echo "#### VM storage stats:"

vmware-toolbox-cmd stat raw text vscsi scsi0:0

vmware-toolbox-cmd stat raw text vscsi ide0:0

echo "#### VM networking stats:"

vmware-toolbox-cmd stat raw text vnet 00:0c:29:1e:23:f3
vmware-toolbox-cmd stat raw text vnet 00:0c:29:32:0a:6a

echo "#### VM ARP networking stats:"
arp
/bin/esxcfg-route -n

echo "#### VM IP routing networking stats:"
route
/bin/esxcfg-route -l

echo "#### VM IP networking stats:"
ifconfig

echo "#### BIOS strings:"
dmidecode|grep -i vmware
esxcfg-info -w | head -30

echo "#### BIOS signature:"
dmidecode|head -16 | tail -10

echo "#### BIOS Release date:"
dmidecode|grep "Release Date:"

echo "#### PCI strings:"
lspci

echo "#### USB devices:"
lsusb

echo "#### CPU features:"
lscpu
cat /proc/cpuinfo

echo "#### Kernel modules:"
lsmod | grep vm
esxcfg-module  -l

echo "#### vsockets environment:"
vsockets_hostname

echo "#### VM network basic scan:"
echo "#### --- scanning for interesting services:"
nmap -sV --version-all -p 902,912,80,443 192.168.189.0/24  | grep -v closed
echo "#### --- scanning for interesting VMware operating systems:"
nmap -vv -O --osscan-guess -p 902,912,80,443,8080 192.168.189.0/24 | grep 'Nmap scan report for\|guesses\|Running\|Too many fingerprints' | grep -v 'host down'

echo "### ESXi basic scan:"
echo "### ...ESXi: 192.168.189.134"
echo "### ..... SSL certificate"
echo -n | openssl s_client -connect 192.168.189.134:443 2>/dev/null | openssl x509 -text -noout -issuer -subject
echo "### ..... API"
wget --server-response --no-check-certificate -O - https://192.168.189.134/sdk/vimService.wsdl

echo "#### (Finished at " $(date)")"
