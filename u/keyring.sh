
#############################
# processing ubuntu-keyring #
#############################

cd keyring
apt-get source ubuntu-keyring
cd ubuntu-keyring-*/keyrings
gpg --import < ubuntu-archive-keyring.gpg
PACKAGE_SIGN=`gpg --list-keys DFFB3809 | grep uid | sed 's/uid *//'`
gpg --export FBB75451 437D05B5 DFFB3809 > ubuntu-archive-keyring.gpg
cd ..
#dpkg-buildpackage -rfakeroot -m"$PACKAGE_SIGN" -kDFFB3809
#cd ..
#cp ubuntu-keyring*deb $ISO_REMASTER_DIR/pool/main/u/ubuntu-keyring

##### SHA512 Key anpassen !!!!
# cat ubuntu-archive-keyring.gpg | sha512sum
##### Copy the hash and replace it in the SHA512SUMS.txt.asc file