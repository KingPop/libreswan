sed -i "s/seedbits=.*$/seedbits=520/" /etc/ipsec.conf
ipsec start
/testing/pluto/bin/wait-until-pluto-started
ipsec stop
egrep "bits random|bytes from|seeded" /tmp/pluto.log
sed -i "s/seedbits=.*$/seedbits=1024/" /etc/ipsec.conf
ipsec start
/testing/pluto/bin/wait-until-pluto-started
ipsec stop
egrep "bits random|bytes from|seeded" /tmp/pluto.log
sed -i "s/seedbits=.*$/seedbits=2048/" /etc/ipsec.conf
ipsec start
/testing/pluto/bin/wait-until-pluto-started
ipsec stop
egrep "bits random|bytes from|seeded" /tmp/pluto.log
test -f /usr/local/libexec/ipsec/pluto && PLUTOBIN="/usr/local/libexec/ipsec/pluto"
test -f /usr/libexec/ipsec/pluto && PLUTOBIN="/usr/libexec/ipsec/pluto"
./checksec.sh --file $PLUTOBIN
