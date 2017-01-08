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
and adds those IP addresses to an ipset. Simple as that.

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
included in any distribution yet.

### Gentoo
You can find a live ebuild for vallumd in my [Gentoo overlay](https://github.com/stintel/gentoo-overlay)

### Manual install
Manual installation is very easy.
Requirements:
* cmake
* libipset
* libmosquitto

Instructions:
```
git clone https://github.com/stintel/vallumd.git
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

As vallumd only adds IP addresses to an IPset, it's recommend to use the
timeout option while creating the IPset. When adding an element to an
IPset with the timeout option set, elements added to the set will be
automatically removed when the timeout expires. This value is in seconds.

IPset creation example:
`ipset create blacklist hash:ip timeout 3600`

Now you can start vallumd. The following command line options exist:
```
 -h: MQTT host to connect to
 -p: MQTT port to connect to (1883)
 -t: MQTT topic and IPset name (vallumd)
 -V: print version number and exit
```
The host option is required, the others are optional (default value).

Starting vallumd:
`vallumd -h 192.168.0.1 -t blacklist`

This will listen for messages on the MQTT broker at 192.168.0.1, in the
blacklist topic, and when a message is received, the IP address in the message
will be added to the IPset named blacklist.

So now we have everything in place to start adding IPs to the blacklist.
All we have to do is configure our IDS, IPS or Honeypot to send malicious
IP addresses to our MQTT broker.

For fail2ban, this could be done with the Mosquitto client `mosquitto_pub`.
Create a new action in `/etc/fail2ban/action.d/vallumd.conf`:
```
[Definition]
actionban = mosquitto_pub -h 192.168.50.5 -t vallumd -m <ip>
```
And configure your fail2ban jails to use the vallumd action.
