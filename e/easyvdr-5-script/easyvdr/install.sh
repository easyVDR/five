#!/bin/bash
debconf-set-selections <<< "postfix postfix/mailname string easyvdr"
debconf-set-selections <<< "postfix postfix/main_mailer_type string 'No configuration'"

#mal sehen ob das so gut ist...
debconf-set-selections <<<  "shared shared/default-x-display-manager select lightdm"

sudo add-apt-repository -y --no-update ppa:easyvdr-team/5-base-unstable
sudo add-apt-repository -y --no-update ppa:easyvdr-team/5-vdr-unstable
sudo add-apt-repository -y ppa:easyvdr-team/5-others-unstable
sudo apt dist-upgrade -y
sudo apt install -y easyvdr-5
sudo systemctl daemon-reload >/dev/null || true
sudo systemctl enable "/etc/systemd/easyvdr/easyvdr-setup-call.service" || true > /dev/null 2>&1

sleep 5

echo reboote jetzt
sudo reboot
