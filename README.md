vallumd
=======

This program allows you to centralize and distribute IP blacklists.

If you maintain a server on the Internet, it's very likely you encountered
one or more brute force attacks. Not a problem, just install fail2ban. Done.

But if you're running multiple servers, each of them running their fail2ban
instance, they'll all have different IP addresses in the ban list. Wouldn't
it be nice to have a shared ban list across all your fail2ban instances?
Or in case all your machines are behind a router or firewall you control
yourself, wouldn't it be nice to drop malicious traffic at the edge of your
network?

That's exactly what vallumd helps to achieve.


How it works
------------

Vallumd connects to an MQTT broker, reads messages containing IP addresses,
and adds or deletes those IP addresses to or from an ipset. Simple as that.

This means it is not useful on its own, but it makes vallumd very flexible.
You can decide for yourself what kind of iptables rule you want to reference
the ipset in. Integrating it with fail2ban is as simple as creating a new
action that uses mosquitto_pub to send a message to your MQTT broker.
And since there are MQTT libraries out there for most common languages, it
shouldn't be too hard to integrate with your favorite IDS, IPS or Honeypot.


Why the silly name
------------------

The name vallumd comes from the Latin word "vallum", which means wall.
And that's what vallumd does: build a wall to protect your digital empire.


Installation
------------

The nicest way to install any package, is by using your distribution's
package management. However, since this project is very young, it has not been
included in many distributions yet.

### CentOS/RedHat

Make sure you have the EPEL repository enabled.


#### CentOS 7 dependencies
```
sudo yum -y install cmake3 ipset-devel mosquitto-devel openssl-devel pkgconfig '@Development Tools'
```

#### Common
You can now generate an RPM package with cpack:
```
git clone https://codeberg.org/stintel/vallumd.git
cd vallumd
cmake3 .
cpack3 -G RPM

sudo yum -y localinstall build/*.rpm
```
Build-tested on CentOS 7.

### Debian/Ubuntu

You can generate a DEB package with cpack:
```
sudo apt-get -y install build-essential cmake file libipset-dev libmosquitto-dev libssl-dev pkg-config

git clone https://codeberg.org/stintel/vallumd.git
cd vallumd
cmake .
cpack -G DEB

sudo dpkg -i build/*.deb
```
Build-tested on Debian 10, 11, 12, 13 and Ubuntu 18.04, 20.04, 22.04, 24.04.

### Gentoo
You can find a live ebuild for vallumd in my [Gentoo overlay](https://github.com/stintel/gentoo-overlay)

### OpenWrt/LEDE

If you're running an OpenWrt DD snapshot or LEDE, vallumd is available
in the packages feed, and can be installed with opkg:
```
opkg update
opkg install vallumd
```

### Manual install
Manual installation is very easy.
Requirements:
* cmake
* libipset
* libmosquitto
* libssl

Instructions:
```
git clone https://codeberg.org/stintel/vallumd.git
cd vallumd
cmake .
make
make install
```


Usage
-----

To use vallumd, you need an MQTT broker, like [Mosquitto](https://mosquitto.org/).
Depending on your setup, you can run it on the same host that runs vallumd,
but that's no requirement.

The next thing you need is an IPset. To give you full control over the type
of IPset and its options, vallumd will not create the IPset itself.
You can choose between these IPset types:
* bitmap:ip
* bitmap:net
* hash:ip
* hash:net

IPset creation example:
`ipset create blacklist hash:ip timeout 3600`

Now you can start vallumd. The following command line options exist:
```
 -h: MQTT host to connect to
 -p: MQTT port to connect to (1883)
 -u: MQTT username
 -P: MQTT password
 -t: MQTT topic and IPset name
 -V: print version number and exit
 -c: path to CA file
 -T: use TLS
```
The host and topic options are required, the others are optional (default
value). It is possible to specify multiple topics.

Starting vallumd:
`vallumd -h 192.168.0.1 -t blacklist`

This will listen for messages on the MQTT broker at 192.168.0.1, in the
blacklist topic, and when a message is received, the IP address in the message
will be added to or remove from the IPset named blacklist.

So now we have everything in place to start adding IPs to the blacklist.
All we have to do is configure our IDS, IPS or Honeypot to send malicious
IP addresses to our MQTT broker.

For fail2ban, this could be done with the Mosquitto client `mosquitto_pub`.
Create a new action in `/etc/fail2ban/action.d/vallumd.conf`:
```
[Definition]
actionban = mosquitto_pub -h 192.168.0.1 -q 2 -t blacklist/add -m <ip>
actionunban = mosquitto_pub -h 192.168.0.1 -q 2 -t blacklist/del -m <ip>
```
And configure your fail2ban jails to use the vallumd action.


Running as a service
--------------------

The packages generated with CPack support OpenRC, Upstart and systemd.
Service configuration files are where you would expect them in your distro:
* CentOS/RedHat: /etc/sysconfig/vallumd
* Debian/Ubuntu: /etc/default/vallumd
* Gentoo: /etc/conf.d/vallumd
* OpenWrt/LEDE: /etc/config/vallumd

After editing the file relevant for your distribution, start the vallumd service
just like you would start any other service on your system.


Logging
-------
Vallumd uses syslog(3) for logging.


Get involved
------------

* Join the #vallumd IRC channel on irc.oftc.net
