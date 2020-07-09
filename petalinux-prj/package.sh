helpFunction(){
    echo "Usage: $0 [<qspi/sd>]";
}

echo "";
if [[ $1 == "sd" ]]
then
    echo "===== Generate image for sd card =====";
    petalinux-package --boot --bif boot.bif --boot-device sd -o images/linux/BOOT.bin --force;
    echo "";
    echo "Image generated: images/linux/BOOT.bin";
elif [[ $1 == "qspi" ]]
then
    echo "===== Generate image for qspi =====";
    petalinux-package --boot --bif boot.bif --boot-device flash -o images/linux/QSPI.bin --force;
    echo "";
    echo "Image generated: images/linux/QSPI.bin";
else
    helpFunction;
fi

echo "";
exit 1;
