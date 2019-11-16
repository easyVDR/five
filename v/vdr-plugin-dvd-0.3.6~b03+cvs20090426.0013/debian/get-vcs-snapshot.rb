File.open("debian/changelog").first =~ /(.*) \((.*)\+.*-.*\)/
package = $1
upstream_version = $2
cvs_version=`date --utc +%0Y%0m%0d.%0k%0M`.chomp
new_version="#{upstream_version}+cvs#{cvs_version}"

puts "Downloading #{package} version #{new_version}"

puts `cvs -d:pserver:anonymous@dvdplugin.cvs.sourceforge.net:/cvsroot/dvdplugin export -D NOW -d #{package}-#{new_version} dvd`
puts `tar czf ../#{package}_#{new_version}.orig.tar.gz #{package}-#{new_version}`
puts `rm -rf #{package}-#{new_version}`
puts `dch -v "#{new_version}-1" "New Upstream Snapshot"`
