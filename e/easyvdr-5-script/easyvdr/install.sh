#!/bin/bash

read -p"Installation der easyVDR Pakete durchführen (j/n)? " response
if [ "$response" != "n" ]; then


    echo "Installation gestartet."

    #Murks solange nötig
    sudo mkdir -p /etc/lirc/
    sudo ln -s /etc/lirc/lirc_options.conf.dist /etc/lirc/lirc_options.conf


    sudo debconf-set-selections <<< "postfix postfix/mailname string easyvdr"
    sudo debconf-set-selections <<< "postfix postfix/main_mailer_type string 'No configuration'"
    
    #mal sehen ob das so gut ist...
    sudo debconf-set-selections <<<  "shared shared/default-x-display-manager select lightdm"
    
    
    #Nicht erforderlich wenn die Paketquellen bereits durch Preseed hinzugefügt wurden... 
    #sudo add-apt-repository -y --no-update ppa:easyvdr-team/5-base-unstable
    #sudo add-apt-repository -y --no-update ppa:easyvdr-team/5-vdr-unstable
    #sudo add-apt-repository -y ppa:easyvdr-team/5-others-unstable
    
    
    sudo apt dist-upgrade -y
    sudo apt install -y easyvdr-5
    sudo apt install -y easyvdr-installer
#    sudo systemctl daemon-reload >/dev/null || true
#    sudo systemctl enable "/etc/systemd/easyvdr/easyvdr-setup-call.service" || true > /dev/null 2>&1
    
    sleep 5
    
    echo reboote jetzt
    sudo reboot
    
fi

echo Ende

