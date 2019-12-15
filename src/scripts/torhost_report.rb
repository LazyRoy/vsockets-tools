#!/usr/bin/ruby

print "$$$$ TOR Host Analysis Report [Version 0.10]\n"
print "$$$$\n"
print "$$$$ Analyzing local host TOR environment ...\n"
print "$$$$ (Starting at ", `date`.strip, ")\n"


print "\n$$$$ Important TOR configuration files:\n"
print "\n$$$$ torrc\n\n"
system 'cat /etc/tor/torrc'

print "\n$$$$ /usr/share/tor/tor-service-defaults-torrc\n\n"
system 'cat /usr/share/tor/tor-service-defaults-torrc'


print "\n\n$$$$ Important configuration folders\n"
print "\n$$$$ /etc/whonix.d/\n\n"
system 'ls -l /etc/whonix.d/'
print "\n$$$$ /etc/whonix_firewall.d\n\n"
system 'ls -l /etc/whonix_firewall.d/'

print "\n\n$$$$ Important logs:\n\n"
print "\n\n$$$$ /var/log/syslog\n\n"
system 'tail -40 /var/log/syslog'

print "\n\n$$$$ /var/log/sdwdate.log\n\n"
system 'tail -40 /var/log/sdwdate.log'

print "\n\n$$$$ /var/log/control-port-filter-python.log\n\n"
system 'tail -40 /var/log/control-port-filter-python.log'

print "\n\n$$$$ /var/log/tor/log\n\n"
system 'tail -40 /var/log/tor/log'


print "\n\n$$$$ is whonix running ?\n\n"
system 'whonix'
print "\n\n$$$$$$$$ whonixcheck connection to TOR \n\n"
system 'whonixcheck'

print "\n\n$$$$ is arm available ?\n\n"
system 'arm -h'
system 'arm -v'

print "\n\n$$$$ is tor [service] running ?\n\n"
system 'service --status-all | grep tor'
system 'ps aux | grep "/usr/bin/tor" | grep -v grep'

print "\n\n$$$$$$$$ TOR version\n\n"
system 'tor-ctrl -v -c "GETINFO version"'

print "\n\n$$$$$$$$ is tor-prompt available ?\n\n"
system 'tor-prompt -h'

print "\n\n$$$$$$$$ is torsocks available ?\n\n"
system 'torsocks'

print "\n\n$$$$$$$$ is tor-resolve available ?\n\n"
system 'tor-resolve'

print "\n\ntor-resolve www.google.com\n\n"
system 'tor-resolve www.google.com'


print "\n\n$$$$ is networking [service] running ?\n\n"
system 'service --status-all | grep networking'

print "\n\n$$$$ Network configuration\n\n"
system 'ifconfig'
print "\n\n$$$$$$$$ iptables\n\n"
system 'iptables --list'

print "\n\n$$$$$$$$ active connections IPv4\n\n"
system 'netstat -4 -a -e -p -v'

print "\n\n$$$$$$$$ My Exit Relay IP\n\n"
MY_EXTERNAL_IP = `wget -O - -q http://ipecho.net/plain ; echo`
print "IP=#{MY_EXTERNAL_IP}\n"
system 'wget -O - -q http://api.hackertarget.com/reversedns/?q='+MY_EXTERNAL_IP
print "\n"

print "\n\n$$$$$$$$ uname\n\n"
system 'uname -a'
print "\n\n$$$$$$$$ hostname\n\n"
system 'hostname'
print "\n\n$$$$$$$$ dnsdomainname\n\n"
system 'dnsdomainname'

print "\n\n$$$$ (Ending at ", `date`.strip, ")\n"




