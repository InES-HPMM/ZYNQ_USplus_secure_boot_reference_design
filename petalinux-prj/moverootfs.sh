echo "";
echo "Mount rootfs to /tftpboot/nfsroot";
echo "================================";
rm -rf /tftpboot/nfsroot/*;
tar -xvzf images/linux/rootfs.tar.gz -C /tftpboot/nfsroot;
echo "";
